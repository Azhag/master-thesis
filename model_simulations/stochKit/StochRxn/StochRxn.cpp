//*****************************************************************************|
//*  FILE:    StochRxn.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
//*
//*  LAST MODIFIED: July 7, 2003 
//*             BY: Yang Cao 
//*             TO:  
//*
//*  LAST MODIFIED: Mar 04, 2005 
//*             BY: Hong Li
//*             TO: Combine Yang's version and Andrew's version
//*  LAST MODIFIED: April 28, 2005
//* 		BY: Yang Cao
//* 		TO: 
//*  LAST MODIFIED: July 28, 2005
//* 		BY: Hong Li 
//* 		TO: 
//*
//*
//*  SUMMARY:
//*
//*
//*  NOTES:
//*
//*
//*  TO DO:
//*
//*
//*****************************************************************************|
//       1         2         3         4         5         6         7         8
//345678901234567890123456789012345678901234567890123456789012345678901234567890
#include "StochRxn.h"
#include "ExplicitTau_SingleStep.h"
#include "ImplicitTau_SingleStep.h"
#include "GillespieStepsize.h"
#include "GillespiePetzoldStepsize.h"
#include "SSA.h"
#include "Random.h"
#include "MatrixOps.h"  
#include "IEEE.h"
#include "VectorOps.h"
#include <iostream>
#include <fstream>
#define FixedStep 0
// #define AdaptiveStep 1

#define NC 10
#define AtleastLeap 10
#define FailProtect 100   

namespace CSE {
  namespace StochRxn {


