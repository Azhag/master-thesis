#ifndef CSE_MATH_ERRORS_H
#define CSE_MATH_ERRORS_H
//*****************************************************************************|
//*  FILE:    Errors.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 12, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Add an exception for Linear Solver
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
#include <exception>


namespace CSE {
  namespace Math {

    class MathError : public std::exception {};

    class LinearSolverError : public MathError
    { 
	public: 
	       virtual const char* what() const throw();
    }; 

    class BoundsError : public MathError
    {
      public:
        virtual const char* what() const throw();
    };


    class NonconformableShapesError : public MathError
    {
      public:
        virtual const char* what() const throw();
    };
 

    class FormatError : public MathError
    {
      public:
        virtual const char* what() const throw();
    };


    class SingularMatrixError : public MathError
    {
      public:
        virtual const char* what() const throw();
    };


  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_ERRORS_H
