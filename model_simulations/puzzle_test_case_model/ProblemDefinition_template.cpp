#include "ProblemDefinition.h"

// 1: two targets, 1 puzzle, no robots
// 2: two targets, 3 puzzles, no robots
// 3: two targets, 60 puzzles, no robots
// #define TEST_CASE_1
// #define TEST_CASE_2
// #define TEST_CASE_3

// #define ALPHA 0.01
// #define ALPHA 0.5
// #define ALPHA 0.99

// #ifdef TEST_CASE_1
// 	#define TEST_CASE 1
// 	#define SIZE_X 10
// 	#define NB_REACTIONS 12
// #endif
// #ifdef TEST_CASE_2
// 	#define TEST_CASE 2
// 	#define SIZE_X 10
// 	#define NB_REACTIONS 12
// #endif
// #ifdef TEST_CASE_3
// 	#define TEST_CASE 3
	#define SIZE_X 10
	#define NB_REACTIONS 6
// #endif


Vector Initialize()
{
	Vector x0(SIZE_X, 0.0);		

	// switch (TEST_CASE) {
	// 		case 1:
	// 			// 2 final puzzles
	// 			// init: p{1,3,4} = 1, p{2} = 2
	// 			x0(0) = 1.;
	// 			x0(1) = 2.;
	// 			x0(2) = 1.;
	// 			x0(3) = 1.;
	// 			
	// 			break;
	// 			
	// 		case 2:
	// 			// 2 final puzzles
	// 			// init: p{1,3,4} = 3, p{2} = 6
	// 			x0(0) = 3.;
	// 			x0(1) = 6.;
	// 			x0(2) = 3.;
	// 			x0(3) = 3.;
	// 			
	// 			break;
	// 			
	// 		case 3:
			// 2 final puzzles
			// init: p{1,3,4} = 60, p{2} = 120
			x0(0) = 1.;
			x0(1) = 2.;
			x0(2) = 1.;
			x0(3) = 1.;
			
			// break;
	// }
		x0 = NB_PUZZLES*x0;
			
	return x0;
} 

Matrix Stoichiometry()
{
	Matrix nu(SIZE_X, NB_REACTIONS, 0.0);

	// R1: assembling p5
	nu(0,0) = -1.0;
	nu(1,0) = -1.0;
	nu(4,0) = 1.0;
	
	// R2: assembling p6
	nu(2,1) = -1.0;
	nu(3,1) = -1.0;
	nu(5,1) = 1.0;
	
	// R3: assembling p7
	nu(4,2) = -1.0;
	nu(5,2) = -1.0;
	nu(6,2) = 1.0;
	
	// R4: assembling pf1
	nu(6,3) = -1.0;
	nu(1,3) = -1.0;
	nu(8,3) = 1.0;
	
	// R5: assembling p8
	nu(4, 4) = -1.0;
	nu(1, 4) = -1.0;
	nu(7, 4) = 1.0;

	// R6: assembling pf2
	nu(5, 5) = -1.0;
	nu(7, 5) = -1.0;
	nu(9, 5) = 1.0;
	
	// **** Backward reactions
	// // R7, R1-: disassemble P5
	// nu(0,6) = 1.0;
	// nu(1,6) = 1.0;
	// nu(4,6) = -1.0;
	// 
	// // R8, R2-: disassemble P6
	// nu(2,7) = 1.0;
	// nu(3,7) = 1.0;
	// nu(5,7) = -1.0;
	// 
	// // R9, R3-: disassemble P7
	// nu(4,8) = 1.0;
	// nu(5,8) = 1.0;
	// nu(6,8) = -1.0;
	// 
	// // R10, R4-: disassemble Pf1
	// nu(6,9) = 1.0;
	// nu(1,9) = 1.0;
	// nu(8,9) = -1.0;
	// 
	// // R11, R5-: disassemble P8
	// nu(4, 10) = 1.0;
	// nu(1, 10) = 1.0;
	// nu(7, 10) = -1.0;
	// 
	// // R12, R6-: disassemble Pf2
	// nu(5, 11) = 1.0;
	// nu(7, 11) = 1.0;
	// nu(9, 11) = -1.0;
	
	return nu;
}

Vector Propensity(const Vector& x)
{
	Vector a(NB_REACTIONS);
	Vector p_assembly(6);
	Vector p_optimized_backward(6);
	
	// double p_encounter = 0.0215;
	// p_encounter = speed*width*timestep/arenaSize;
	double p_encounter_robot = 0.0;
	
	// switch(TEST_CASE) {
		// case 1:
		// 		p_encounter_robot = 0.015548; // arena size 1.95, wr=0.6
		// 				
		// 		break;
		// 	case 2:
		// 		// p_encounter_robot = 0.0065689; // arena size 3, wr=0.6
		// 		p_encounter_robot = 0.007540901; // arena size 2.8, wr = 0.6 (model ODE optimized)
		// 
		// 		break;
		// 		
		// 	case 3:
			// p_encounter_robot = 0.0065689; // arena size 3, wr=0.6
			p_encounter_robot = 0.007540901; // arena size 2.8, wr = 0.6 (model ODE optimized)

			// break;
	// }
	
	p_assembly(0) = 0.9778;
	p_assembly(1) = 0.9674;
	p_assembly(2) =  0.8636;
	p_assembly(3) =  0.9737;
	p_assembly(4) =  0.8333;
	p_assembly(5) =  1.0;
	
	// p_optimized_backward(0) = 0.0188522;
	// p_optimized_backward(1) = 0.0075409;
	// p_optimized_backward(2) = 0.0037704;
	// p_optimized_backward(4) = 0.0094261;
	// 
	// if (ALPHA == 0.01) {
	// 	p_optimized_backward(3) = 0.0330741;
	// 	p_optimized_backward(5) = 0.0001336;
	// } else if (ALPHA == 0.5) {
	// 	p_optimized_backward(3) = 0.0006614;
	// 	p_optimized_backward(5) = 0.0002646;
	// } else if (ALPHA == 0.99) {
	// 	p_optimized_backward(3) = 0.0003341;
	// 	p_optimized_backward(5) = 0.0132296;		
	// }
	// 

	// Rates
	a(0) = p_assembly(0)*p_encounter_robot*x(0)*x(1);
	a(1) = p_assembly(1)*p_encounter_robot*x(2)*x(3);
	a(2) = p_assembly(2)*p_encounter_robot*x(4)*x(5);
	a(3) = p_assembly(3)*p_encounter_robot*x(6)*x(1);
	a(4) = p_assembly(4)*p_encounter_robot*x(4)*x(1);
	a(5) = p_assembly(5)*p_encounter_robot*x(5)*x(7);
	// a(6) = p_optimized_backward(0)*x(4);
	// a(7) = p_optimized_backward(1)*x(5);
	// a(8) = p_optimized_backward(2)*x(6);
	// a(9) = p_optimized_backward(3)*x(8);
	// a(10) = p_optimized_backward(4)*x(7);
	// a(11) = p_optimized_backward(5)*x(9);
		
	//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
	
	
	
	return a;
}

