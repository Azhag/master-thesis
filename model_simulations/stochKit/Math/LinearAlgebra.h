#ifndef CSE_MATH_LINEARALGEBRA_H
#define CSE_MATH_LINEARALGEBRA_H
//*****************************************************************************|
//*  FILE:    LinearAlgebra.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 25, 2003
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

namespace CSE {
  namespace Math {


    Vector operator+(const Vector& lhs, const Vector& rhs);
    Vector operator+(const RowProxy& lhs, const RowProxy& rhs);
    Vector operator+(const ColProxy& lhs, const ColProxy& rhs);
    Vector operator+(const Vector& lhs, const RowProxy& rhs);
    Vector operator+(const RowProxy& lhs, const Vector& rhs);
    Vector operator+(const Vector& lhs, const ColProxy& rhs);
    Vector operator+(const ColProxy& lhs, const Vector& rhs);
    Vector operator+(const RowProxy& lhs, const ColProxy& rhs);
    Vector operator+(const ColProxy& lhs, const RowProxy& rhs);

    Vector operator-(const Vector& lhs, const Vector& rhs);
    Vector operator-(const RowProxy& lhs, const RowProxy& rhs);
    Vector operator-(const ColProxy& lhs, const ColProxy& rhs);
    Vector operator-(const Vector& lhs, const RowProxy& rhs);
    Vector operator-(const RowProxy& lhs, const Vector& rhs);
    Vector operator-(const Vector& lhs, const ColProxy& rhs);
    Vector operator-(const ColProxy& lhs, const Vector& rhs);
    Vector operator-(const RowProxy& lhs, const ColProxy& rhs);
    Vector operator-(const ColProxy& lhs, const RowProxy& rhs);
    
    double InnerProduct(const Vector& lhs, const Vector& rhs);
    double InnerProduct(const RowProxy& lhs, const RowProxy& rhs);
    double InnerProduct(const ColProxy& lhs, const ColProxy& rhs);
    double InnerProduct(const Vector& lhs, const RowProxy& rhs);
    double InnerProduct(const RowProxy& lhs, const Vector& rhs);
    double InnerProduct(const Vector& lhs, const ColProxy& rhs);
    double InnerProduct(const ColProxy& lhs, const Vector& rhs);
    double InnerProduct(const RowProxy& lhs, const ColProxy& rhs);
    double InnerProduct(const ColProxy& lhs, const RowProxy& rhs);


  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_LINEARALGEBRA_H
