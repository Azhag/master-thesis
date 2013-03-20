#include "ProblemDefinition.h"

// 1: 1 puzzle, 5 robots
// 2: 3 puzzle, 15 robots
// 3: two targets, 1 puzzle, 5 robots
// 4: two targets, 3 puzzles, 5 robots
// #define TEST_CASE_1
// #define TEST_CASE_2
// #define TEST_CASE_3
#define TEST_CASE_4

#ifdef TEST_CASE_1
	#define TEST_CASE 1
	#define SIZE_X 13
	#define NB_REACTIONS 8
#endif
#ifdef TEST_CASE_2
	#define TEST_CASE 2
	#define SIZE_X 13
	#define NB_REACTIONS 8
#endif
#ifdef TEST_CASE_3
	#define TEST_CASE 3
	#define SIZE_X 15
	#define NB_REACTIONS 10
#endif
#ifdef TEST_CASE_4
	#define TEST_CASE 4
	#define SIZE_X 15
	#define NB_REACTIONS 10
#endif




Vector Initialize()
{
	// robot, p{1..4} free, p{1..4} carried, p{5...7} carried, p8 free
	Vector x0(SIZE_X, 0.0);		

	switch (TEST_CASE) {
		case 1:
			// init: 5 robots, p{1,3,4} = 1, p{2} = 2
			x0(0) = 5.;
			x0(1) = 1.;
			x0(2) = 2.;
			x0(3) = 1.;
			x0(4) = 1.;
			
			break;
			
		case 2:
			// init: 15 robots, p{1,3,4} = 3, p{2} = 6
			x0(0) = 15.;
			x0(1) = 3.;
			x0(2) = 6.;
			x0(3) = 3.;
			x0(4) = 3.;
			
			break;
			
		case 3:
			// 2 final puzzles
			// init: 5 robots, p{1,3,4} = 1, p{2} = 2
			x0(0) = 5.;
			x0(1) = 1.;
			x0(2) = 2.;
			x0(3) = 1.;
			x0(4) = 1.;
			
		case 4:
			// 2 final puzzles
			// init: 15 robots, p{1,3,4} = 3, p{2} = 6
			x0(0) = 15.;
			x0(1) = 3.;
			x0(2) = 6.;
			x0(3) = 3.;
			x0(4) = 3.;
			
	}
	
	return x0;
} 

Matrix Stoichiometry()
{
	Matrix nu(SIZE_X, NB_REACTIONS, 0.0);

	// R1: carrying p1
	nu(0,0) = -1.;
	nu(1,0) = -1.;
	nu(5,0) = 1.;

	// R2: carrying p2
	nu(0,1) = -1.0;
	nu(2,1) = -1.0;
	nu(6,1) = 1.0;
	
	// R3: carrying p3
	nu(0,2) = -1.0;
	nu(3,2) = -1.0;
	nu(7,2) = 1.0;
	
	// R4: carrying p4
	nu(0,3) = -1.0;
	nu(4,3) = -1.0;
	nu(8,3) = 1.0;
	
	// R5: assembling p5
	nu(0,4) = 1.0;
	nu(5,4) = -1.0;
	nu(6,4) = -1.0;
	nu(9,4) = 1.0;
	
	// R6: assembling p6
	nu(0,5) = 1.0;
	nu(7,5) = -1.0;
	nu(8,5) = -1.0;
	nu(10,5) = 1.0;
	
	// R7: assembling p7
	nu(0,6) = 1.0;
	nu(9,6) = -1.0;
	nu(10,6) = -1.0;
	nu(11,6) = 1.0;
	
	// R8: assembling pf1
	nu(0,7) = 2.0;
	nu(11,7) = -1.0;
	nu(6,7) = -1.0;
	if (TEST_CASE >= 3) {
		nu(13,7) = 1.0;
	} else {
		nu(12,7) = 1.0;
	}

	
	if(TEST_CASE >= 3) {
		// R9: assembling p8
		nu(0, 8) = 1.0;
		nu(6, 8) = -1.0;
		nu(9, 8) = -1.0;
		nu(12, 8) = 1.0;

		// R10: assembling pf2
		nu(0, 9) = 2.0;
		nu(10, 9) = -1.0;
		nu(12, 9) = -1.0;
		nu(14, 9) = 1.0;
	}
	
	return nu;
}

