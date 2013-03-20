//*****************************************************************************|
//*  FILE:    SolverOptions.cpp
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: July 30, 2003
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
#include "SolverOptions.h"
#include "ExplicitTau_SingleStep.h"
#include "ImplicitTau_SingleStep.h"
#include "CaoStepsize.h"
#include "SSA.h"
#include "FixedStepsize.h"
#include "GillespieStepsize.h"
#include "StoreState.h"
#include <stdarg.h>
#include <string>
#include <stdexcept>



namespace CSE {
  namespace StochRxn {


    SolverOptions ConfigStochRxn(int fixedoption)
    {
      SolverOptions solvOpt;

      // Configure an options structure with the defaults,
      // then we'll modify them as necessary in response to
      // the argument strings.
	if (fixedoption == 1) { 
          solvOpt.stepsize_selector_func = &SSADirect_Stepsize;
          solvOpt.single_step_func = &SSA_SingleStep;
          solvOpt.store_state_func = &Exponential_StoreState;
          solvOpt.absolute_tol = 1e-6;
          solvOpt.relative_tol = 1e-5;
          solvOpt.initial_stepsize = 1e-6;
          solvOpt.epsilon = 0.05;
          solvOpt.progress_interval = 1000L;
          solvOpt.StepControl = 0; 
	} else { 
	  solvOpt.stepsize_selector_func = &Cao_Stepsize;
          solvOpt.single_step_func = &ExplicitTau_SingleStep;
          solvOpt.store_state_func = &Exponential_StoreState;
          solvOpt.absolute_tol = 1e-6;
          solvOpt.relative_tol = 1e-5;
          solvOpt.initial_stepsize = 1e-6;
          solvOpt.epsilon = 0.03;
          solvOpt.progress_interval = 1L;
          solvOpt.StepControl = 1;    
	} 
      return solvOpt;
    }

    SolverOptions ConfigStochRxn()    
    { 
	SolverOptions solvOpt; 
	  solvOpt.stepsize_selector_func = &Cao_Stepsize;
          solvOpt.single_step_func = &ExplicitTau_SingleStep;
          solvOpt.store_state_func = &Exponential_StoreState;
          solvOpt.absolute_tol = 1e-6;
          solvOpt.relative_tol = 1e-5;
          solvOpt.initial_stepsize = 1e-6;
          solvOpt.epsilon = 0.03;
          solvOpt.progress_interval = 1L;
          solvOpt.StepControl = 1;  
	return solvOpt;        
    } 


  } // Close CSE::StochRxn namespace
} // Close CSE namespace

