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

    nu(0,0) = 1;

    nu(0,1) = -1;

    nu(0,2) = 1;

    nu(0,3) = -1;

    return nu;
}

Matrix DependentGrapth()
{
    Matrix DG(5, 5, 0.0);

     DG(0,0) = 0; 
     DG(0,1) = 1; 
     DG(0,2) = 3; 
     DG(0,3) = -1; 

     DG(1,0) = 0; 
     DG(1,1) = 1; 
     DG(1,2) = 3; 
     DG(1,3) = -1; 

     DG(2,0) = 0; 
     DG(2,1) = 1; 
     DG(2,2) = 3; 
     DG(2,3) = -1; 

     DG(3,0) = 0; 
     DG(3,1) = 1; 
     DG(3,2) = 3; 
     DG(3,3) = -1; 

   return DG;
}

Vector Propensity(const Vector& x)
{
    double
    c1=0.03,
    c2=0.0001,
    c3=200,
    c4=3.5;

    Vector a(4, 0.0);

    a(0) = (c1/2.0)*x(0)*(x(0)-1);
    a(1) = (c2/6.0)*x(0)*(x(0)-1)*(x(0)-2);
    a(2) = c3;
    a(3) = c4*x(0);
    return a;
}

Vector PartialProp(int RIndex, const Vector& x, const Matrix& dg, Vector& a, double& a0)
{
    double
    c1=0.03,
    c2=0.0001,
    c3=200,
    c4=3.5;


    int i=0, tempI=0;
    while(dg(RIndex, i) != -1){
         tempI = int(dg(RIndex, i));
         i++;
         switch(tempI)
         {
             case 0:
             {
                  a0 = a0 -a(0);
                  a(0) = (c1/2.0)*x(0)*(x(0)-1);
                  a0 = a0 + a(0);
             }
             case 1:
             {
                  a0 = a0 -a(1);
                  a(1) = (c2/6.0)*x(0)*(x(0)-1)*(x(0)-2);
                  a0 = a0 + a(1);
             }
             case 2:
             {
                  a0 = a0 -a(2);
                  a(2) = c3;
                  a0 = a0 + a(2);
             }
             case 3:
             {
                  a0 = a0 -a(3);
                  a(3) = c4*x(0);
                  a0 = a0 + a(3);
             }
        } 
    }
    return a;
}

Matrix PropensityJacobian(const Vector& x)
{
    double
    c1=0.03,
    c2=0.0001,
    c3=200,
    c4=3.5;

    Matrix j(4,1,0.0);

    j(0,0) = (c1/2.0)*x(0)*2-(c1/2.0);
    j(1,0) = (c2/6.0)*x(0)*(x(0)-2)*2-(c2/6.0)*(x(0)-2);
    j(3,0) = c4;

    return j;
}

void Equilibrium(Vector& x, Vector& a, int rxn)
{
      }
