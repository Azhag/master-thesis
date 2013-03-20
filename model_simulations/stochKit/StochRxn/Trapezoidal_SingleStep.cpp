//*****************************************************************************|
//*  FILE:    Trapezoidal_SingleStep.cpp
//*
//*  AUTHOR: Yang Cao 
//*
//*  CREATED: September, 2003
//*
//*  MODIFIED: June 4, 2004 
//*        BY: Hong  
//*        TO:  
//*
//*  MODIFIED:  
//*        BY:  
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
#include "Trapezoidal_SingleStep.h"
#include "VectorOps.h"
#include "MatrixOps.h"
#include "Random.h"


namespace CSE {
  namespace StochRxn {


    void Trapezoidal_SingleStep(Vector& x,
                                double t,
                                double& tau,
                                Vector& a,
                                double a0,
                                const Matrix& nu,
                                PropensityFunc propFunc,
                                PropensityJacobianFunc propJacFunc,
                                double abs_tol,
                                double rel_tol,
                                int& RXN,
                                Vector& p)
    {
      static Vector x1 = x; 
      static Vector a1 = a; 
      
      p = PoissonRandom(a * tau);
      x1 = x + nu*p; 
      a1 = propFunc(x1); 
      p = Round( 0.5*tau*(a1 - a) + p) ; 
    }
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
