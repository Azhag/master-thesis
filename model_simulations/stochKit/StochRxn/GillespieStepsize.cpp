//*****************************************************************************|
//*  FILE:    GillespieStepsize.cpp
//*
//*  AUTHOR:  Yang Cao
//*
//*  CREATED: Nov. 4, 2004
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
#include "GillespieStepsize.h"

namespace CSE {
  namespace StochRxn {

    
    double Gillespie_Stepsize(Vector& x ,
                          Vector& a ,
                          double & a0 ,
                          const Matrix& nu ,
                          double tau,
                          double eps, 
                          PropensityJacobianFunc jacobian)
    {
	static Vector ksi = nu*a; 
	static double r; 
	static Vector tmpr = a; 

	ksi = nu*a; 
	tmpr = jacobian(x) * ksi ; 
	r = tmpr.Norm1(); 
	// std::cout << a0 << ' ' << eps*a0/r << ' ' << Inf();   
	if (r > 0) return (eps*a0/r); 
	else return 1.; // temperary setup 
    }

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
