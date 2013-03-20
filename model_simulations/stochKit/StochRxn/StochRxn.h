#ifndef CSE_STOCHRXN_H
#define CSE_STOCHRXN_H
//*****************************************************************************|
//*  FILE:    StochRxn.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: November 14, 2002
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
#include "VectorFwd.h"
#include "ReactionSet.h"
#include "SolverOptions.h"
#include "SolutionHistory.h"

namespace CSE {
  namespace StochRxn {


    SolutionHistory StochRxn(const Vector& x0,
                             double t0,
                             double tf,
                             const ReactionSet& reactions,
                             const SolverOptions& options);

    void WriteHistoryFile(const SolutionHistory& hist,
	                          const std::string& fileName);


  } // Close StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_H
