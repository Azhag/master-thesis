//*****************************************************************************|
//*  FILE:    Vector.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 10, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Add Norm functions 
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
#include "VectorOps.h"
#include "Matrix.h"
#include "IEEE.h"
#include "Errors.h"
#include <sstream>
#include <iomanip>
#define fabs(a) ( ((a) > 0)? (a):-(a))


#ifdef DEBUG
  const bool debugging = true;
#else
  const bool debugging = false;
#endif

namespace CSE {
  namespace Math {

    Vector::Vector()
      : mSize(-1)
      , mData(0)
    {
    }

    Vector::Vector(int size)
      : mSize(size)
      , mData(new double[size])
    {
      if (debugging) {
        std::uninitialized_fill(mData, mData + mSize, NaN());
      }
    }


    Vector::Vector(int size, double initVal)
      : mSize(size)
      , mData(new double[size])
    {
      std::uninitialized_fill(mData, mData + mSize, initVal);
    }


    Vector::Vector(const RowProxy& matRow)
      : mSize(matRow.Size())
      , mData(new double[mSize])
    {
      for (int i = 0; i < mSize; ++i) {
        mData[i] = matRow(i);
      }
    }


    Vector::Vector(const ColProxy& matCol)
      : mSize(matCol.Size())
      , mData(new double[mSize])
    {
      for (int i = 0; i < mSize; ++i) {
        mData[i] = matCol(i);
      }
    }


    Vector::Vector(const Vector& copy)
      : mSize(copy.mSize)
      , mData(new double[mSize])
    {
      std::copy(copy.mData, copy.mData + mSize, mData);
    }


    Vector::~Vector()
    {
      delete [] mData;
    }
  

    Vector& Vector::operator=(const Vector& rhs)
    {
      if (this != &rhs) {
        if (mSize == rhs.Size()) {
          for (int i = 0; i < mSize; ++i) {
            mData[i] = rhs(i);
          }
        }
        else {
          Vector temp(rhs);
          std::swap(*this, temp);
        }
      }
      return *this;
    }


    Vector& Vector::operator=(const RowProxy& rhs)
    {
      if (mSize == rhs.Size()) {
        for (int i = 0; i < mSize; ++i) {
          mData[i] = rhs(i);
        }
      }
      else {
        Vector temp(rhs);
        std::swap(*this, temp);
      }
      return *this;
    }


    Vector& Vector::operator=(const ColProxy& rhs)
    {
      if (mSize == rhs.Size()) {
        for (int i = 0; i < mSize; ++i) {
          mData[i] = rhs(i);
        }
      }
      else {
        Vector temp(rhs);
        std::swap(*this, temp);
      }
      return *this;
    }

  
#ifdef DEBUG
    
    // These are bounds-checked if DEBUG is defined
    
    double Vector::operator()(int index) const
    {
      bounds_check(index);
      return mData[index];
    }


    double& Vector::operator()(int index)
    {
      bounds_check(index);
      return mData[index];
    }
    
#endif

    double Vector::Norm()
    { 
	int i; 
	double max = 0; 

	for (i = 0; i < mSize; i++) {
	  if (max < fabs(mData[i]))  max = fabs(mData[i]); 
	}
	return max; 
    }

    double Vector::Norm1()
    { 
	int i; 
	double norm = 0; 
	for (i = 0; i < mSize; i ++) {
	  norm += fabs(mData[i]); 
	} 
	return norm; 
    } 

    double Vector::Norm2()
    { 
        int i; 
        double norm = 0, tmp; 
        for (i = 0; i < mSize; i ++) {
	      tmp = fabs(mData[i]); 
          norm += tmp*tmp;
        } 
        return sqrt(norm); 
    }

    double Vector::Sum() // if all elements are positive, Sum is the Norm1
    {
       double sum = 0.0;
       int i;
       for (i = 0; i < mSize; ++i) {
          sum += mData[i];
       }
       return sum;
     }


    std::string Vector::AsString() const
    {
      std::ostringstream strm;
      strm.precision(10);
      
      strm << '[';
	  int tmpSize  = this->Size(); //Hong Jul 21
      for (int i = 0; i < tmpSize; ++i) {
        strm << "  " << mData[i];
      }

      strm << "  ]";
      return strm.str();
    }


    Vector& Vector::operator+=(const Vector& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] += rhs.mData[i];
      }
      return *this;
    }


    Vector& Vector::operator+=(const RowProxy& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] += rhs(i);
      }
      return *this;
    }


    Vector& Vector::operator+=(const ColProxy& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] += rhs(i);
      }
      return *this;
    }
    


    Vector& Vector::operator-=(const Vector& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] -= rhs.mData[i];
      }
      return *this;
    }


    Vector& Vector::operator-=(const RowProxy& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] -= rhs(i);
      }
      return *this;
    }


    Vector& Vector::operator-=(const ColProxy& rhs)
    {
      if (rhs.Size() != mSize) {
        throw NonconformableShapesError();
      }

      for (int i = 0; i < mSize; ++i) {
        mData[i] -= rhs(i);
      }
      return *this;
    }

    
    Vector& Vector::operator*=(double val)
    {
      for (int i = 0; i < mSize; ++i) {
        mData[i] *= val;
      }
      return *this;
    }


    Vector& Vector::operator/=(double val)
    {
      for (int i = 0; i < mSize; ++i) {
        mData[i] /= val;
      }
      return *this;      
    }
    

    Vector Vector::operator-() const
    {
      Vector rv(*this);
      for (int i = 0; i < mSize; ++i) {
        rv.mData[i] = -rv.mData[i];
      }
      return rv;
    }

    
    void Vector::swap(Vector& rhs)
    {
      std::swap(mSize, rhs.mSize);
      std::swap(mData, rhs.mData);
    }


    Vector operator*(const Vector& lhs, double rhs)
    {
      return Vector(lhs) *= rhs;
    }


    Vector operator*(double lhs, const Vector& rhs)
    {
      return Vector(rhs) *= lhs;
    }


    Vector operator+(const Vector& lhs, const Vector& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const Vector& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const Vector& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const RowProxy& lhs, const Vector& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const RowProxy& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const RowProxy& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const ColProxy& lhs, const Vector& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const ColProxy& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator+(const ColProxy& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) += rhs;
    }


    Vector operator-(const Vector& lhs, const Vector& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const Vector& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const Vector& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }

    Vector operator-(const RowProxy& lhs, const Vector& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const RowProxy& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const RowProxy& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const ColProxy& lhs, const Vector& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const ColProxy& lhs, const RowProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator-(const ColProxy& lhs, const ColProxy& rhs)
    {
      return Vector(lhs) -= rhs;
    }


    Vector operator*(const Matrix& lhs, const Vector& rhs)
    {
      if (lhs.Cols() != rhs.Size()) {
        throw NonconformableShapesError();
      }

      const int rows = lhs.Rows();
      Vector rv(rows);

      for (int i = 0; i < rows; ++i) {
        rv(i) = UncheckedInnerProduct(lhs.Row(i), rhs);
      }

      return rv;
    }
    
    
#ifdef DEBUG
    
    void Vector::bounds_check(int index) const
    {
      if (!mData || index < 0 || index >= mSize) {
        throw BoundsError();
      }
    }
    
#endif
    
  } // Close CSE::Math namespace  
} // Close CSE namespace


namespace std {

  std::ostream& operator<<(std::ostream& out, const CSE::Math::Vector& vec)
  {
    out << vec.AsString();
    return out;
  }

}
