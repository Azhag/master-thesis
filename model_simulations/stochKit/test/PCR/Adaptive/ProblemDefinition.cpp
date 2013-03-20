#include "ProblemDefinition.h"

Vector Initialize()
{
 	Vector x0(8, 0.0);

	x0(0) = 11;
	x0(1) = 11; 
	x0(2) = 0; 
	x0(3) = 4.5165e12; 
	x0(4) = 0; 
	x0(5) = 4.5165e12;
	x0(6) = 0; 
	x0(7) = 0; 
	 
	return x0;
} 

Matrix Stoichiometry()
{
   Matrix nu(8, 8, 0.0);

   nu(0,0) = -1; nu(1,0) = -1; nu(2,0) = 1; 
   nu(0,1) = 1;  nu(1,1) = 1 ; nu(2,1) = -1;
   nu(1,2) = -1; nu(3,2) = -1; nu(4,2) = 1;
   nu(1,3) = 1;  nu(3,3) = 1;  nu(4,3) = -1; 
   nu(1,4) = -1; nu(5,4) = -1; nu(6,4) = 1; 
   nu(1,5) = 1;  nu(5,5) = 1;  nu(6,5) = -1; 
   nu(3,6) = -1; nu(7,6) = 1; 
   nu(3, 7) = 1;  nu(7,7) = -1;
   
   return nu;
}

Vector Propensity(const Vector& x)
{
   const double c1=1.801e-13, c2=0, c3=c1, c4=0.0032;
   const double c5= 2.227e-14, c6= 0.2146, c7 = 10032, c8 = 8491; 
   Vector a(8);
	
   a(0) = c1*x(0)*x(1);
   a(1) = c2*x(2); 
   a(2) = c3*x(0)*x(3);
   a(3) = c4*x(4);
   a(4) = c5*x(0)*x(5); 
   a(5) = c6*x(6);
   a(6) = c7*x(3); 
   a(7) = c8*x(7); 
	
   return a;
}
