#ifndef CSE_STOCHRXN_COLLECTSTATS_H
#define CSE_STOCHRXN_COLLECTSTATS_H
//*****************************************************************************|
//*  FILE:    CollectStats.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: February 19, 2003
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
#include "StochRxn.h"
#include <vector>
#include <string>
#include <utility>

namespace CSE {
  namespace StochRxn {

    typedef std::vector<std::pair<double, Vector> > EndPtStats;

    EndPtStats CollectStats(unsigned int runs,
                            const Vector& x0,
                            double t0,
                            double tf,
                            const ReactionSet& reactions,
                            const SolverOptions& options);

    void WriteStatFile(const EndPtStats& stats, const std::string& fileName);


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_COLLECTSTATS_H
