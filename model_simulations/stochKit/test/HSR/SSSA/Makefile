#******************************************************************************|
#*  FILE:    Makefile
#*
#*  AUTHOR: Hong Li 
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

OBJS = hsrstats \
		singlehsr

all : ${OBJS}

hsrstats : HsrStats.o ProblemDefinition.o
	${CPP} -o hsrstats HsrStats.o ProblemDefinition.o ${LINK_OPTIONS} 

singlehsr : SingleHsr.o ProblemDefinition.o
	${CPP} -o singlehsr SingleHsr.o ProblemDefinition.o ${LINK_OPTIONS} 

HsrStats.o: HsrStats.cpp 
	${CPP} -c HsrStats.cpp ${CPP_OPTIONS}	

SingleHsr.o: SingleHsr.cpp 
	${CPP} -c SingleHsr.cpp ${CPP_OPTIONS}	

ProblemDefinition.o : ProblemDefinition.cpp 
	${CPP} -c ProblemDefinition.cpp ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} *.o core* *~
