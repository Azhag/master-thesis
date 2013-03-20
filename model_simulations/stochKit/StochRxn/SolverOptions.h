#ifndef CSE_STOCHRXN_SOLVEROPTIONS_H
#define CSE_STOCHRXN_SOLVEROPTIONS_H
//*****************************************************************************|
//*  FILE:    SolverOptions.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
//*
//*  LAST MODIFIED:  
//*             BY:  
//*             TO:  
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
#include "MatrixFwd.h"
#include "VectorFwd.h"
#include "SolutionHistory.h"
#include "SolutionPt.h"
#include "Propensity.h"


namespace CSE {
  namespace StochRxn {

	// Some typedefs to clarify things:
    /*
    typedef double (*StepsizeSelectorFunc)( Vector& x,
                                           Vector& a,
                                           double& a0,
                                           const Matrix& nu,
                                           double tau,
                                           double eps);
    */
    typedef double (*StepsizeSelectorFunc)( Vector& x,
                                           Vector& a,
                                           double& a0,
                                           const Matrix& nu,
                                           double tau,
                                           double eps,
                                           PropensityJacobianFunc j);

    typedef void (*SingleStepFunc)(Vector& x,
                                   double t,
                                   double& tau,
                                   Vector& a,
                                   double a0,
                                   const Matrix& nu,
                                   PropensityFunc propFunc,
                                   PropensityJacobianFunc propJacFunc,
                                   double abs_tol,
                                   double rel_tol,
                                   int& RXN, 
				                   Vector& p);
/*
    typedef void (*SingleStepFunc)(Vector& x,
                                   double t,
                                   double& tau,
                                   Vector& a,
                                   double a0,
                                   const Matrix& nu,
                                   PropensityFunc propFunc,
                                   double abs_tol,
                                   double rel_tol,
                                   int& RXN, 
				                   Vector& p);
*/
    typedef void (*StoreStateFunc)(SolutionHistory& hist, const SolutionPt& x);
 
    struct SolverOptions
    {
      StepsizeSelectorFunc   stepsize_selector_func;
      SingleStepFunc         single_step_func;
      StoreStateFunc         store_state_func;
      double                 absolute_tol;
      double                 relative_tol;
      double                 epsilon;
      double                 initial_stepsize;
      int                    progress_interval;
      int                    StepControl;
    };


    SolverOptions ConfigStochRxn(int fixedoption);

    SolverOptions ConfigStochRxn();



  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_SOLVEROPTIONS_H
