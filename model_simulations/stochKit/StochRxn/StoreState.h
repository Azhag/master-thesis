#ifndef CSE_STOCHRXN_STORESTATE_H
#define CSE_STOCHRXN_STORESTATE_H
//*****************************************************************************|
//*  FILE:    StoreState.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 25, 2003
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
#include "SolutionHistory.h"
#include "SolutionPt.h"


namespace CSE {
  namespace StochRxn {

    void Exponential_StoreState(SolutionHistory& hist, const SolutionPt& x);
    
    void NoHistory_StoreState(SolutionHistory& hist, const SolutionPt& x);
                            

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_STORESTATE_H
