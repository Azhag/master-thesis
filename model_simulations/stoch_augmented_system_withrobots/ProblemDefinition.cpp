#include "ProblemDefinition.h"

// 1: two targets, 1 puzzle, no robots
// 2: two targets, 3 puzzles, no robots
// 3: two targets, 60 puzzles, no robots
// #define TEST_CASE_1
// #define TEST_CASE_2
#define TEST_CASE_2

// #define ALPHA 0.01
// #define ALPHA 0.5
//#define ALPHA 0.99
#define ALPHA 0.01

#ifdef TEST_CASE_1
	#define TEST_CASE 1
	#define SIZE_X 19
	#define NB_REACTIONS 20
#endif
#ifdef TEST_CASE_2
	#define TEST_CASE 2
	#define SIZE_X 19
	#define NB_REACTIONS 20
#endif
#ifdef TEST_CASE_3
	#define TEST_CASE 3
	#define SIZE_X 19
	#define NB_REACTIONS 20
#endif


Vector Initialize()
{
	Vector x0(SIZE_X, 0.0);		

	x0(0) = 2;
	x0(1) = 0;
	x0(2) = 3;
	x0(3) = 0;
	x0(4) = 0;
	x0(5) = 0;
	x0(6) = 0;
	x0(7) = 0;
	x0(8) = 0;
	x0(9) = 0;
	x0(10) = 0;
	x0(11) = 0;
	x0(12) = 5;
	x0(13) = 0;
	x0(14) = 3;
	x0(15) = 1;
	x0(16) = 0;
	x0(17) = 0;
	x0(18) = 0
;
	
	
	//x0 = x0*(3);
	// switch (TEST_CASE) {
	// 	
	// 	case 1:
	// 		// 2 final puzzles
	// 		// init: p{1,3,4} = 1, p{2} = 2
	// 		x0 = x0*1;
	// 		// x0(10) = 8.; // robots
	// 		
	// 		// 15 -> 18, free assemblies
	// 		
	// 		break;
	// 		
	// 	case 2:
	// 		// 2 final puzzles
	// 		// init: p{1,3,4} = 3, p{2} = 6
	// 		x0 = x0*3;
	// 		// x0(10) = 8.; // robots
	// 		
	// 		// 15 -> 18, free assemblies
	// 		
	// 		break;
	// 		
	// 	case 3:
	// 		// 2 final puzzles
	// 		// init: p{1,3,4} = 60, p{2} = 120
	// 		x0 = x0*60;
	// 		// x0(10) = 8.; // robots
	// 		
	// 		// 15 -> 18, free assemblies
	// 		
	// 		
	// 		break;
	// }
	
	return x0;
} 

