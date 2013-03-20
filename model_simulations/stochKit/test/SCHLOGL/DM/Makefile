#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR:  Andrew Hall
#*
#*  CREATED: January 31, 2003
#*
#*  LAST MODIFIED: Aug 11, 2004
#*             BY: Yang Cao
#*             TO: Add LAPACK, and many test models
#*
#*  LAST MODIFIED:  Tue Jul 27 11:17:22 PDT 2004
#*             BY:  slampoud
#*             TO:  added sprng
#*
#*  LAST MODIFIED:  Thu Nov 23 11:17:22 PDT 2004
#*             BY:  Hong Li
#*             TO:  Standard the demo models
#*
#*  LAST MODIFIED:  Sun Mar 6 11:17:22 PDT 2005
#*             BY:  Hong Li
#*             TO:  Standard the demo models
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
CSE_CPP_HOME = ../../..
include ../../../makefile.config

OBJS = schloglstats \
	singleschlogl

all : ${OBJS}

schloglstats : SchloglStats.o ProblemDefinition.o
	${CPP} -o schloglstats SchloglStats.o ProblemDefinition.o ${LINK_OPTIONS} 

singleschlogl : singleSchlogl.o ProblemDefinition.o
	${CPP} -o singleschlogl singleSchlogl.o ProblemDefinition.o ${LINK_OPTIONS} 

SchloglStats.o: SchloglStats.cpp 
	${CPP} -c SchloglStats.cpp ${CPP_OPTIONS}	

singleSchlogl.o: singleSchlogl.cpp 
	${CPP} -c singleSchlogl.cpp ${CPP_OPTIONS}	

ProblemDefinition.o : ProblemDefinition.cpp 
	${CPP} -c ProblemDefinition.cpp ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} *.o core* *~
