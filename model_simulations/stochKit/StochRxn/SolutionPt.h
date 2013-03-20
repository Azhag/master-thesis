#ifndef CSE_STOCHRXN_SOLUTIONPT_H
#define CSE_STOCHRXN_SOLUTIONPT_H
//*****************************************************************************|
//*  FILE:    SolutionPt.h
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
#include "VectorFwd.h"


namespace CSE {
  namespace StochRxn {

    class SolutionPt
    {
      public:
        SolutionPt();
        SolutionPt(double t, const Vector& x, double dt);

        double Time() const;
        const Vector& State() const;
        double NextStep() const;

      private:
        double mTime;
        double mNextStep;
        Vector mState;
    };

  } // Close CSE::StochRxn namespace
} // Close CSE namespace


#endif // CSE_STOCHRXN_SOLUTIONPT_H
