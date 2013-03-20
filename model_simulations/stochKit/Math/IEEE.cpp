//*****************************************************************************|
//*  FILE:    IEEE.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 10, 2003
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
#include "IEEE.h"
#include <math.h>


namespace CSE {
  namespace Math {

    const double Zero = 0.0;


    bool AlmostZero(double val, double tolerance)
    {
      return fabs(val) <= tolerance;
    }


    bool AlmostEqual(double a, double b, double tolerance)
    {
      return fabs(a - b) <= tolerance;
    }

    
    double Round(double val)
    {
      int ival = static_cast<int>(val); // truncates
      double rem = val - ival;

      return (rem >= 0.5) ? ival + 1 : ((rem <= -0.5) ? ival - 1 : ival);
    }


  } // Close CSE::Math namespace
} // Close CSE namespace
