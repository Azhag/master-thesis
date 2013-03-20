//*****************************************************************************|
//*  FILE:    FixedStepsize.cpp
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
#include "FixedStepsize.h"

namespace CSE {
  namespace StochRxn {

    
    double Fixed_Stepsize(Vector& /* x */,
                          Vector& /* a */,
                          double & /* a0 */,
                          const Matrix& /* nu */,
                          double tau,
                          double /* eps */,
                          PropensityJacobianFunc j)
    {
      return tau;
    }
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
