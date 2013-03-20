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
#include "Random.h"
#include <time.h>
#include <iostream>
#include <stdlib.h>

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Matrix DependentGrapth();
Vector Propensity(const Vector& x);
Vector PartialProp(int RIndex, const Vector& x, const Matrix& dg,  double& a0);
Matrix PropensityJacobian(const Vector& x);
void Equilibrium(Vector& x, Vector& a, int rxn);

int main(int argc, const char* argv[])
{
   try{
      //parse arguments
      int iterations;
      const char* outFile;

      if (argc != 3) {
         std::cerr << "Usage:  dimerstats <# runs> <output file>";
         exit(EXIT_FAILURE);
      }
      else {
         iterations = atoi(argv[1]);
         outFile = argv[2];
      }

      time_t curTime = time(0);
      CSE::Math::Seeder(static_cast<unsigned int>(curTime), curTime);

      // Set up the problem:
      Vector X0 = Initialize();
      Matrix nu = Stoichiometry();
      Matrix dg = DependentGrapth();
      ReactionSet rxns(nu, Propensity);

      // Configure solver
      SolverOptions opt = ConfigStochRxn(1,"ossa");
      opt.stepsize_selector_func = SSADirect_Stepsize; 
      opt.single_step_func = SSA_SingleStep; 
      opt.progress_interval = 1000000;
      opt.initial_stepsize = 0.001;
      opt.absolute_tol = 1e-6;
      opt.relative_tol = 1e-5;

      double TimeFinal =500.0;

      //Make the Run and report results
      EndPtStats endpts = CollectStats(iterations, X0, 0, TimeFinal, rxns, opt);
      WriteStatFile(endpts, outFile);
      std::cerr << "Done.\n";
   }

   catch (const std::exception& ex) {
      std::cerr << "\nCaught " << ex.what() << '\n';
   }

   return 0;
}
