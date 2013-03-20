//******************************************************************************
//*  FILE:  HeatShock.cpp
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

#include "ProblemDefinition.h"
#include "SolverOptions.h"
#include "CollectStats.h"
#include "SSA.h"
#include "Vector.h"
#include "Matrix.h"
#include "FixedStepsize.h"
#include "VariedStepsize.h"
#include "Random.h"
#include "math.h"
#include "IEEE.h"
#include "SolveGE.h"
#include "VectorOps.h"
#include "HeatShock.h"
#include <iostream>
#include <stdlib.h>

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Matrix DependentGrapth();
Vector Propensity(const Vector& x);
Vector PartialProp(int RIndex, const Vector& x, const Matrix& dg, double& a0);
Matrix PropensityJacobian(const Vector& x);
void Equilibrium(Vector& x, Vector& a, int rxn);

/*
int main(int argc, const char* argv[])
{
     //parse arguments
     int iterations;
     const char *outFile;

     if (argc != 3) {
           std::cerr << "Usage:  dimerstats <# runs>"
                     <<"<output file>";
           exit(EXIT_FAILURE);
      }
      else {
           iterations = atoi(argv[1]);
           outFile = argv[2];
      }
*/
int HeatShock(int iterations, char* outFile)
{
  try {
    // Set up the problem:
    Vector X0 = Initialize();
    Matrix nu = Stoichiometry();
    Matrix dg = DependentGrapth();
    ReactionSet rxns(nu, dg,Propensity, PartialProp, PropensityJacobian);

    // Configure solver
    SolverOptions opt = ConfigStochRxn(0);

      opt.stepsize_selector_func = SSADirect_Stepsize;
      opt.single_step_func =  SSA_SingleStep;
      opt.progress_interval = 100000000L;
      opt.initial_stepsize = 0.001;
      opt.absolute_tol = 1e-6;
      opt.relative_tol = 1e-5;
                                      
    double TimeFinal = 500.0;                                                              

    // Make the run
    EndPtStats stats = 
		CollectStats(iterations, X0, 0, TimeFinal, rxns, opt);
    WriteStatFile(stats, outFile);
    std::cerr << "Endpoints written to file <  "<< outFile << ">.\n";
  }
  catch (const std::exception& ex) {
    std::cerr << "\nCaught " << ex.what() << '\n';
  } 
  return 0;
}
