//*****************************************************************************|
//*  FILE:    GillespiePetzoldStepsize.cpp
//*
//*  AUTHOR:  Yang Cao
//*
//*  CREATED: Nov. 8, 2004
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
#include "GillespiePetzoldStepsize.h"

namespace CSE {
  namespace StochRxn {

    
    double GillespiePetzold_Stepsize(Vector& x ,
                          Vector& a ,
                          double & a0 ,
                          const Matrix& nu ,
                          double tau,
                          double eps, 
                          PropensityJacobianFunc jacobian)
    {
	static Matrix f(x.Size(), x.Size()); 
	static Vector mu = a; 
	static Vector sigma = a; 
	static double r1, r2; 
	static double tau1, tau2; 

	f = jacobian(x)*nu; 
	mu = f*a; 
	sigma = InnerProduct(f, f)*a; 
	r1 = mu.Norm(); //Max norm
	r2 = sigma.Norm(); // Max norm
	tau1 = eps * a0; 
	tau2 = tau1 * tau1; 
	if (r1 > 0) 
	  tau1 = tau1/r1; 
	else 
	  tau1 = 100.; 
	if (r2 > 0) 
	  tau2 = tau2/r2; 
	else 
	  tau2 = 100.; 
 
	if (tau1 > tau2) 
	  return tau2; 
	else 
	  return tau1; 
    }

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
