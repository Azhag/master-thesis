#ifndef CSE_MATH_VECTOROPS_H
#define CSE_MATH_VECTOROPS_H
//*****************************************************************************|
//*  FILE:    VectorOps.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Remove Sum() function (implemented in Vector.h already)
//*  LAST MODIFIED: Nov. 4, 2004
//* 		BY: Yang Cao
//* 		TO: Implement the GillespieStepsizeSelection
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
#include "Vector.h"
#include "Matrix.h"

namespace CSE {
  namespace Math {

    double Norm(const Vector& v, double pNorm = 2.0);

    Vector Round(const Vector& v);
    double Min(const Vector& v); 
    double Max(const Vector& v); 

    double InnerProduct(const Vector& lhs, const Vector& rhs);
    double InnerProduct(const Vector& lhs, const RowProxy& rhs);
    double InnerProduct(const Vector& lhs, const ColProxy& rhs);

    double InnerProduct(const RowProxy& lhs, const RowProxy& rhs);
    double InnerProduct(const RowProxy& lhs, const Vector& rhs);
    double InnerProduct(const RowProxy& lhs, const ColProxy& rhs);

    double InnerProduct(const ColProxy& lhs, const ColProxy& rhs);
    double InnerProduct(const ColProxy& lhs, const Vector& rhs);
    double InnerProduct(const ColProxy& lhs, const RowProxy& rhs);


    double UncheckedInnerProduct(const Vector& lhs, const Vector& rhs);
    double UncheckedInnerProduct(const Vector& lhs, const RowProxy& rhs);
    double UncheckedInnerProduct(const Vector& lhs, const ColProxy& rhs);

    double UncheckedInnerProduct(const RowProxy& lhs, const RowProxy& rhs);
    double UncheckedInnerProduct(const RowProxy& lhs, const Vector& rhs);
    double UncheckedInnerProduct(const RowProxy& lhs, const ColProxy& rhs);

    double UncheckedInnerProduct(const ColProxy& lhs, const ColProxy& rhs);
    double UncheckedInnerProduct(const ColProxy& lhs, const Vector& rhs);
    double UncheckedInnerProduct(const ColProxy& lhs, const RowProxy& rhs);

  
  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_VECTOROPS_H
