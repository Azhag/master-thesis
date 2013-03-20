#ifndef CSE_STOCHRXN_REACTIONSET_H
#define CSE_STOCHRXN_REACTIONSET_H
//*****************************************************************************|
//*  FILE:    ReactionSet.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
//*
//*  LAST MODIFIED: HONG Li
//*             BY:  June 2, 2004
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
#include "MatrixFwd.h"
#include "Propensity.h"


namespace CSE {
  namespace StochRxn {


    class ReactionSet
    {
      public:
        ReactionSet(const Matrix& nu,
                    const Matrix& dg,
                    PropensityFunc prop,
                    PartialPropensityFunc pprop,
                    PropensityJacobianFunc propJac,
                    EquilibriumFunc equilibrium)
          : mNu(nu)
          , mDg(dg)
          , mPropensity(prop)
          , mPartialPropensity(pprop)
          , mPropensityJacobian(propJac)
          , mEquilibrium(equilibrium)
        {
        }

        ReactionSet(const Matrix& nu,
                    PropensityFunc prop)
          : mNu(nu)
          , mPropensity(prop)
        {
	  mPropensityJacobian = NULL; 
	  mEquilibrium = NULL; 
        }

        ReactionSet(const Matrix& nu,
                    PropensityFunc prop,
                    PropensityJacobianFunc propJac)
          : mNu(nu)
          , mPropensity(prop)
          , mPropensityJacobian(propJac)
        {
	  mEquilibrium = NULL;  
        }
        ReactionSet(const Matrix& nu,
                    PropensityFunc prop,
                    EquilibriumFunc equilibrium)
          : mNu(nu)
          , mPropensity(prop)
          , mEquilibrium(equilibrium)
        {
	   mPropensityJacobian = NULL; 
        }

        ReactionSet(const Matrix& nu,
                    const Matrix& dg,
                    PropensityFunc prop,
                    PartialPropensityFunc pprop)
          : mNu(nu)
          , mDg(dg)
          , mPropensity(prop)
          , mPartialPropensity(pprop)
        {
	   mPropensityJacobian = NULL;
	   mEquilibrium = NULL;  
        }

        const Matrix& Nu() const
        {
          return mNu;
        }

        const Matrix& DG() const
        {
          return mDg;
        }

        PropensityFunc Propensity() const
        {
          return mPropensity;
        }
        
        PartialPropensityFunc PartialPropensity() const
        {
          return mPartialPropensity;
        }
        
        PropensityJacobianFunc PropensityJacobian() const
        {
          return mPropensityJacobian;
        }

        EquilibriumFunc Equilibrium() const
        {
           return mEquilibrium;
        }

      private:
        Matrix mNu ;
        Matrix mDg ;
        PropensityFunc mPropensity ;
        PartialPropensityFunc mPartialPropensity ;
        PropensityJacobianFunc mPropensityJacobian ;
        EquilibriumFunc mEquilibrium ;
    };


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

#endif // CSE_STOCHRXN_REACTIONSET_H
