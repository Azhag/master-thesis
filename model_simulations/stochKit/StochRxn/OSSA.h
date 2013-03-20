#ifndef CSE_STOCHRXN_OSSA_H
#define CSE_STOCHRXN_OSSA_H
//*****************************************************************************|
//*  FILE:    SSA.h
//*
//*  AUTHOR:  HONG
//*
//*  CREATED: Mar 25, 2005
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
#include "VectorFwd.h"
#include "MatrixFwd.h"
#include "Propensity.h"


namespace CSE {
  namespace StochRxn {

    double OSSADirect_Stepsize(Vector& x,
                              Vector& a,
                              double& a0,
                              const Matrix& nu,
                              double tau,
                              double eps,
                              PropensityJacobianFunc j);
    

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
                        Vector& p);
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_OSSA_H
