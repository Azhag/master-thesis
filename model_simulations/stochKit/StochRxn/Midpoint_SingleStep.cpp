//*****************************************************************************|
//*  FILE:    MidpointTau_SingleStep.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 15, 2003
//*
//*  MODIFIED:  June 4, 2004
//*        BY:  Hong Li
//*        TO:  
//*
//*  MODIFIED:  
//*        BY:  
//*        TO:  
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
#include "Midpoint_SingleStep.h"
#include "VectorOps.h"
#include "MatrixOps.h"
#include "Random.h"
#include "SolveGE.h" 
#include <iostream>

using namespace CSE::Math;

namespace CSE {
  namespace StochRxn {


    void Midpoint_SingleStep(Vector& x,
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
      p = a; 
      // static Vector x1 = x; 
      static Vector a1 = a; 
      // static Vector delx = x; 
/*
      p = PoissonRandom(0.5* a * tau); // better midpoint method
      p = 0.5* a * tau; // original midpoint method
      x1 = x + nu*p; 
*/
      const Vector A = nu * a;
      const Vector E = 0.5* tau * A;
      Vector delx(x.Size(), 0);
      bool converged = false;
      int iterations = 0;
      const Matrix I = Identity(x.Size());
      Vector xPlusDelx(x.Size());
      static Matrix AA = I;
      static Vector BB = E;
      static Vector deldelx = x;
      while (!converged && iterations < 10) {
        xPlusDelx = x+ delx;
        AA = I - 0.5*tau * nu * propJacFunc(xPlusDelx);
        BB = E + 0.5*tau * nu * propFunc(xPlusDelx) - delx;
        SolveGE(AA, deldelx, BB);

        converged = (Norm(deldelx, 2) <= rel_tol * Norm(delx, 2) + abs_tol);
        delx += deldelx;
        ++iterations;
      }
      a1 = propFunc(x + delx); 
      a1 = 0.5*(a + a1)*tau; 
      p = PoissonRandom(a1);
    }
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
