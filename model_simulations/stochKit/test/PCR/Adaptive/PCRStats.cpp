//******************************************************************************
//*  FILE:   DimerStats.cpp
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
#include "Vector.h"
#include "Matrix.h"
#include "Random.h"
#include "Solver.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>


using namespace CSE::Math;
using namespace CSE::StochRxn;

Vector Initialize();
Matrix Stoichiometry();
Vector Propensity(const Vector& x);

int main(int argc, const char* argv[])
{
	clock_t start, end;
	start = clock();	
	

    try{
      //parse arguments
      int iterations;
      const char* outFile;

      if (argc != 3) {
         std::cerr << "Usage:  dimerstats <# runs>"
		 		   <<"<output file>";
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
      ReactionSet rxns(nu, Propensity);

      // Configure solver
      SolverOptions opt = ConfigStochRxn(0);  

      double TimeFinal = 30;       

      //Make the Run and report results
      EndPtStats endpts = 
      CollectStats(iterations, X0, 0, TimeFinal, rxns, opt);
      WriteStatFile(endpts, outFile);
      //std::cerr << "Endpoints written to file:  "<< outFile << "\n";
   }

   catch (const std::exception& ex){
      std::cerr << "\nCaught " << ex.what() << '\n';
   }
	
	end = clock();
	double cpu_time_used;
	cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
	std::cerr  << "total time =" << cpu_time_used << '\n';

   return 0;
}
