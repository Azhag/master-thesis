#include "ProblemDefinition.h"

Vector Initialize()
{
 	Vector x0(1, 0.0);

	x0(0) = 250;
	return x0;
} 

Matrix Stoichiometry()
{
   Matrix nu(1, 4, 0.0);

   nu(0,0) = 1; nu(0,1) = -1; 
   nu(0,2) = 1; nu(0,3) = -1;
   
   return nu;
}

Vector Propensity(const Vector& x)
{
   double c1 = 0.03, c2 = 0.0001, c3 = 200, c4 = 3.5;
   Vector a(4);
   a(0) = c1/2. * x(0)*(x(0) -1);
   a(1) = (c2/6.0)*x(0)*(x(0)-1)*(x(0)-2);
   a(2) = c3 ;
   a(3) = c4 * x(0);

   return a;
}

