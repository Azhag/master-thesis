#ifndef CSE_STOCHRXN_SOLUTIONHISTORY_H
#define CSE_STOCHRXN_SOLUTIONHISTORY_H
//*****************************************************************************|
//*  FILE:    SolutionHistory.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
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
#include <vector>
#include "SolutionPt.h"


namespace CSE {
  namespace StochRxn {

    typedef std::vector<SolutionPt> SolutionHistory;


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_SOLUTIONHISTORY_H
