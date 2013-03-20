#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR:  Andrew Hall
#*
#*  CREATED: January 9, 2003
#*
#*  LAST MODIFIED: Aug 10, 2004
#*             BY: Yang Cao
#*             TO: Use LAPACK linear solver (Option)
#*
#*  LAST MODIFIED:  Thu Aug  5 15:26:24 PDT 2004
#*             BY:  slampoud
#*             TO:  
#*
#*  LAST MODIFIED:  Tue Jul 27 11:17:22 PDT 2004
#*             BY:  slampoud
#*             TO:  
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


OBJS = \
	com.o \
	Errors.o \
	IEEE.o \
	linpack.o \
	Matrix.o \
	MatrixOps.o \
	Random.o \
	ranlib.o \
	SolveGE.o \
	Vector.o \
	VectorOps.o

lib : 	${OBJS}
	ar cruv libCSE_Math.a ${OBJS} 

com.o : \
	ranlib.h \
	com.c
	${C} -c com.c ${CPP_OPTIONS}


Errors.o : \
	Errors.h \
	Errors.cpp
	${CPP} -c Errors.cpp ${CPP_OPTONS}


IEEE.o : \
	IEEE.h \
	IEEE.cpp
	${CPP} -c IEEE.cpp ${CPP_OPTIONS}


linpack.o : \
	linpack.c
	${C} -c linpack.c ${CPP_OPTIONS}


Matrix.o : \
	Errors.h \
	IEEE.h \
	Vector.h \
	VectorOps.h \
	Matrix.h \
	Matrix.cpp
	${CPP} -c Matrix.cpp ${CPP_OPTIONS}


MatrixOps.o : \
	Matrix.h \
	MatrixOps.h \
	MatrixOps.cpp
	${CPP} -c MatrixOps.cpp ${CPP_OPTIONS}


Random.o : \
	IEEE.h \
	ranlib.h \
	Vector.h \
	Random.h \
	Random.cpp
	${CPP} -c Random.cpp ${CPP_OPTIONS} ${SPRNG_INCLUDE}


ranlib.o : \
	ranlib.h \
	ranlib.c 
	${C} -c ranlib.c ${CPP_OPTIONS} ${SPRNG_INCLUDE}


SolveGE.o : \
	IEEE.h \
	Errors.h \
	Vector.h \
	Matrix.h \
	SolveGE.h \
	SolveGE.cpp
	${CPP} -c SolveGE.cpp ${CPP_OPTIONS} -I$(LAPACKPP_INC) -DUSELAPACK
#	${CPP} -c SolveGE.cpp ${CPP_OPTIONS} -I$(LAPACKPP_INC)

Vector.o : \
	Errors.h \
	IEEE.h \
	Matrix.h \
	VectorOps.h \
	Vector.h \
	Vector.cpp
	${CPP} -c Vector.cpp ${CPP_OPTIONS}


VectorOps.o : \
	Errors.h \
	IEEE.h \
	Matrix.h \
	Vector.h \
	VectorOps.h \
	VectorOps.cpp
	${CPP} -c VectorOps.cpp ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} core* *~ *.a