Vector Propensity(const Vector& x)
{
	Vector a(NB_REACTIONS);
	Vector p_assembly(6);
	
	// double p_encounter = 0.0215;
	// p_encounter = speed*width*timestep/arenaSize;
	double p_encounter = 0.0;
	double p_encounter_robot = 0.0;
	switch(TEST_CASE) {
		case 1:
			p_encounter = 0.010076568613468; // arena size 2
			// p_encounter = 0.011516078415392; // arena size 2
			p_encounter_robot = 0.98*0.017274117623088; // arena size 2
			p_encounter_robot_11 = p_encounter_robot; // arena size 2
			a(0) = p_encounter*x(0)*x(1);
			a(1) = p_encounter*x(0)*x(2);
			a(2) = p_encounter*x(0)*x(3);
			a(3) = p_encounter*x(0)*x(4);

			a(4) = p_encounter_robot*x(5)*x(6);
			a(5) = p_encounter_robot*x(7)*x(8);
			a(6) = p_encounter_robot*x(9)*x(10);
			a(7) = p_encounter_robot_11*x(11)*x(6);
			break;
			
		case 2:
			p_encounter = 0.004398859193826; // arena size 3
			p_encounter_robot = 0.006284084562608; // arena size 3
			p_encounter_robot_11 = p_encounter_robot; // arena size 3
			a(0) = p_encounter*x(0)*x(1);
			a(1) = p_encounter*x(0)*x(2);
			a(2) = p_encounter*x(0)*x(3);
			a(3) = p_encounter*x(0)*x(4);

			a(4) = p_encounter_robot*x(5)*x(6);
			a(5) = p_encounter_robot*x(7)*x(8);
			a(6) = p_encounter_robot*x(9)*x(10);
			a(7) = p_encounter_robot_11*x(11)*x(6);
			break;
		case 3:
			// p_encounter = 0.010076568613468; // arena size 2
			// p_encounter = 0.011516078415392; // arena size 1.85 wp 0.35
			// p_encounter = 0.009853444594170; // arena size 2 wp 0.4
			p_encounter = 0.010365; // arena size 1.95 wp 0.4
			// p_encounter_robot = 0.017274117623088; // arena size 2 arena 1.85, wr = 0.6
			// p_encounter_robot = 0.015012173204414; // arena size 2 arena 1.85, wr=0.55
			// p_encounter_robot = 0.014780166891254; // arena size 2, wr=0.6
			p_encounter_robot = 0.015548; // arena size 1.95, wr=0.6
				
			p_assembly(0) = 0.9778;
			p_assembly(1) = 0.9674;
			p_assembly(2) =  0.8636;
			p_assembly(3) =  0.9737;
			p_assembly(4) =  0.8333;
			p_assembly(5) =  1.0;
			
			a(0) = p_encounter*x(0)*x(1);
			a(1) = p_encounter*x(0)*x(2);
			a(2) = p_encounter*x(0)*x(3);
			a(3) = p_encounter*x(0)*x(4);

			a(4) = p_assembly(0)*p_encounter_robot*x(5)*x(6);
			a(5) = p_assembly(1)*p_encounter_robot*x(7)*x(8);
			a(6) = p_assembly(2)*p_encounter_robot*x(9)*x(10);
			a(7) = p_assembly(3)*p_encounter_robot*x(11)*x(6);
			a(8) = p_assembly(4)*p_encounter_robot*x(6)*x(9);
			a(9) = p_assembly(5)*p_encounter_robot*x(10)*x(12);
		case 4:
			p_encounter = 0.0043793; // arena size 3 wp 0.4
			p_encounter_robot = 0.0065689; // arena size 3, wr=0.6

			p_assembly(0) = 0.9778;
			p_assembly(1) = 0.9674;
			p_assembly(2) =  0.8636;
			p_assembly(3) =  0.9737;
			p_assembly(4) =  0.8333;
			p_assembly(5) =  1.0;

			a(0) = p_encounter*x(0)*x(1);
			a(1) = p_encounter*x(0)*x(2);
			a(2) = p_encounter*x(0)*x(3);
			a(3) = p_encounter*x(0)*x(4);

			a(4) = p_assembly(0)*p_encounter_robot*x(5)*x(6);
			a(5) = p_assembly(1)*p_encounter_robot*x(7)*x(8);
			a(6) = p_assembly(2)*p_encounter_robot*x(9)*x(10);
			a(7) = p_assembly(3)*p_encounter_robot*x(11)*x(6);
			a(8) = p_assembly(4)*p_encounter_robot*x(6)*x(9);
			a(9) = p_assembly(5)*p_encounter_robot*x(10)*x(12);
	}
		
	//	const double c1 = 1, c2 = 10, c3 = 1000, c4 = 0.1; 
	
	
	
	return a;
}

