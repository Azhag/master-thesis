//*****************************************************************************|
//*  FILE:    Errors.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 12, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004
//*             BY: Yang Cao
//*             TO: Add an exception for Linear Solver
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
#include "Errors.h"


namespace CSE {
  namespace Math {

    const char* BoundsError::what() const throw()
    {
      return "Bounds Error";
    }
    
    const char* LinearSolverError::what() const throw()
    { 
	return "LinearSolverError Reported by LAPACK"; 
    }
    
    const char* NonconformableShapesError::what() const throw()
    {
      return "Nonconformable Shapes";
    }


    const char* FormatError::what() const throw()
    {
      return "Format Error";
    }

    
    const char* SingularMatrixError::what() const throw()
    {
      return "Singular Matrix";
    }


  } // Close CSE::Math namespace
} // Close CSE namespace
