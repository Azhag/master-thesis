#include "ProblemDefinition.h"

Vector Initialize()
{
 	Vector x0(3, 0.0);

	x0(0) = 10000; 
	return x0;
} 

Matrix Stoichiometry()
{
   Matrix nu(3, 4, 0.0);

   nu(0,0) = -1; nu(0,1) = -2; nu(0,2) = 2;
   nu(1,1) = 1; nu(1,2) = -1; nu(1,3) = -1;
   nu(2,3) = 1;
   
   return nu;
}

Vector Propensity(const Vector& x)
{
   double c1=1, c2=0.002, c3=0.5, c4=0.04;
//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
   Vector a(4);
   a(0) = c1*x(0);
   a(1) = (c2/2.0)*x(0)*(x(0)-1);
   a(2) = c3*x(1);
   a(3) = c4*x(1);
   return a;
}


Matrix PropensityJacobian(const Vector& x)
{
    double c1=1, c2=0.002, c3=0.5, c4=0.04;
//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
   Matrix j(4,3,0.0);
   j(0,0) = c1;
   j(1,0) = c2 * x(0) - c2/2.0;
   j(2,1) = c3;
   j(3,1) = c4; 
   return j;
}

