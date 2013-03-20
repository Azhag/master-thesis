//*****************************************************************************|
//*  FILE:    StoreState.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 25, 2003
//*
//*  LAST MODIFIED: June 1, 2004 
//*             BY: HONG 
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
#include "StoreState.h"


namespace CSE {
  namespace StochRxn {

    void Exponential_StoreState(SolutionHistory& hist, const SolutionPt& x)
    {
      hist.push_back(x);
    }
    

    void NoHistory_StoreState(SolutionHistory& hist, const SolutionPt& x)
    {
      if (hist.empty()) {
        hist.push_back(x);
      }
      else {
        hist[0] = x;
      }
    }
                            

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

