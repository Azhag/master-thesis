//*****************************************************************************|
//*  FILE:    SolutionPt.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 20, 2003
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
#include "SolutionPt.h"
#include "IEEE.h"


namespace CSE {
  namespace StochRxn {


    SolutionPt::SolutionPt()
      : mTime(Math::NaN())
      , mNextStep(Math::NaN()) 
    {
    }


    SolutionPt::SolutionPt(double t, const Vector& x, double dt)
      : mTime(t)
      , mNextStep(dt)
      , mState(x)
    {
    }

    
    double SolutionPt::Time() const
    {
      return mTime;
    }


    const Vector& SolutionPt::State() const
    {
      return mState;
    }


    double SolutionPt::NextStep() const
    {
      return mNextStep;
    }


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

