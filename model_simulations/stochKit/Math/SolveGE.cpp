//*****************************************************************************|
//*  FILE:    SolveGE.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 26, 2003
//*
//*  LAST MODIFIED: Aug 10, 2004 
//*             BY: Yang Cao
//*             TO: Use LAPACK to solve linear equation
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
#include "SolveGE.h"
#include "Errors.h"
#include "IEEE.h"
#include <math.h>
#include <algorithm>
#include <iostream>

#ifdef USELAPACK
namespace CSE {
  namespace Math { 

    void SolveGE(const Matrix& A, Vector &x, const Vector& b)
    { 
	Matrix copyA = A; 
	SolveGEIP(copyA, x, b); 
    } 

    void SolveGEIP(const Matrix& A, Vector &x, const Vector& b)
    {
        const int size = A.Rows();

        if ((A.Cols() != size) || (b.Size() != size) || x.Size() != size) {
                throw NonconformableShapesError();
        }
	x = b; 
	static long N, NRHS = 1, LDA, LDX, info; 
	N = size; 
	long *ipiv = new long[N]; 
	LDA = N; LDX = N;  

	F77NAME(dgesv)(&N, &NRHS, A.addr(), &LDA, ipiv, x.addr(), &LDX, &info); 
	
	if (info != 0)
        	throw LinearSolverError();

    }
  }
}
#else
namespace {

  using namespace CSE::Math;

  void eliminate(Matrix& aug, const int size, const double singTolerance)
  {
    // Now, do GE (with partial pivoting) to get an upper triangular
    // augmented matrix.
    for (int i = 0; i < size; ++i) {

      // Step 1: Pivot largest row to top of remaining matrix
      int largestRow = i;
      double largestVal = fabs(aug(i,i));
      
      // 1a) Find largest element under current diagonal element:
      for (int j = i + 1; j < size; ++j) {
        double absVal = fabs(aug(j, i));
        if (absVal > largestVal) {
          largestRow = j;
          largestVal = absVal;
        }
      }
      
      // 1b) Pivot if necessary
      if (largestRow != i) {
        for (int j = i; j < size + 1; ++j) {
          std::swap(aug(i, j), aug(largestRow, j));
        }
      }

      if (AlmostZero(aug(i, i), singTolerance)) {
        throw SingularMatrixError();
      }
      
      // Step 2: Determine scale & eliminate subdiagonal elements:
      for (int j = i + 1; j < size; ++j) {
        double pivot = aug(j, i) / aug(i,i);
        
        for (int k = i; k < size + 1; ++k) {
          aug(j, k) -= pivot * aug(i, k);
        }
      }
    }
  }


  void back_substitute(Matrix& aug, const int size)
  {
    for (int i = size - 1; i >= 0; --i) {
      aug(i, size) /= aug(i, i);

      const double val = aug(i, size);

      for (int j = 0; j < i; ++j) {
        aug(j, size) -= val * aug(j, i);
      }
    }
  }

    
}

namespace CSE {
  namespace Math {

    void SolveGE(const Matrix& A, Vector &x, const Vector& b)
    { 
	SolveGEIP(A, x, b); 
    } 

    void SolveGEIP(const Matrix& A, Vector &x, const Vector& b)
    { 
      // Verify that the system is well-formed
      const int size = A.Rows();
      if ((A.Cols() != size) || (b.Size() != size)) {
        throw NonconformableShapesError();
      }

      // Prepare the augmented matrix
      Matrix aug(size, size+1, NaN());
      for (int i = 0; i < size; ++i) {
        aug.Col(i) = A.Col(i);
      }
      aug.Col(size) = b;
      
      // Go to town...
      eliminate(aug, size, fabs(1e-14));
      back_substitute(aug, size);

      x = aug.Col(size);
    }
  } // Close CSE::Math namespace
} // Close CSE namespace
#endif
