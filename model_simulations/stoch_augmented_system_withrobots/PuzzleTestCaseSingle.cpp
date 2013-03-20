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
		const char* outFile;

		if (argc != 2) {
			std::cerr << "Usage:  puzzletestcasesingle <output file>\n";
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
		SolverOptions opt = ConfigStochRxn(1);  

		opt.stepsize_selector_func = SSADirect_Stepsize;
		opt.single_step_func =  SSA_SingleStep;
		opt.progress_interval = 1;
		opt.store_state_func =  &Exponential_StoreState; 
		opt.initial_stepsize = 0.001;
		opt.absolute_tol = 1e-6;
		opt.relative_tol = 1e-5;   
		opt.StepControl = FixedStep;   

		double TimeFinal = 20000.0;

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
