#ifndef CSE_MATH_MATRIXOPS_H
#define CSE_MATH_MATRIXOPS_H
//*****************************************************************************|
//*  FILE:    MatrixOps.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 27, 2003
//*
//*  LAST MODIFIED: Nov. 8, 2004 
//*             BY: Yang Cao
//*             TO: Add InnerProduct function
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
#include "Matrix.h"


namespace CSE {
  namespace Math {

    Matrix Identity(int size);

    Matrix Transpose(const Matrix& mat);

    Matrix InnerProduct(const Matrix &lm, const Matrix &rm); 

  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_MATRIXOPS_H
