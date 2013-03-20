//******************************************************************************
//*  FILE:  SchloglStats.cpp
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
#include <stdlib.h>
#include <iostream>
#include "SchloglStats.h"

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Vector Propensity(const Vector& x);


int SchloglStats(int iterations, char* outFile)
{
   try{
      // Set up the problem:
      Vector X0 = Initialize();
      Matrix nu = Stoichiometry();
      ReactionSet rxns(nu, Propensity);

      // Configure solver
      SolverOptions opt = ConfigStochRxn(0);  

      opt.stepsize_selector_func = SSADirect_Stepsize;
      opt.single_step_func =  SSA_SingleStep;
      opt.progress_interval = 100000000L;
      opt.epsilon = 0.05;
      opt.StepControl = 0;
      opt.initial_stepsize = 0.2;
      opt.absolute_tol = 1e-6;
      opt.relative_tol = 1e-5;   

      double TimeFinal = 4.0;       

      //Make the Run and report results
      EndPtStats endpts = 
      CollectStats(iterations, X0, 0, TimeFinal, rxns, opt);
      WriteStatFile(endpts, outFile);
      //std::cerr << "Endpoints written to file:  "<< outFile << "\n";
   }

   catch (const std::exception& ex){
      std::cerr << "\nCaught " << ex.what() << '\n';
   }

   return 0;
}

