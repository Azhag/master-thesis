//*****************************************************************************|
//*  FILE:    SSA.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 25, 2003
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
#include "SSA.h"
#include "Random.h"
#include "math.h"
#include <iostream>


namespace CSE {
  namespace StochRxn {


    double SSADirect_Stepsize( Vector& /* x */,
                               Vector& /* a */,
                              double & a0,
                              const Matrix& /* nu */,
                              double /* tau */,
                              double /* eps */,
                              PropensityJacobianFunc /* j */)
    {
      //return ( 1 / a0 ) * log( 1 / CSE::Math::UniformRandom() );
      return -log( CSE::Math::UniformRandom() )/a0;
    }
    

    void SSA_SingleStep(Vector& x,
                        double t,
                        double &tau,
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
      const double f = CSE::Math::UniformRandom() * a0;

      int rxn = -1;
      double jsum = 0.0;
      const int M = nu.Cols();

      while ((jsum < f) && (rxn < M)) {
        ++rxn;
        jsum += a(rxn);
      }

      x += nu.Col(rxn);
      RXN = rxn;
    }

    double OSSADirect_Stepsize( Vector& /* x */,
                               Vector& /* a */,
                              double & a0,
                              const Matrix& /* nu */,
                              double /* tau */,
                              double /* eps */,
                              PropensityJacobianFunc /* j */)
    {
      //return ( 1 / a0 ) * log( 1 / CSE::Math::UniformRandom() );
      return -log( CSE::Math::UniformRandom() )/a0;
    }
    

    void OSSA_SingleStep(Vector& x,
                        double t,
                        double &tau,
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
      const double f = CSE::Math::UniformRandom() * a0;

      int rxn = -1;
      double jsum = 0.0;
      const int M = nu.Cols();

      while ((jsum < f) && (rxn < M)) {
        ++rxn;
        jsum += a(rxn);
      }

      x += nu.Col(rxn);
      RXN = rxn;
    }
    
    double MSSADirect_Stepsize( Vector& /* x */,
                               Vector& /* a */,
                              double & a0,
                              const Matrix& /* nu */,
                              double /* tau */,
                              double /* eps */,
                              PropensityJacobianFunc /* j */)
    {
      //return ( 1 / a0 ) * log( 1 / CSE::Math::UniformRandom() );
      return -log( CSE::Math::UniformRandom() )/a0;
    }
    

    void MSSA_SingleStep(Vector& x,
                        double t,
                        double &tau,
                        Vector& a,
                        double a0,
                        const Matrix& nu,
                        PropensityFunc propFunc,
                        PropensityJacobianFunc propJacFunc,
                        double abs_tol,
                        double rel_tol,
                        int& RXN,
			Vector & p)
    {
      const double f = CSE::Math::UniformRandom() * a0;

      int rxn = -1;
      double jsum = 0.0;
      const int M = nu.Cols();

      while ((jsum < f) && (rxn < M)) {
        ++rxn;
        jsum += a(rxn);
      }

      x += nu.Col(rxn);
      RXN = rxn;
    }
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

