//*****************************************************************************|
//*  FILE:    MatrixOps.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 27, 2003
//*
//*  LAST MODIFIED:  Nov. 8, 2004
//*             BY:  Yang Cao
//*             TO:  Add InnerProduct function
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
#include "MatrixOps.h"


namespace CSE {
  namespace Math {


    Matrix Identity(int size)
    {
      Matrix rv(size, size, 0.0);
      for (int i = 0; i < size; ++i) {
        rv(i, i) = 1.0;
      }
      return rv;
    }


    Matrix Transpose(const Matrix& mat)
    {
      const int origCols = mat.Cols();
      const int origRows = mat.Rows();

      Matrix rv(origCols, origRows);

      for (int col = 0; col < origCols; ++col) {
        for (int row = 0; row < origRows; ++row) {
          rv(col, row) = mat(row, col);
        }
      }
      return rv;
    }

    Matrix InnerProduct(const Matrix &lm, const Matrix &rm)
    { 
	int Rowsize = lm.Rows(); 
	int Colsize = lm.Cols(); 
	Matrix rv(Rowsize, Colsize); 
	for (int col = 0; col < Colsize; col++)
	  for (int row = 0; row < Rowsize; row++) 
		rv(row, col) = lm(row, col)*rm(row, col); 

	return rv; 
    } 

  } // Close CSE::Math namespace
} // Close CSE namespace