    SolutionHistory StochRxn(const Vector& x0,
                             const double t0,
                             const double tf,
                             const ReactionSet& reactions,
                             const SolverOptions& options)
    {
      // Unpack the solver options structure into usable form:
     
      // STEP 1:  Configure the solver using the options parameter:
      //
      // 1a) Set the function pointers for the stepsize, single step, and
      //     history buffer management functions.
      //
      const StepsizeSelectorFunc selectStepsize
        = options.stepsize_selector_func;
      const SingleStepFunc takeStep
        = options.single_step_func;
      const StoreStateFunc storeState
        = options.store_state_func;
 
      // 1b) Now, set solver parameters such as error tolerances
      // 
      const double ABS_TOL = options.absolute_tol;
      const double REL_TOL = options.relative_tol;
      const double EPS = options.epsilon;
      const double INIT_STEP = options.initial_stepsize;
      const int STEP_CONTROL = options.StepControl;     
      const int PROGRESS_INTERVAL = options.progress_interval;    
      
      // STEP 2:  Get the relevant bits from the ReactionSet parameter
      //          (specifically, the propensity and jacobian functions and
      //          the reaction matrix nu;
      //
      const PropensityFunc propensity = reactions.Propensity();
      const Matrix nu = reactions.Nu();

      PartialPropensityFunc partialPropensity = NULL;
      Matrix dg;
      PropensityJacobianFunc propJac;
      EquilibriumFunc equilibrium = NULL;

      if(takeStep == OSSA_SingleStep){ 
          partialPropensity = reactions.PartialPropensity();
          dg = reactions.DG();
      }
      propJac = reactions.PropensityJacobian();
      if(takeStep == MSSA_SingleStep)
        equilibrium =  reactions.Equilibrium();
      
      // STEP 3:  Prepare local solver state
      //
      double t = t0;
      double tau = INIT_STEP;
      Vector x = x0;
      Vector a(nu.Cols(), 0.0); // current propensity
      double a0; 
      SolutionHistory history;
      bool finished = false;
      int steps = 0;

	// These variables are used in the adaptive tau-leaping method
      int SSAcheck = 0; 
      int Ncritical = 0, Nnormal, i, j, subj = -1, rxn = -1; 
      static int N = x.Size(); 
      static int M = a.Size(); 
      static double r1, tau2; 
      static Vector p = a; 
      static int Method = 1; // 1 = SSA, 0 = Tau without critical 2 = Tau with critical
      double suba0, jsum; 
      Vector Reindex(M); 
      
      // STEP 4:  Let 'er rip:
      //
      /********************************************************************/
      /*   MAIN SIMULATION LOOP                                           */
      /********************************************************************/
      if( STEP_CONTROL == FixedStep)
      {
          int rxn = 0;
          if(takeStep == MSSA_SingleStep)
          {
             a = propensity(x);
             equilibrium(x, a, -1);
          } 
          a = propensity(x);
          a0 = a.Sum();	//Hong Jul 21
          while (!finished) {
        
            // 4a)  Compute current reaction propensity:
            //
            if(takeStep != OSSA_SingleStep){
               a0 = a.Sum();	//Hong Jul 21
	    }
            // 4b) Determine stepsize to use for this iteration:
            //
            if ((t >= tf) || (Math::AlmostZero(a0, 1e-15))) {
              // We just took the last step
              finished = true;
              tau = 0.0;
            }
            else {
              tau = selectStepsize(x, a, a0, nu, tau, EPS, propJac);
              // Cut this tau if necessary to avoid overshooting final time:
              if (t + tau >= tf) {
                 if ((takeStep == SSA_SingleStep)||(takeStep == OSSA_SingleStep)||(takeStep == MSSA_SingleStep))  {
 	            finished = true; 
                  } else 
                 tau = tf - t; 
              }
            }

            // 4c) Append current state (including next stepsize tau) to history
            //     buffer (that is, do whatever storeState does):
            if ( ++steps % PROGRESS_INTERVAL == 0) {
              storeState(history, SolutionPt(t, x, tau));
            }

            // 4d) Now, make a single step to time t + tau:
            if (!finished) {
              takeStep(x, t, tau, a, a0, nu, propensity, propJac, ABS_TOL, REL_TOL, rxn, p);
	      if ( (takeStep != SSA_SingleStep) &&  (takeStep != OSSA_SingleStep) &&(takeStep != MSSA_SingleStep))  {   
	        x += nu*p; 
	      } 
              t += tau;

	      if (takeStep == MSSA_SingleStep) {
                 equilibrium(x, a, -1);
	/* negative states could arise if the equilibrium solution is very close to 0. 
                 for (int i = 0; i < x0.Size(); i++)
                   if (x(i) < 0 ) {
                     x(i) = 0;
                 }
	*/
              }

              if (takeStep == OSSA_SingleStep) {
		a = partialPropensity(rxn,x,dg, a, a0);
              } else
                a = propensity(x);
              }

	      for (int i = 0; i < x0.Size(); ++i) {
                if (x(i) < 0.0) {
                  x -= nu*p;
                  t -= tau;
                  break;
                }
              }  
            }
     } else // Added by Yang Cao for adaptive tau-leaping. 4/28/2005
     {
	
  		while (!finished) { 
		  a = propensity(x); 
	  a0 = a.Sum(); 
	  if ((t >= tf) || (Math::AlmostZero(a0, 1e-15))) {
            // We just took the last step
            tau = 0.0;
            finished = true;
          }
          else if (SSAcheck < 1) {      
	    tau = selectStepsize(x, a, a0, nu, tau, EPS, propJac); 
	    if ( tau < AtleastLeap/a0) {
              Method = 1;
              SSAcheck = FailProtect;
              // tau = -log( CSE::Math::UniformRandom() )/a0;
            } else {
              Method = 0; 
	      Ncritical = 0;
              Nnormal = 0;
              suba0   = 0;
              for ( j = 0; j < M; j++) {
                i = 0;
                if ( a(j) > 0) {
                  for (i = 0; i < N; i++) {
                    if ( nu(i, j) < 0 && x(i)/(-nu(i,j)) < NC) {
                      suba0 += a(j);
                      Ncritical++;
                      Reindex(M - Ncritical) = j;
                      break;
                    }
                  }
                }
                if ( i >=N || a(j) == 0) {
                  Reindex(Nnormal) = j;
                  Nnormal ++ ;
                }
              }
	      subj = -1;
              tau2 = tau + 1;;
              if ( suba0 > 0) {
                tau2 = -log( CSE::Math::UniformRandom() )/suba0;
              } 
	      if ( tau2 <= tau) {
                tau = tau2;
                Method = 2;
                r1 = CSE::Math::UniformRandom() * suba0;
                jsum = 0.0;
                while (jsum < r1) {
                  ++subj;
                  jsum += a((int)Reindex(M - 1 - subj));
                }
                subj = (int) Reindex( M - 1 - subj );
              }

              if (t + tau >= tf) {
                tau = tf - t;
                subj = -1;
              } 
	    } 
	  } else {
            SSAcheck --;
            Method = 1;
          } 
	  if (Method == 1) {
            tau = -log( CSE::Math::UniformRandom() )/a0;
            if ( t + tau > tf ) finished = true;
          }

          if (!finished) {
            if (Method != 1) {
	      takeStep(x, t, tau, a, a0, nu, propensity, propJac, ABS_TOL, REL_TOL, rxn, p); 
              for ( i = 0; i < Ncritical; i ++)
                p( (int)Reindex(M -1 - i) ) = 0;
              if (subj != -1 ) p(subj) = 1;

              x += nu*p;
              t += tau;
              } else {
               r1 = CSE::Math::UniformRandom() * a0;

                rxn = -1;
                jsum = 0.0;

              while ((jsum < r1) && (rxn < M)) {
                ++rxn;
                jsum += a(rxn);
              }

              x += nu.Col(rxn);
              t += tau;
	         } 
              for (int i = 0; i < x0.Size(); ++i) {
                if (x(i) < 0.0) {
                x -= nu*p;
                t -= tau;   
		break; 
	      }
	    } 
	    if (++steps % PROGRESS_INTERVAL == 0) {
              storeState(history, SolutionPt(t, x, tau));
	      // std::cout << t << '\t' << tau << '\t' << a0 << '\t' << Method << '\n';  
            }   
	  }
	}
      }
      /********************************************************************/
      /*   END OF MAIN SIMULATION LOOP                                    */
      /********************************************************************/
      storeState(history, SolutionPt(t, x, tau));

      return history;
    }

    void WriteHistoryFile(const SolutionHistory& hist,
                          const std::string& fileName)
    {
        std::ofstream outFile(fileName.c_str());
 
        for (unsigned int i = 0; i < hist.size(); ++i) {
	        const SolutionPt& sp = hist[i];
	        const Vector& st = sp.State();

            outFile << sp.Time();
                                             
            for (int j = 0; j < st.Size(); ++j) {
	            outFile << ' ' << st(j);
	        }
	        outFile << '\n';
        }
    }

  } // Close StochRxn namespace
} // Close CSE namespace
