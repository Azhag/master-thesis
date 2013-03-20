#include "ProblemDefinition.h"

Vector Initialize()
{
 	Vector x0(6, 0.0);

	x0(0) = 1;
	x0(4) = 100;
	x0(5) = 5;
	 
	return x0;
} 

Matrix Stoichiometry()
{
   Matrix nu(6, 10, 0.0);

   nu(0,0) = -1; nu(0,1) = 1; nu(0,2) = -1; nu(0,3) = 1;
   nu(1,0) = 1; nu(1,1) = -1; nu(1,4) = -1; nu(1,5) = 1;
   nu(2,2) = 1; nu(2,3) = -1; nu(2,6) = -1; nu(2,7) = 1;
   nu(3,4) = 1; nu(3,5) = -1; nu(3,6) = 1; nu(3,7) = -1;
   nu(4,0) = -1; nu(4,1) = 1; nu(4,2) = -1; nu(4,3) = 1; nu(4,4) = -1; nu(4,5) = 1;
   nu(4,6) = -1; nu(4,7) = 1;
   nu(5,8) = 1;
   nu(5,9) = -1;
   
   return nu;
}

Vector Propensity(const Vector& x)
{
   double c1=100, c2=100, ct=1000, cd=100;
//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
   Vector a(10);
   a(0) = c1*x(0)*x(4);
   a(2) = c1*x(0)*x(4);
   a(4) = c1/100*x(1)*x(4);
   a(6) = c1/100*x(2)*x(4);
	
	a(1) = c2*x(1)*(2.5-2.25*double(x(5)/double(1+x(5))));
	a(3) = c2*x(2)*(1.2-0.2*double(x(5)/double(1+x(5))));
	a(5) = c2*x(3)*(1.2-0.2*double(x(5)/double(1+x(5))));
	a(7) = c2*x(3)*(2.5-2.25*double(x(5)/double(1+x(5))));

   a(8) = ct*x(1);
   a(9) = cd*x(5);
   return a;
}
