//*****************************************************************************|
//*  FILE:    ImplicitTrapzoidal_SingleStep.cpp
//*
//*  AUTHOR:  Yang Cao
//*
//*  CREATED: September, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: Change functions for SolveGE 
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
#include "ImplicitTrapezoidal_SingleStep.h"
#include "VectorOps.h"
#include "MatrixOps.h"
#include "Random.h"
#include "SolveGE.h"
#include <iostream>

using namespace CSE::Math;

namespace CSE {
  namespace StochRxn {


    void ImplicitTrapezoidal_SingleStep(Vector& x,
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
      p = PoissonRandom(a * tau);
      const Vector A = nu * a;
      const Vector E = (nu * p) - (0.5* tau * A);
      Vector delx(x.Size(), 0);
      bool converged = false;
      int iterations = 0;
      const Matrix I = Identity(x.Size());
      Vector xPlusDelx(x.Size());
      static Matrix AA = I;
      static Vector BB = E;
      static Vector deldelx = x;
      static Vector K = p; 
      while (!converged && iterations < 10) {
        xPlusDelx = x+ delx;
        AA = I - 0.5*tau * nu * propJacFunc(xPlusDelx);
        BB = E + 0.5*tau * nu * propFunc(xPlusDelx) - delx;
        SolveGEIP(AA, deldelx, BB);

        converged = (deldelx.Norm2() <= rel_tol * delx.Norm2() + abs_tol);

        delx += deldelx;
        ++iterations;
      }

      if (iterations >= 10 && !converged) {
        std::cerr << "Warning:  ImplicitTrapzoidal_SingleStep: 10 iterations"
                  << " without convergence!\n";
      }

      K = (0.5*(propFunc(x + delx) - a) * tau + p);
      p = Round(K); 
    }
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace
