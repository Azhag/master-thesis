#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR:  Hong Li
#*
#*  CREATED: Jul. 1, 2005
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

OBJS = dimerstats \
		dimersingle

all : ${OBJS}

dimerstats : DimerStats.o ProblemDefinition.o
	${CPP} -o dimerstats DimerStats.o ProblemDefinition.o ${LINK_OPTIONS} 

dimersingle : SingleDriver.o ProblemDefinition.o
	${CPP} -o dimersingle SingleDriver.o ProblemDefinition.o ${LINK_OPTIONS} 

DimerStats.o: DimerStats.cpp 
	${CPP} -c DimerStats.cpp ${CPP_OPTIONS}	

SingleDriver.o: SingleDriver.cpp 
	${CPP} -c SingleDriver.cpp ${CPP_OPTIONS}	

ProblemDefinition.o : ProblemDefinition.cpp 
	${CPP} -c ProblemDefinition.cpp ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} *.o core* 
