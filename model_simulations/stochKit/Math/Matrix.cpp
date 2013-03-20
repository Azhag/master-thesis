//*****************************************************************************|
//*  FILE:    Matrix.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 8, 2003
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
#include "Matrix.h"
#include "IEEE.h"
#include "Errors.h"
#include "Vector.h"
#include "VectorOps.h"
#include <algorithm>
#include <memory>
#include <iostream>
#include <math.h>


#ifdef DEBUG
  const bool debugging = true;
#else
  const bool debugging = false;
#endif


namespace CSE {
  namespace Math {


    /****************************************/
    /*        RowProxy Implementation       */ 
    //***************************************/

    RowProxy& RowProxy::operator=(double val)
    {
	  int tmpSize = Size();	//Hong Jul 21
      for (int i = 0; i < tmpSize; ++i) {
        mSourceMat(mRow, i) = val;
      }
      return *this;
    }


    RowProxy& RowProxy::operator=(const Vector& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(mRow, i) = rhs(i);
      }
      return *this;
    }


    RowProxy& RowProxy::operator=(const RowProxy& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(mRow, i) = rhs(i);
      }
      return *this;
    }


    RowProxy& RowProxy::operator=(const ColProxy& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(mRow, i) = rhs(i);
      }
      return *this;     
    }


    /****************************************/
    /*        ColProxy Implementation       */ 
    //***************************************/

    ColProxy& ColProxy::operator=(double val)
    {
	  int tmpSize  = Size();
      for (int i = 0; i < tmpSize; ++i) {
        mSourceMat(i, mCol) = val;
      }
      return *this;
    }


    ColProxy& ColProxy::operator=(const Vector& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(i, mCol) = rhs(i);
      }
      return *this;
    }


    ColProxy& ColProxy::operator=(const ColProxy& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(i, mCol) = rhs(i);
      }
      return *this;     
    }


    ColProxy& ColProxy::operator=(const RowProxy& rhs)
    {
      int size = Size();
      if (size != rhs.Size()) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < size; ++i) {
        mSourceMat(i, mCol) = rhs(i);
      }
      return *this;     
    }



    /****************************************/
    /*        Matrix Implementation         */ 
    //***************************************/
  
    Matrix::Matrix()
      : mRows(-1)
      , mCols(-1)
      , mData(0)
    {
    }


    Matrix::Matrix(int rows, int cols)
      : mRows(rows)
      , mCols(cols)
      , mData((rows > 0 && cols > 0)
              ? new double[rows * cols]
              : throw BoundsError())
    {
      if (debugging) {
        std::uninitialized_fill(mData, mData + rows*cols, NaN());
      }
    }


    Matrix::Matrix(int rows, int cols, double initVal)
      : mRows(rows)
      , mCols(cols)
      , mData((rows > 0 && cols > 0)
              ? new double[rows * cols]
              : throw BoundsError())
    {
      std::uninitialized_fill(mData, mData + rows*cols, initVal);
    }


    Matrix::Matrix(const Matrix& copy)
      : mRows(copy.mRows)
      , mCols(copy.mCols)
      , mData(new double[mRows * mCols])
    {
      std::copy(copy.mData, copy.mData + mRows*mCols, mData);
    }


    Matrix::~Matrix()
    {
      delete [] mData;
    }
  

    Matrix& Matrix::operator=(const Matrix& rhs)
    {
      Matrix temp(rhs);
      std::swap(*this, temp);
      return *this;
    }


    void Matrix::swap(Matrix& rhs)
    {
      std::swap(mRows, rhs.mRows);
      std::swap(mCols, rhs.mCols);
      std::swap(mData, rhs.mData);
    }
    
    
#ifdef DEBUG
    double Matrix::operator()(int row, int col) const
    {
      check_bounds(row, col);
      return mData[row * mCols + col];
    }
    
    
    double& Matrix::operator()(int row, int col)
    {
      check_bounds(row, col);
      return mData[row * mCols + col];
    }
    
    
    void Matrix::check_bounds(int row, int col) const
    {
      if (!mData || row < 0 || row >= mRows || col < 0 || col >= mCols) {
        throw BoundsError();
      }
    }
    
    
