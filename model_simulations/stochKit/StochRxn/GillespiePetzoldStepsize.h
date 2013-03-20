#ifndef CSE_STOCHRXN_GILLESPIEPETZOLDSTEPSIZE_H
#define CSE_STOCHRXN_GILLESPIEPETZOLDSTEPSIZE_H
//*****************************************************************************|
//*  FILE:    GillespiePetzoldStepsize.h
//*
//*  AUTHOR:  Yang Cao
//*
//*  CREATED: Nov. 8, 2004
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
#include "VectorOps.h"
#include "MatrixOps.h"
#include "IEEE.h"
#include "Propensity.h"

using namespace CSE::Math; 

namespace CSE {
  namespace StochRxn {
    
    double GillespiePetzold_Stepsize( Vector& x,
                          Vector& a,
                          double& a0,
                          const Matrix& nu,
                          double tau,
                          double eps,
                          PropensityJacobianFunc j);
    

  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_FIXEDSTEPSIZE_H
