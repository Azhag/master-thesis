#ifndef CSE_MATH_VECTOR_H
#define CSE_MATH_VECTOR_H
//*****************************************************************************|
//*  FILE:    Vector.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 8, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao	
//*             TO: Add norm functions and addr()
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
#include <algorithm>
#include <iosfwd>
#include <string>
#include <math.h>


namespace CSE {
  namespace Math {

    class RowProxy;
    class ColProxy;
    class Matrix;


    class Vector 
    {
      public:
        Vector();
        explicit Vector(int size);
        Vector(int size, double initVal);
        explicit Vector(const RowProxy& matRow);
        explicit Vector(const ColProxy& matCol);
	Vector(double *addr, int size):mSize(size), mData(addr){}; 

        Vector(const Vector& copy);
        ~Vector();
       
        Vector& operator=(const Vector& rhs);
        Vector& operator=(const RowProxy& rhs);
        Vector& operator=(const ColProxy& rhs);

        
#ifdef DEBUG
        // These are bounds-checked if DEBUG is defined
        double operator()(int index) const;
        double& operator()(int index);
#else
        double operator()(int index) const { return  mData[index]; }
        double& operator()(int index) { return mData[index]; }
#endif
        
        int Size() const { return mSize; }
	double Norm(); // Max norm
	double Norm1(); // 1-norm
	double Norm2(); // 2-norm
	double Sum();
	double * addr() const { return mData; }
	double * addr() { return mData; }
		
        std::string AsString() const;
        
        Vector& operator+=(const Vector& rhs);
        Vector& operator+=(const RowProxy& rhs);
        Vector& operator+=(const ColProxy& rhs);
        
        Vector& operator-=(const Vector& rhs);
        Vector& operator-=(const RowProxy& rhs);
        Vector& operator-=(const ColProxy& rhs);

        Vector& operator*=(double val);
        Vector& operator/=(double val);

        Vector operator-() const;
        
        void swap(Vector& rhs);
        
#ifdef DEBUG
      private:
        void bounds_check(int index) const;
#endif

      private:
        int mSize;
        double* mData;
    };


    Vector operator+(const Vector& lhs, const Vector& rhs);
    Vector operator+(const Vector& lhs, const RowProxy& rhs);
    Vector operator+(const Vector& lhs, const ColProxy& rhs);

    Vector operator+(const RowProxy& lhs, const Vector& rhs);
    Vector operator+(const RowProxy& lhs, const RowProxy& rhs);
    Vector operator+(const RowProxy& lhs, const ColProxy& rhs);

    Vector operator+(const ColProxy& lhs, const Vector& rhs);
    Vector operator+(const ColProxy& lhs, const RowProxy& rhs);
    Vector operator+(const ColProxy& lhs, const ColProxy& rhs);


    Vector operator-(const Vector& lhs, const Vector& rhs);
    Vector operator-(const Vector& lhs, const RowProxy& rhs);
    Vector operator-(const Vector& lhs, const ColProxy& rhs);

    Vector operator-(const RowProxy& lhs, const Vector& rhs);
    Vector operator-(const RowProxy& lhs, const RowProxy& rhs);
    Vector operator-(const RowProxy& lhs, const ColProxy& rhs);

    Vector operator-(const ColProxy& lhs, const Vector& rhs);
    Vector operator-(const ColProxy& lhs, const RowProxy& rhs);
    Vector operator-(const ColProxy& lhs, const ColProxy& rhs);

    
    Vector operator*(const Vector& lhs, double rhs);
    Vector operator*(double lhs, const Vector& rhs);

    Vector operator*(const Matrix& lhs, const Vector& rhs);


    

  } // Close CSE::Math namespace
} // Close CSE namespace


namespace std {
  
  template <>
  inline void swap(CSE::Math::Vector& lhs, CSE::Math::Vector& rhs)
  {
    lhs.swap(rhs);
  }

  std::ostream& operator<<(std::ostream& out, const CSE::Math::Vector& vec);
  

}

#endif // CSE_MATH_VECTOR_H
