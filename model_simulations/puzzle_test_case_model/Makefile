CSE_CPP_HOME = ../stochKit
include ../stochKit/makefile.config

TARGET = PuzzleTestCaseSingle PuzzleTestCaseStats PuzzleTestCaseSingleMultiple

ADDITIONAL_FILES = ProblemDefinition


OBJS = ${TARGET} ${ADDITIONAL_FILES}
SOURCEFILES = $(OBJS:=.cpp)
OBJECTFILES = $(OBJS:=.o)

all:  ${TARGET}

$(OBJS):  $(OBJECTFILES)
	@echo "--- Linking ---"
	${CPP} -o $@ $@.o ${ADDITIONAL_FILES:=.o} ${LINK_OPTIONS} 

.cpp.o:
	@echo "---" $@ "---"
	${CPP} -c -o $@ $< ${CPP_OPTIONS}

clean : 
	rm -f ${OBJS} *.o core* 
