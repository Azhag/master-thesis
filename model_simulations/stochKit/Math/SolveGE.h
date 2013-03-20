#ifndef CSE_MATH_SOLVEGE_H
#define CSE_MATH_SOLVEGE_H
//*****************************************************************************|
//*  FILE:    SolveGE.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 26, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Allow users to choose LAPACK(fortran) as linear solver 
//*
//*  SUMMARY:
//*
//*
//*  NOTES: If you use LAPACK linear solver, the matrix A will be changed
//*
//*
//*  TO DO:
//*
//*
//*****************************************************************************|
//       1         2         3         4         5         6         7         8
//345678901234567890123456789012345678901234567890123456789012345678901234567890
#include "Matrix.h"
#include "Vector.h"

#ifdef USELAPACK  
// Linkage names between C, C++, and Fortran (platform dependent)
#if  defined(RIOS)
#define F77NAME(x) x
#else
#define F77NAME(x) x##_
#endif

extern "C" void F77NAME(dgesv)(long *n, long *k, double *A, long *lda, long *ipiv,
            double *X, long *ldx, long *info);

#endif

namespace CSE {
  namespace Math {

    void SolveGE(const Matrix& A, Vector &x, const Vector& b);
    void SolveGEIP(const Matrix& A, Vector &x, const Vector& b);

  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_SOLVEGE_H