Matrix Stoichiometry()
{
	Matrix nu(SIZE_X, NB_REACTIONS, 0.0);

	// R1: assembling p5
	nu(10,0) = 1.0;
	nu(0,0) = -1.0;
	nu(1,0) = -1.0;
	nu(4,0) = 1.0;
	
	// R2: assembling p6
	nu(10,1) = 1.0;
	nu(2,1) = -1.0;
	nu(3,1) = -1.0;
	nu(5,1) = 1.0;
	
	// R3: assembling p7
	nu(10,2) = 1.0;
	nu(4,2) = -1.0;
	nu(5,2) = -1.0;
	nu(6,2) = 1.0;
	
	// R4: assembling pf1
	nu(10,3) = 1.0;
	nu(6,3) = -1.0;
	nu(1,3) = -1.0;
	nu(8,3) = 1.0;
	
	// R5: assembling p8
	nu(10,4) = 1.0;
	nu(4, 4) = -1.0;
	nu(1, 4) = -1.0;
	nu(7, 4) = 1.0;

	// R6: assembling pf2
	nu(10,5) = 1.0;
	nu(5, 5) = -1.0;
	nu(7, 5) = -1.0;
	nu(9, 5) = 1.0;
	
	// **** Backward reactions, one on ground
	// R7, R1-: disassemble P5
	nu(0,6) = 1.0;
	nu(12,6) = 1.0;
	nu(4,6) = -1.0;
	
	// R8, R2-: disassemble P6
	nu(2,7) = 1.0;
	nu(14,7) = 1.0;
	nu(5,7) = -1.0;
	
	// R9, R3-: disassemble P7
	nu(15,8) = 1.0;
	nu(5,8) = 1.0;
	nu(6,8) = -1.0;
	
	// R10, R4-: disassemble Pf1
	nu(6,9) = 1.0;
	nu(12,9) = 1.0;
	nu(8,9) = -1.0;
	
	// R11, R5-: disassemble P8
	nu(4, 10) = 1.0;
	nu(12, 10) = 1.0;
	nu(7, 10) = -1.0;
	
	// R12, R6-: disassemble Pf2
	nu(5, 11) = 1.0;
	nu(18, 11) = 1.0;
	nu(9, 11) = -1.0;
	
	// *** Backward none on ground
	// // R7, R1-: disassemble P5
	// nu(10,6) = -1.0;
	// nu(0,6) = 1.0;
	// nu(1,6) = 1.0;
	// nu(4,6) = -1.0;
	// 
	// // R8, R2-: disassemble P6
	// nu(10,7) = -1.0;
	// nu(2,7) = 1.0;
	// nu(3,7) = 1.0;
	// nu(5,7) = -1.0;
	// 
	// // R9, R3-: disassemble P7
	// nu(10,8) = -1.0;
	// nu(4,8) = 1.0;
	// nu(5,8) = 1.0;
	// nu(6,8) = -1.0;
	// 
	// // R10, R4-: disassemble Pf1
	// nu(10,9) = -1.0;
	// nu(6,9) = 1.0;
	// nu(1,9) = 1.0;
	// nu(8,9) = -1.0;
	// 
	// // R11, R5-: disassemble P8
	// nu(10,10) = -1.0;
	// nu(4, 10) = 1.0;
	// nu(1, 10) = 1.0;
	// nu(7, 10) = -1.0;
	// 
	// // R12, R6-: disassemble Pf2
	// nu(10,11) = -1.0;
	// nu(5, 11) = 1.0;
	// nu(7, 11) = 1.0;
	// nu(9, 11) = -1.0;
	
	// *****Backward all on ground
	// R7, R1-: disassemble P5
	// nu(11,6) = 1.0;
	// nu(12,6) = 1.0;
	// nu(4,6) = -1.0;
	// 
	// // R8, R2-: disassemble P6
	// nu(13,7) = 1.0;
	// nu(14,7) = 1.0;
	// nu(5,7) = -1.0;
	// 
	// // R9, R3-: disassemble P7
	// nu(15,8) = 1.0;
	// nu(16,8) = 1.0;
	// nu(6,8) = -1.0;
	// 
	// // R10, R4-: disassemble Pf1
	// nu(17,9) = 1.0;
	// nu(12,9) = 1.0;
	// nu(8,9) = -1.0;
	// 
	// // R11, R5-: disassemble P8
	// nu(15, 10) = 1.0;
	// nu(12, 10) = 1.0;
	// nu(7, 10) = -1.0;
	// 
	// // R12, R6-: disassemble Pf2
	// nu(16, 11) = 1.0;
	// nu(18, 11) = 1.0;
	// nu(9, 11) = -1.0;
	
	// Carying stuff
	// R13: carry 1
	nu(10, 12) = -1; // robot
	nu(0, 12) = 1;
	nu(11, 12) = -1; // free piece

	// R14: carry 2
	nu(10, 13) = -1; // robot
	nu(1, 13) = 1;
	nu(12, 13) = -1; // free piece
	
	// R15: carry 3
	nu(10, 14) = -1; // robot
	nu(2, 14) = 1;
	nu(13, 14) = -1; // free piece
	
	// R16: carry 4
	nu(10, 15) = -1; // robot
	nu(3, 15) = 1;
	nu(14, 15) = -1; // free piece
	
	// R17: carry 5
	nu(10, 16) = -1; // robot
	nu(4, 16) = 1;
	nu(15, 16) = -1; // free piece
	
	// R18: carry 6
	nu(10, 17) = -1; // robot
	nu(5, 17) = 1;
	nu(16, 17) = -1; // free piece
	
	// R19: carry 7
	nu(10, 18) = -1; // robot
	nu(6, 18) = 1;
	nu(17, 18) = -1; // free piece	
	
	// R19: carry 7
	nu(10, 19) = -1; // robot
	nu(7, 19) = 1;
	nu(18, 19) = -1; // free piece	
	
	return nu;
}