#endif
    
    Matrix& Matrix::operator+=(const Matrix& rhs)
    {
      if ((rhs.Rows() != mRows) || (rhs.Cols() != mCols)) {
        throw NonconformableShapesError();
      }
      
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] += rhs.mData[i];
      }

      return *this;
    }


    Matrix& Matrix::operator-=(const Matrix& rhs)
    {
      if ((rhs.Rows() != mRows) || (rhs.Cols() != mCols)) {
        throw NonconformableShapesError();
      }
      
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] -= rhs.mData[i];
      }

      return *this;
    }


    Matrix& Matrix::operator*=(double val)
    {
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] *= val;
      }
      return *this;
    }
    

    Matrix& Matrix::operator/=(double val)
    {
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] /= val;
      }
      return *this;
    }


    Matrix& Matrix::operator+=(double val)
    {
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] += val;
      }
      return *this;
    }


    Matrix& Matrix::operator-=(double val)
    {
      const int size = mRows * mCols;

      for (int i = 0; i < size; ++i) {
        mData[i] -= val;
      }
      return *this;
    }
    

    
    void Matrix::output(std::ostream& out) const
    {
      for (int row = 0; row < mRows; ++row) {
        for (int col = 0; col < mCols; ++col) {
          out << '\t' << operator()(row, col);
        }
        out << '\n';
      }
    }

    
    Matrix operator*(const Matrix& lhs, const Matrix& rhs)
    {
      if (lhs.Cols() != rhs.Rows()) {
        throw NonconformableShapesError();
      }

      const int rows = lhs.Rows();
      const int cols = rhs.Cols();

      Matrix rv(rows, cols);

      for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
          rv(row, col) = UncheckedInnerProduct(lhs.Row(row), rhs.Col(col));
        }
      }

      return rv;
    }

/*
    Matrix operator.(const Matrix& lhs, const Matrix& rhs)
    {
	if (lhs.Cols() != rhs.Cols || lhs.Rows() != rhs.Rows() ) {
	   throw NonconformableShapesError();
	} 
	const int rows = lhs.Rows();
	const int cols = lhs.Cols();
	Matrix rv(rows, cols); 
	for (int row = 0; row < rows; ++row) {
          for (int col = 0; col < cols; ++col) {
            rv(row, col) = lhs(row, col)*rhs.Col(row, col));
        }
      }

      return rv;
    }
*/
    
    Matrix operator+(const Matrix& lhs, const Matrix& rhs)
    {
      return Matrix(lhs) += rhs;
    }


    Matrix operator-(const Matrix& lhs, const Matrix& rhs)
    {
      return Matrix(lhs) -= rhs;
    }


    Matrix operator*(double lhs, const Matrix& rhs)
    {
      return Matrix(rhs) *= lhs;
    }


    Matrix operator*(const Matrix& lhs, double rhs)
    {
      return Matrix(lhs) *= rhs;
    }


    Matrix operator+(double lhs, const Matrix& rhs)
    {
      return Matrix(rhs) += lhs;
    }


    Matrix operator+(const Matrix& lhs, double rhs)
    {
      return Matrix(lhs) += rhs;
    }


    Matrix operator-(double lhs, const Matrix& rhs)
    {
      return Matrix(rhs.Rows(), rhs.Cols(), lhs) -= rhs;
    }


    Matrix operator-(const Matrix& lhs, double rhs)
    {
      return Matrix(lhs) -= rhs;
    }


    Matrix operator/(const Matrix& lhs, double rhs)
    {
      return Matrix(lhs) /= rhs;
    }    


    std::ostream& operator<<(std::ostream& out, const Matrix& mat)
    {
      mat.output(out);
      return out;
    }

    
  } // Close CSE::Math namespace
} // Close CSE namespace
