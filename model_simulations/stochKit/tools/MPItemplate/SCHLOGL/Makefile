#******************************************************************************
#*  FILE:  Makefile
#*
#*  AUTHOR: HongLi
#*
#*  CREATED: Mar 06, 2005
#*
#*  LAST MODIFIED:
#*             BY:
#*             TO:
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
#****************************************************************************|
#        1         2         3         4         5         6         7         8
#//2345678901234567890123456789012345678901234567890123456789012345678901234567890

CSE_CPP_HOME = ../../..
include ../../../makefile.config

MPICC = mpiCC

EXE	= p_schlogl
OBJS = parallel.o SchloglStats.o ProblemDefinition.o

all : ${EXE}

.cpp.o:
	$(MPICC) -c ${CPP_OPTIONS} $<

.c.o:
	$(MPICC) -c ${CPP_OPTIONS} $<

p_schlogl: $(OBJS)
	$(MPICC) -o $@ $^ ${LINK_OPTIONS}

clean : 
	rm -f $(EXE) ${OBJS} *.o core* *~
