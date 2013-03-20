#ifndef CSE_MATH_IEEE_H
#define CSE_MATH_IEEE_H
//*****************************************************************************|
//*  FILE:    IEEE.h
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

namespace CSE {
  namespace Math {

    extern const double Zero;

    inline double NaN() { return Zero/Zero;    }
    inline double Inf() { return 1.0/Zero;     }
    inline double MinusInf() { return -1.0/Zero; }
    
    bool AlmostZero(double val, double tolerance);
    bool AlmostEqual(double a, double b, double tolerance);

    double Round(double val);

    inline int Sign(double x) 
    {
      return (x > 0) ? +1 : ((x < 0) ? -1 : 0);
    }


  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_IEEE_H