Vector Propensity(const Vector& x)
{
	Vector a(NB_REACTIONS);
	Vector p_assembly(6);
	Vector p_optimized_backward(6);
	double p_encounter = 0.0;
	
	// double p_encounter = 0.0215;
	// p_encounter = speed*width*timestep/arenaSize;
	double p_encounter_robot = 0.0;
	
	switch(TEST_CASE) {
		// case 1:
		// 			p_encounter = 0.010365; // arena size 1.95 wp 0.4
		// 			p_encounter_robot = 0.015548; // arena size 1.95, wr=0.6
		// 			// p_encounter = 0.01151; // arena size 1.95 wp 0.4
		// 			// p_encounter_robot = 0.020274; // arena size 1.95, wr=0.6
		// 			
		// 			// p_encounter = 0.009853;
		// 			// p_encounter_robot = 0.01478;
		// 				
		// 			break;
		case 2:
			p_encounter = 0.00437930870852; // arena size 3 wp 0.4
			// p_encounter_robot = 0.0065689; // arena size 3, wr=0.6
			p_encounter_robot = 0.00656896306278; // arena size 2.8, wr = 0.6 (model ODE optimized)
	
			break;
			
		// case 3:
		// 		p_encounter = 0.0043793; // arena size 3 wp 0.4
		// 		// p_encounter_robot = 0.0065689; // arena size 3, wr=0.6
		// 		p_encounter_robot = 0.007540901; // arena size 2.8, wr = 0.6 (model ODE optimized)
		// 
		// 		break;
	}
	
	p_assembly(0) = 0.9778;
	p_assembly(1) = 0.9674;
	p_assembly(2) =  0.8636;
	p_assembly(3) =  0.9737;
	p_assembly(4) =  0.8333;
	p_assembly(5) =  1.0;
	
	p_optimized_backward(0) = 0.0188522;
	p_optimized_backward(1) = 0.0075409;
	p_optimized_backward(2) = 0.0037704;
	p_optimized_backward(4) = 0.0094261;

	if (ALPHA == 0.01) {
		p_optimized_backward(3) = 0.0330741;
		p_optimized_backward(5) = 0.0001336;
	} else if (ALPHA == 0.5) {
		p_optimized_backward(3) = 0.0006614;
		p_optimized_backward(5) = 0.0002646;
	} else if (ALPHA == 0.99) {
		p_optimized_backward(3) = 0.0003341;
		p_optimized_backward(5) = 0.0132296;		
	} else if (ALPHA == 0.4) {
		p_optimized_backward(3) = 0.0008268;
		p_optimized_backward(5) = 0.0002204;		
	}

	// Rates
	a(0) = p_assembly(0)*p_encounter_robot*x(0)*x(1);
	a(1) = p_assembly(1)*p_encounter_robot*x(2)*x(3);
	a(2) = p_assembly(2)*p_encounter_robot*x(4)*x(5);
	a(3) = p_assembly(3)*p_encounter_robot*x(6)*x(1);
	a(4) = p_assembly(4)*p_encounter_robot*x(4)*x(1);
	a(5) = p_assembly(5)*p_encounter_robot*x(5)*x(7);

	// Backward none on floor: needs a robot
	// a(6) = p_optimized_backward(0)*x(4)*x(10);
	// a(7) = p_optimized_backward(1)*x(5)*x(10);
	// a(8) = p_optimized_backward(2)*x(6)*x(10);
	// a(9) = p_optimized_backward(3)*x(8)*x(10);
	// a(10) = p_optimized_backward(4)*x(7)*x(10);
	// a(11) = p_optimized_backward(5)*x(9)*x(10);
	
	// Backward one on floor
	a(6) = p_optimized_backward(0)*x(4);
	a(7) = p_optimized_backward(1)*x(5);
	a(8) = p_optimized_backward(2)*x(6);
	a(9) = p_optimized_backward(3)*x(8);
	a(10) = p_optimized_backward(4)*x(7);
	a(11) = p_optimized_backward(5)*x(9);
	
	a(12) = p_encounter*x(10)*x(11);
	a(13) = p_encounter*x(10)*x(12);
	a(14) = p_encounter*x(10)*x(13);
	a(15) = p_encounter*x(10)*x(14);
	a(16) = p_encounter*x(10)*x(15);
	a(17) = p_encounter*x(10)*x(16);
	a(18) = p_encounter*x(10)*x(17);
	a(19) = p_encounter*x(10)*x(18);
		
	//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
	
	return a;
}

