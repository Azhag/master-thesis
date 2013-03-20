#ifndef CSE_STOCHRXN_PROPENSITY_H
#define CSE_STOCHRXN_PROPENSITY_H
//*****************************************************************************|
//*  FILE:    Propensity.h
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
#include "VectorFwd.h"
#include "MatrixFwd.h"


namespace CSE {
  namespace StochRxn {

    // Some useful typedefs
    typedef Vector (*PropensityFunc)(const Vector& x);
    typedef Vector (*PartialPropensityFunc)(int RIndex, const Vector& x, const Matrix& dg, Vector& a, double& a0);
    typedef Matrix (*PropensityJacobianFunc)(const Vector& x);
    typedef void (*EquilibriumFunc)(Vector& x, Vector& a, int rxn);

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_PROPENSITY_H
