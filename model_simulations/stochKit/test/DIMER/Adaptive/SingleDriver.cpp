//******************************************************************************
//*  FILE:   SingleDriver.cpp
//*
//*  AUTHOR: HongLi
//*
//*  CREATED: Mar 06, 2005
//*
//*  LAST MODIFIED: Apr 3, 2005
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
#include "Vector.h"
#include "Matrix.h"
#include "Random.h"
#include "StoreState.h"
#include "Solver.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

#define FixStepsize 0;
#define AdaptiveStepsize 1;

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Vector Propensity(const Vector& x);

int main(int argc, const char* argv[])
{
   try{
      //parse arguments
      const char* outFile;

      if (argc != 2) {
         std::cerr << "Usage:  dimersingle <output file>";
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
      ReactionSet rxns(nu, Propensity);

      // Configure solver
      SolverOptions opt;// = ConfigStochRxn(0);  

      opt.stepsize_selector_func = Cao_Stepsize;
      opt.single_step_func =  ExplicitTau_SingleStep;
      opt.progress_interval = 1;
      opt.store_state_func =  &Exponential_StoreState; 
      opt.initial_stepsize = 0.001;
      opt.absolute_tol = 1e-6;
      opt.epsilon = 0.01; 
      opt.relative_tol = 1e-5;   
      opt.StepControl = 1;   

      double TimeFinal = 10.0;       

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
