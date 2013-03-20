//******************************************************************************
//*  FILE:   SingleDriver.cpp
//*
//*  AUTHOR: HongLi
//*
//*  CREATED: Mar 06, 2005
//*
//*  LAST MODIFIED:
//*             BY:
//*             TO:
//*  SUMMARY:
//*
//*
//*  NOTES:
//*
//*
//*
//*  TO DO:
//*
//*
//****************************************************************************|
//        1         2         3         4         5         6         7         8
//2345678901234567890123456789012345678901234567890123456789012345678901234567890

#include "StochRxn.h"
#include "ProblemDefinition.h"
#include "SolverOptions.h"
#include "CollectStats.h"
#include "SSA.h"
#include "Vector.h"
#include "Matrix.h"
#include "FixedStepsize.h"
#include "ImplicitTau_SingleStep.h"
#include "ExplicitTau_SingleStep.h"
#include "Trapezoidal_SingleStep.h"
#include "ImplicitTrapezoidal_SingleStep.h"
#include "Midpoint_SingleStep.h"
#include "Vector.h"
#include "Matrix.h"
#include "Random.h"
#include "StoreState.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Matrix DependentGrapth();
Vector Propensity(const Vector& x);
Vector PartialProp(int RIndex, const Vector& x, const Matrix& dg);
Matrix PropensityJacobian(const Vector& x);
void Equilibrium(Vector& x, Vector& a, int rxn);

int main(int argc, const char* argv[])
{
   try{
      //parse arguments
      const char* outFile;

      if (argc != 2) {
         std::cerr << "Usage:  dimerstats <# runs>"
		 		   <<"<output file>";
         exit(EXIT_FAILURE);
      }
      else {
	     outFile = argv[1];
      }
      
      time_t curTime = time(0);
      CSE::Math::Seeder(static_cast<unsigned int>(curTime), curTime);

      // Set up the problem:
      Vector X0 = Initialize();
      Matrix nu = Stoichiometry();
      Matrix dg = DependentGrapth();
      ReactionSet rxns(nu, dg, Propensity, PartialProp, PropensityJacobian, Equilibrium);

      // Configure solver
      SolverOptions opt;  

      opt.stepsize_selector_func = OSSADirect_Stepsize;
      opt.single_step_func =  OSSA_SingleStep;
      opt.progress_interval = 1;
	  opt.store_state_func =  &Exponential_StoreState; 
      opt.initial_stepsize = 0.001;
      opt.absolute_tol = 1e-6;
      opt.relative_tol = 1e-5;   

      double TimeFinal = 30.0;       

      //Make the Run and report results
	  SolutionHistory sln = StochRxn(X0, 0, TimeFinal, rxns, opt);
	  WriteHistoryFile(sln, outFile);
      //std::cerr << "Endpoints written to file:  "<< outFile << "\n";
   }

   catch (const std::exception& ex){
      std::cerr << "\nCaught " << ex.what() << '\n';
   }

   return 0;
}
