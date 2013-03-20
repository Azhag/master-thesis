//******************************************************************************
//*  FILE:   PuzzleTestCaseSingle.cpp
//*  AUTHOR: Loic Matthey
//*  CREATED: June 16 2008
//*
//****************************************************************************|

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

#define FixedStep 0;

using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Vector Propensity(const Vector& x);

int main(int argc, const char* argv[])
{
	try{
	//parse arguments
		int iterations = 0;
		const char* prefixOutFile;
		char outFile[50];
		
		if (argc != 3) {
			std::cerr << "Usage:  PuzzleTestCaseSingleMultiple <# runs>"
				<<" <output files>" << std::endl;
			exit(EXIT_FAILURE);
		} else {
			iterations = atoi(argv[1]);
			prefixOutFile = argv[2];
		}

		time_t curTime = time(0);
		CSE::Math::Seeder(static_cast<unsigned int>(curTime), curTime);

		for(int i = 0; i < iterations; ++i) {
			// Current output file
			sprintf(outFile, "%s_%d.txt",prefixOutFile, i);
			
			// Set up the problem:
			Vector X0 = Initialize();
			Matrix nu = Stoichiometry();
			ReactionSet rxns(nu, Propensity);

			// Configure solver
			SolverOptions opt = ConfigStochRxn(1);  

			opt.stepsize_selector_func = SSADirect_Stepsize;
			opt.single_step_func =  SSA_SingleStep;
			opt.progress_interval = 1;
			opt.store_state_func =  &Exponential_StoreState; 
			opt.initial_stepsize = 0.001;
			opt.absolute_tol = 1e-6;
			opt.relative_tol = 1e-5;   
			opt.StepControl = FixedStep;   

			double TimeFinal = 1200.0;

			//Make the Run and report results
			SolutionHistory sln = StochRxn(X0, 0, TimeFinal, rxns, opt);

			WriteHistoryFile(sln, outFile);
			//std::cerr << "Endpoints written to file:  "<< outFile << "\n";
		}
	}

	catch (const std::exception& ex){
		std::cerr << "\nCaught " << ex.what() << '\n';
	}

	return 0;
}
