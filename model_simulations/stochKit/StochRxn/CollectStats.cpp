//*****************************************************************************|
//*  FILE:    CollectStats.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: February 19, 2003
//*
//*  MODIFIED: June 04, 2004 
//*        BY: Hong
//*        TO:  
//*
//*  MODIFIED:  
//*        BY: 
//*        TO:  
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
#include "CollectStats.h"
#include "StoreState.h"
#include <stdio.h>
#include <iostream>


namespace CSE {
	namespace StochRxn {


		EndPtStats CollectStats(unsigned int runs,
			const Vector& x0,
			double t0,
			double tf,
			const ReactionSet& reactions,
			const SolverOptions& options)
		{
			// We'll use basically the options the user passed in,
			// but we want to make sure we only store the end state, not the
			// whole solution history.
			SolverOptions solvOpts(options);
			solvOpts.store_state_func = NoHistory_StoreState;

			EndPtStats stats(runs, std::pair<double, Vector>(0.0, x0) ); // Pre-allocate enough space
								// for all the runs we want to make

			SolutionHistory runResult;
			for (unsigned int i = 0; i < runs; ++i) {
				runResult = StochRxn(x0, t0, tf, reactions, solvOpts);
				stats[i].first = runResult.back().Time();
				stats[i].second = runResult.back().State();
			}

			return stats;
		}


		void WriteStatFile(const EndPtStats& stats, const std::string& fileName)
		{
			FILE* outFile = fopen(fileName.c_str(), "w");

			const unsigned int vecLength = stats.front().second.Size();

			for (unsigned int i = 0; i < stats.size(); ++i) {
				const Vector& state = stats[i].second;
				// fprintf(outFile, "%d", (int)state(0));
				fprintf(outFile, "%f", stats[i].first);
				for (unsigned int j = 0; j < vecLength; ++j) {
					//    fprintf(outFile, "\t%d", (int)state(j));
					fprintf(outFile, " %f", state(j));
				}
				fprintf(outFile, "\n");
			}
			fclose(outFile); 
		}


	} // Close CSE::StochRxn namespace
} // Close CSE namespace
