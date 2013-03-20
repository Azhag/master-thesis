#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR:  Andrew Hall
#*
#*  CREATED: January 9, 2003
#*
#*  LAST MODIFIED: Nov. 8, 2004 
#*             BY: Yang Cao
#*             TO: Add Gillespie and GillespiePetzold stepsize selection method
#*
#*  LAST MODIFIED: Nov. 18, 2004 
#*             BY: HONG
#*             TO: Standard the demo 
#*
#*  SUMMARY:
#*
#*
#*  NOTES:
#*
#*
#*
#*  TO DO:
#*
#*
#******************************************************************************|
#        1         2         3         4         5         6         7         8
#2345678901234567890123456789012345678901234567890123456789012345678901234567890
CSE_CPP_HOME = ..
include ../makefile.config

OBJS = CollectStats.o \
	ExplicitTau_SingleStep.o \
	FixedStepsize.o \
	Trapezoidal_SingleStep.o \
	ImplicitTrapezoidal_SingleStep.o \
	Midpoint_SingleStep.o \
	ImplicitTau_SingleStep.o \
	SolutionPt.o \
	SSA.o \
	StochRxn.o \
	StoreState.o \
	GillespieStepsize.o \
	GillespiePetzoldStepsize.o \
	CaoStepsize.o \
	SolverOptions.o	 

lib : \
	${OBJS}
	ar cruv libCSE_StochRxn.a ${OBJS}

CollectStats.o : \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	ReactionSet.h \
	SolverOptions.h \
	SolutionHistory.h\
	SolutionPt.h \
	StochRxn.h \
	CollectStats.h \
	CollectStats.cpp 
	${CPP} -c CollectStats.cpp ${CPP_OPTIONS}


ExplicitTau_SingleStep.o : \
	${CSE_MATH}/Random.h \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	ExplicitTau_SingleStep.h \
	ExplicitTau_SingleStep.cpp
	${CPP} -c ExplicitTau_SingleStep.cpp ${CPP_OPTIONS}

Midpoint_SingleStep.o : \
	${CSE_MATH}/Random.h \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	Midpoint_SingleStep.h \
	Midpoint_SingleStep.cpp
	${CPP} -c Midpoint_SingleStep.cpp ${CPP_OPTIONS}

Trapezoidal_SingleStep.o : \
	${CSE_MATH}/Random.h \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	Trapezoidal_SingleStep.h \
	Trapezoidal_SingleStep.cpp 
	${CPP} -c Trapezoidal_SingleStep.cpp ${CPP_OPTIONS}

ImplicitTrapezoidal_SingleStep.o: \
	${CSE_MATH}/Random.h \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	${CSE_MATH}/MatrixOps.h \
	${CSE_MATH}/VectorOps.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	ImplicitTrapezoidal_SingleStep.h \
	ImplicitTrapezoidal_SingleStep.cpp
	${CPP} -c ImplicitTrapezoidal_SingleStep.cpp ${CPP_OPTIONS}

FixedStepsize.o : \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	FixedStepsize.h \
	FixedStepsize.cpp
	${CPP} -c FixedStepsize.cpp ${CPP_OPTIONS}

GillespieStepsize.o: ${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	GillespieStepsize.h \
	GillespieStepsize.cpp
	${CPP} -c GillespieStepsize.cpp ${CPP_OPTIONS} 

GillespiePetzoldStepsize.o: ${CSE_MATH}/Vector.h ${CSE_MATH}/Matrix.h  VectorFwd.h \
	MatrixFwd.h  Propensity.h  GillespiePetzoldStepsize.h  GillespiePetzoldStepsize.cpp
	${CPP} -c GillespiePetzoldStepsize.cpp ${CPP_OPTIONS}       

CaoStepsize.o: ${CSE_MATH}/Vector.h ${CSE_MATH}/Matrix.h  VectorFwd.h \
	MatrixFwd.h  Propensity.h  CaoStepsize.h  CaoStepsize.cpp
	${CPP} -c CaoStepsize.cpp ${CPP_OPTIONS}   

ImplicitTau_SingleStep.o : \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	${CSE_MATH}/MatrixOps.h \
	${CSE_MATH}/VectorOps.h \
	${CSE_MATH}/Random.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	ImplicitTau_SingleStep.h \
	ImplicitTau_SingleStep.cpp
	${CPP} -c ImplicitTau_SingleStep.cpp ${CPP_OPTIONS}


SolutionPt.o : \
	${CSE_MATH}/IEEE.h \
	${CSE_MATH}/Vector.h \
	VectorFwd.h \
	SolutionPt.h \
	SolutionPt.cpp
	${CPP} -c SolutionPt.cpp ${CPP_OPTIONS}


SSA.o : \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/Matrix.h \
	${CSE_MATH}/Random.h \
	VectorFwd.h \
	MatrixFwd.h \
	Propensity.h \
	SSA.h \
	SSA.cpp
	${CPP} -c SSA.cpp ${CPP_OPTIONS}


StochRxn.o : \
	${CSE_MATH}/IEEE.h \
	${CSE_MATH}/Vector.h \
	${CSE_MATH}/VectorOps.h \
	${CSE_MATH}/Matrix.h \
	SolutionPt.h \
	SolutionHistory.h \
	Propensity.h \
	ReactionSet.h \
	SolverOptions.h \
	StochRxn.h \
	StochRxn.cpp
	${CPP} -c StochRxn.cpp ${CPP_OPTIONS}


StoreState.o : \
	${CSE_MATH}/Vector.h \
	SolutionPt.h \
	SolutionHistory.h \
	StoreState.h \
	StoreState.cpp
	${CPP} -c StoreState.cpp ${CPP_OPTIONS}

SolverOptions.o : \
	SolverOptions.h \
	 ImplicitTau_SingleStep.h \
	SolverOptions.cpp \
	 ImplicitTau_SingleStep.cpp
	${CPP} -c SolverOptions.cpp  ImplicitTau_SingleStep.cpp ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} *.o core* *.a *~
