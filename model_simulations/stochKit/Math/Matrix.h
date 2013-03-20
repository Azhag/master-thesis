#ifndef CSE_MATH_MATRIX_H
#define CSE_MATH_MATRIX_H
//*****************************************************************************|
//*  FILE:    Matrix.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 8, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Change the matrix storage order (from row to column) 
//* 		    Add the double *addr() function
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
#include <iosfwd>
#include <exception>
#include <algorithm>


namespace CSE {
  namespace Math {

    class Matrix;
    class RowProxy;
    class ColProxy;
    class Vector;


    class RowProxy
    {
      public:
        RowProxy(Matrix& mat, int row);

        double& operator()(int col);
        double operator()(int col) const;

        int Size() const;
        
        RowProxy& operator=(double val);
        RowProxy& operator=(const Vector& rhs);
        RowProxy& operator=(const RowProxy& rhs);
        RowProxy& operator=(const ColProxy& rhs);

      private:
        Matrix& mSourceMat;
        int mRow;
    };

    
    class ColProxy
    {
      public:
        ColProxy(Matrix& mat, int col);

        double& operator()(int row);
        double operator()(int row) const;
 
       int Size() const;

        ColProxy& operator=(double val);
        ColProxy& operator=(const Vector& rhs);
        ColProxy& operator=(const ColProxy& rhs);
        ColProxy& operator=(const RowProxy& rhs);

      private:
        Matrix& mSourceMat;
        int mCol;
    };

  
    class Matrix
    {
      public:
        Matrix();
        Matrix(int rows, int cols);
        Matrix(int rows, int cols, double initVal);
        Matrix(const Matrix& copy);
        ~Matrix();
        
        Matrix& operator=(const Matrix& rhs);
        
#ifdef DEBUG
        // In debug mode, these are bounds-checked
        double operator()(int row, int col) const;
        double& operator()(int row, int col);
#else
        double operator()(int row, int col) const
        {
          // return mData[row * mCols + col];
	  return mData[col * mRows + row]; // modified by Yang Cao to change the storage order of matrix
        }

        double& operator()(int row, int col)
        {
          // return mData[row * mCols + col];
	  return mData[col * mRows + row]; // modified by Yang Cao to change the storage order of matrix
        }
#endif

        int Rows() const {  return mRows;  }
        int Cols() const {  return mCols;  }
	double * addr() const { return mData; }
	double * addr() { return mData; }
        
        const RowProxy Row(int row) const
        {
          return RowProxy(const_cast<Matrix&>(*this), row);
        }

        RowProxy Row(int row)
        {
          return RowProxy(*this, row);
        }

        const ColProxy Col(int col) const
        {
          return ColProxy(const_cast<Matrix&>(*this), col);
        }

        ColProxy Col(int col)
        {
          return ColProxy(*this, col);
        }

        Matrix& operator+=(const Matrix& rhs);
        Matrix& operator-=(const Matrix& rhs);
        
        Matrix& operator*=(double val);
        Matrix& operator/=(double val);
        Matrix& operator+=(double val);
        Matrix& operator-=(double val);


        void swap(Matrix& rhs);
        void output(std::ostream& out) const;
        
#ifdef DEBUG
      private:
        void check_bounds(int row, int col) const;
#endif
        
      private:
        int mRows;
        int mCols;
        double* mData;
    };


    Matrix operator*(const Matrix& lhs, const Matrix& rhs);
    Matrix operator+(const Matrix& lhs, const Matrix& rhs);
    Matrix operator-(const Matrix& lhs, const Matrix& rhs);

    Matrix operator*(double lhs, const Matrix& rhs);
    Matrix operator*(const Matrix& lhs, double rhs);

    Matrix operator+(double lhs, const Matrix& rhs);
    Matrix operator+(const Matrix& lhs, double rhs);

    Matrix operator-(double lhs, const Matrix& rhs);
    Matrix operator-(const Matrix& lhs, double rhs);

    Matrix operator/(const Matrix& lhs, double rhs);




    std::ostream& operator<<(std::ostream& out, const Matrix& mat);



    //*************************************************************
    //          RowProxy/ColProxy implementations
    //*************************************************************
    
    inline RowProxy::RowProxy(Matrix& mat, int row)
      : mSourceMat(mat), mRow(row)
    {
    }


    inline double& RowProxy::operator()(int col)
    {
      return mSourceMat(mRow, col);
    }


    inline double RowProxy::operator()(int col) const
    {
      return mSourceMat(mRow, col);
    }


    inline int RowProxy::Size() const
    {
      return mSourceMat.Cols();
    }

    
    inline ColProxy::ColProxy(Matrix& mat, int col)
      : mSourceMat(mat), mCol(col)
    {
    }
    

    inline double& ColProxy::operator()(int row)
    {
      return mSourceMat(row, mCol);
    }


    inline double ColProxy::operator()(int row) const
    {
      return mSourceMat(row, mCol);
    }


    inline int ColProxy::Size() const
    {
      return mSourceMat.Rows();
    }
    

  } // Close CSE::Math namespace
} // Close CSE namespace


namespace std {

  template <> 
  inline void swap(CSE::Math::Matrix& lhs, CSE::Math::Matrix& rhs)
  {
    lhs.swap(rhs);
  }

}


#endif // CSE_MATH_MATRIX_H
