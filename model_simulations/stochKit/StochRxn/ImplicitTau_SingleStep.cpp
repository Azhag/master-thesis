//*****************************************************************************|
//*  FILE:    ImplicitTau_SingleStep.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 28, 2003
//*
//*  MODIFIED: Aug 11, 2004 
//*        BY: Yang Cao
//*        TO:  
//*
//*  MODIFIED: December 2, 2004 
//*        BY: HONG 
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
#include "ImplicitTau_SingleStep.h"
#include "VectorOps.h"
#include "MatrixOps.h"
#include "Random.h"
#include "SolveGE.h"
#include <iostream>


using namespace CSE::Math;


namespace CSE {
  namespace StochRxn {

    void ImplicitTau_SingleStep(Vector& x,
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
      ImplicitTau_kround_SingleStep(x, t, tau, a, a0, nu,
                                    propFunc, propJacFunc, abs_tol, rel_tol, RXN, p);
    }


    void ImplicitTau_kround_SingleStep(Vector& x,
                                       double t,
                                       double & tau,
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
      const Vector E = (nu * p) - (tau * A);
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
        AA = I - tau * nu * propJacFunc(xPlusDelx);
        BB = E + tau * nu * propFunc(xPlusDelx) - delx;
        SolveGEIP(AA, deldelx, BB); // AA gets changed if using LAPACK linear solver
	// SolveGE(AA, deldelx, BB); // AA doesn't get changed 

        converged = (deldelx.Norm2() <= rel_tol * delx.Norm2() + abs_tol);

        delx += deldelx;
        ++iterations;
      }

      if (iterations >= 10 && !converged) {
        std::cerr << "Warning:  ImplicitTau_kround_SingleStep: 10 iterations"
                  << " without convergence!\n";
      }

      p = Round((propFunc(x + delx) - a) * tau + p);
    } 

    void ImplicitTau_New_SingleStep(Vector& x,
                                       double t,
                                       double & tau,
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
      static Vector A = x;
      static Vector delx(x.Size(), 0);
      bool converged = false;
      int iterations = 0;
      const Matrix I = Identity(x.Size());
      static Vector xPlusDelx(x.Size());
      static Matrix AA = I;
      static Vector BB = x;
      static Vector deldelx = x;
      while (!converged && iterations < 10) {
        xPlusDelx = x+ delx;
        AA = I - tau * nu * propJacFunc(xPlusDelx);
        BB = tau * nu * propFunc(xPlusDelx) - delx;
        SolveGE(AA, deldelx, BB);

        converged = (deldelx.Norm2() <= rel_tol * delx.Norm2() + abs_tol);

        delx += deldelx;
        ++iterations;
      }

      if (iterations >= 10 && !converged) {
        std::cerr << "Warning:  ImplicitTau_kround_SingleStep: 10 iterations"
                  << " without convergence!\n";
      }
      p = PoissonRandom(propFunc(xPlusDelx) * tau);;
    }


    void ImplicitTau_xround_SingleStep(Vector& x,
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
    }


    void ImplicitTau_trunc_SingleStep(Vector& x,
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
    }


    void ImplicitTau_real_SingleStep(Vector& x,
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
      
    }



  } // Close CSE::StochRxn namespace
} // Close CSE namespace
