#ifndef CSE_STOCHRXN_MIDPOINT_SINGLESTEP
#define CSE_STOCHRXN_MIDPOINT_SINGLESTEP_H
//*****************************************************************************|
//*  FILE:    ExplicitTau_SingleStep.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 15, 2003
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
                                Vector& p);
    


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_EXPLICITTAU_SINGLESTEP_H
