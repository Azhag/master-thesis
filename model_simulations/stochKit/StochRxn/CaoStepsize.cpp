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

    
    double Cao_Stepsize(Vector& x ,
                          Vector& a ,
                          double & a0 ,
                          const Matrix& nu ,
                          double tau,
                          double eps, 
                          PropensityJacobianFunc jacobian)
    {
	static Vector mu = x; 
	static Vector sigma = x; 
	static Vector changelimit = x, tau3 = x, tau4 = x; 
	static double r1, r2; 
	static double tau1, tau2; 
	static int N = x.Size(); 
	int i; 

	mu = nu*a; 
	sigma = InnerProduct(nu, nu)*a; 
	r1 = mu.Norm(); //Max norm
	r2 = sigma.Norm(); // Max norm

	for (i = 0; i < N; i ++)  {
                changelimit(i) = eps*fabs(x(i));
                if ( changelimit(i) < 1) changelimit(i) = 1;
                tau3(i) = changelimit(i)/fabs(mu(i));
                tau4(i) = changelimit(i)*changelimit(i)/sigma(i);
        }

        tau1 = Min(tau3);
        tau2 = Min(tau4); 
 
	if (tau1 > tau2) 
	  return tau2; 
	else 
	  return tau1; 
    }

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
