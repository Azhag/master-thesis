include makefile.config

CSE_CPP_HOME = .
TEST_DIR = ${SSPACK_DIR}/test

LIBS = LIBCSEMATH LIBCSESTOCHRXN
TESTS = testproblems

all: $(LIBS) $(TESTS)

LIBCSEMATH: 
	cd $(CSE_MATH); make; 

LIBCSESTOCHRXN: 
	cd $(CSE_STOCHRXN); make; 

testproblems: 
	cd $(TEST_DIR); make; 

clean: 
	cd $(CSE_MATH); make clean; 
	cd $(CSE_STOCHRXN); make clean; 
	cd $(TEST_DIR); make clean; 

