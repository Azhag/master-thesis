#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR:  Hong Li
#*
#*  CREATED: July 1, 2005
#*
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
