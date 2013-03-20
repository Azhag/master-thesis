#include "StochRxn.h"
#include "StoreState.h"
#include "Vector.h"
#include "Matrix.h"
#include "SSA.h"
#include "FixedStepsize.h"
#include "ExplicitTau_SingleStep.h"
#include "ImplicitTau_SingleStep.h"
#include "SolveGE.h"
#include "VectorOps.h"
#include <iostream>

using namespace CSE::Math;
using namespace CSE::StochRxn;


Vector Propensity(const Vector& x);
Matrix PropensityJacobian(const Vector& x);

Vector rates(61, 0.0);


int main()
{
  try {

    // Set up the problem:
    // This is based on Hana's notes about the heat-shock reaction
     
    // Initial state
    Vector x0(28, 0.0);
    
    x0(0) = 0;
    x0(1) = 0;
    x0(2) = 0;
    x0(3) = 0;
    x0(4) = 1;
    x0(5) = 4645669.0;
    x0(6) = 1324;
    x0(7) = 80;
    x0(8) = 16;
    x0(9) = 3413;
    x0(10) = 29;
    x0(11) = 584;
    x0(12) = 1;
    x0(13) = 22;
    x0(14) = 0;
    x0(15) = 171440;
    x0(16) = 9150;
    x0(17) = 2280;
    x0(18) = 6;
    x0(19) = 596;
    x0(20) = 0;
    x0(21) = 13;
    x0(22) = 3;
    x0(23) = 3;
    x0(24) = 7;
    x0(25) = 0;
    x0(26) = 260;
    x0(27) = 0;

    std::cerr << "x0 = " << x0 << '\n';

    const Vector zeros(28, 0.0);

    // Reaction System
    Matrix nu(28, 61);
    Vector rxn(28);

    // Rxn 1:
    rxn = zeros;
    rxn(0) = -1;
    rxn(1) = -1;
    rxn(2) = +1;
    nu.Col(0) = rxn;
    rates(0) = 2.54;

    // Rxn 2:
    nu.Col(1) = -rxn;
    rates(1) = 1.0;

    // Rxn 3:
    rxn = zeros;
    rxn(3) = -1;
    rxn(0) = -1;
    rxn(4) = +1;
    nu.Col(2) = rxn;
    rates(2) = 0.254;

    // Rxn 4:
    nu.Col(3) = -rxn;
    rates(3) = 1;

    // Rxn 5:
    rxn = zeros;
    rxn(0) = -1;
    rxn(5) = -1;
    rxn(6) = +1;
    nu.Col(4) = rxn;
    rates(4) = 0.0254;

    // Rxn 6:
    nu.Col(5) = -rxn;
    rates(5) = 10;
    
    // Rxn 7:
    rxn = zeros;
    rxn(3) = -1;
    rxn(13) = -1;
    rxn(14) = +1;
    nu.Col(6) = rxn;
    rates(6) = 254;

    // Rxn 8:
    nu.Col(7) = -rxn;
    rates(7) = 10000;

    // Rxn 9:
    rxn = zeros;
    rxn(15) = -1;
    rxn(13) = -1;
    rxn(16) = +1;
    nu.Col(8) = rxn;
    rates(8) = 0.000254;

    // Rxn 10:
    nu.Col(9) = -rxn;
    rates(9) = 0.01;

    // Rxn 11:
    rxn = zeros;
    rxn(2) = -1;
    rxn(5) = -1;
    rxn(7) = +1;
    nu.Col(10) = rxn;
    rates(10) = 0.000254;

    // Rxn 12:
    nu.Col(11) = -rxn;
    rates(11) = 1;

    // Rxn 13:
    rxn = zeros;
    rxn(4) = -1;
    rxn(5) = -1;
    rxn(8) = +1;
    nu.Col(12) = rxn;
    rates(12) = 0.000254;

    // Rxn 14:
    nu.Col(13) = -rxn;
    rates(13) = 1;

    // Rxn 15:
    rxn = zeros;
    rxn(2) = -1;
    rxn(9) = -1;
    rxn(11) = 1;
    nu.Col(14) = rxn;
    rates(14) = 2.54;

    // Rxn 16:
    nu.Col(15) = -rxn;
    rates(15) = 1;

    // Rxn 17:
    rxn = zeros;
    rxn(4) = -1;
    rxn(10) = -1;
    rxn(12) = 1;
    nu.Col(16) = rxn;
    rates(16) = 2540;

    // Rxn 18:
    nu.Col(17) = -rxn;
    rates(17) = 1000;

    // Rxn 19:
    rxn = zeros;
    rxn(14) = -1;
    rxn(17) = -1;
    rxn(18) = +1;
    nu.Col(18) = rxn;
    rates(18) = 0.0254;
    
    // Rxn 20:
    nu.Col(19) = -rxn;
    rates(19) = 1;

    // Rxn 21:
    rxn = zeros;
    rxn(21) = +1;
    nu.Col(20) = rxn;
    rates(20) = 6.67;

    // Rxn 22:
    rxn = zeros;
    rxn(21) = -1;
    nu.Col(21) = rxn;
    rates(21) = 0.5;

    // Rxn 23:
    rxn = zeros;
    rxn(13) = +1;
    nu.Col(22) = rxn;
    rates(22) = 20;

    // Rxn 24:
    nu.Col(23) = -rxn;
    rates(23) = 0.03;
    
    // Rxn 25:
    rxn = zeros;
    rxn(16) = -1;
    rxn(15) = +1;
    nu.Col(24) = rxn;
    rates(24) = 0.03;

    // Rxn 26:
    rxn = zeros;
    rxn(14) = -1;
    rxn(3) = +1;
    nu.Col(25) = rxn;
    rates(25) = 0.03;

    // Rxn 27:
    rxn = zeros;
    rxn(18) = -1;
    rxn(3) = +1;
    rxn(17) = +1;
    nu.Col(26) = rxn;
    rates(26) = 0.03;

    // Rxn 28:
    rxn = zeros;
    rxn(27) = -1;
    rxn(3) = +1;
    rxn(26) = +1;
    nu.Col(27) = rxn;
    rates(27) = 0.03;

    // Rxn 29:
    rxn = zeros;
    rxn(22) = +1;
    nu.Col(28) = rxn;
    rates(28) = 1.67;

    // Rxn 30:
    nu.Col(29) = -rxn;
    rates(29) = 0.5;

    // Rxn 31:
    rxn = zeros;
    rxn(17) = +1;
    nu.Col(30) = rxn;
    rates(30) = 20;

    // Rxn 32:
    nu.Col(31) = -rxn;
    rates(31) = 0.03;

    // Rxn 33:
    rxn = zeros;
    rxn(18) = -1;
    rxn(14) = +1;
    nu.Col(32) = rxn;
    rates(32) = 0.03;

    // Rxn 34:
    rxn = zeros;
    rxn(24) = +1;
    nu.Col(33) = rxn;
    rates(33) = 0.00625;

    // Rxn 35:
    nu.Col(34) = -rxn;
    rates(34) = 0.5;

    // Rxn 36:
    rxn = zeros;
    rxn(3) = +1;
    nu.Col(35) = rxn;
    rates(35) = 7;

    // Rxn 37:
    nu.Col(36) = -rxn;
    rates(36) = 0.03;
    
    // Rxn 38:
    rxn = zeros;
    rxn(18) = -1;
    rxn(17) = +1;
    rxn(13) = +1;
    nu.Col(37) = rxn;
    rates(37) = 3;

    // Rxn 39:
    rxn = zeros;
    rxn(20) = -1;
    rxn(19) = 1;
    nu.Col(38) = rxn;
    rates(38) = 0.78;

    // Rxn 40:
    rxn = zeros;
    rxn(27) = -1;
    rxn(13) = +1;
    rxn(26) = +1;
    nu.Col(39) = rxn;
    rates(39) = 0.5;

    // Rxn 41:
    rxn = zeros;
    rxn(23) = +1;
    nu.Col(40) = rxn;
    rates(40) = 1;

    // Rxn 42:
    nu.Col(41) = -rxn;
    rates(41) = 0.5;
    
    // Rxn 43:
    rxn = zeros;
    rxn(19) = +1;
    nu.Col(42) = rxn;
    rates(42) = 20;

    // Rxn 44:
    nu.Col(43) = -rxn;
    rates(43) = 0.03;

    // Rxn 45:
    rxn = zeros;
    rxn(20) = -1;
    rxn(3) = +1;
    nu.Col(44) = rxn;
    rates(44) = 0.03;

    // Rxn 46:
    rxn = zeros;
    rxn(3) = -1;
    rxn(19) = -1;
    rxn(20) = +1;
    nu.Col(45) = rxn;
    rates(45) = 2.54;
    
    // Rxn 47:
    nu.Col(46) = -rxn;
    rates(46) = 10000;

    // Rxn 48:
    rxn = zeros;
    rxn(25) = +1;
    nu.Col(47) = rxn;
    rates(47) = 13.0/30.0;

    // Rxn 49:
    nu.Col(48) = -rxn;
    rates(48) = 0.5;

    // Rxn 50:
    rxn = zeros;
    rxn(26) = +1;
    nu.Col(49) = rxn;
    rates(49) = 20;

    // Rxn 51:
    nu.Col(50) = -rxn;
    rates(50) = 0.03;

    // Rxn 52:
    rxn = zeros;
    rxn(27) = -1;
    rxn(14) = +1;
    nu.Col(51) = rxn;
    rates(51) = 0.03;

    // Rxn 53:
    rxn = zeros;
    rxn(14) = -1;
    rxn(26) = -1;
    rxn(27) = +1;
    nu.Col(52) = rxn;
    rates(52) = 2.54;

    // Rxn 54:
    nu.Col(53) = -rxn;
    rates(53) = 10000;

    // Rxn 55:
    rxn = zeros;
    rxn(4) = -1;
    rxn(0) = +1;
    nu.Col(54) = rxn;
    rates(54) = 0.03;

    // Rxn 56:
    rxn = zeros;
    rxn(12) = -1;
    rxn(0) = +1;
    rxn(10) = +1;
    nu.Col(55) = rxn;
    rates(55) = 0.03;

    // Rxn 57:
    rxn = zeros;
    rxn(8) = -1;
    rxn(6) = +1;
    nu.Col(56) = rxn;
    rates(56) = 0.03;

    // Rxn 58:
    rxn = zeros;
    rxn(14) = -1;
    rxn(13) = +1;
    nu.Col(57) = rxn;
    rates(57) = 0.03;

    // Rxn 59:
    rxn = zeros;
    rxn(18) = -1;
    rxn(13) = +1;
    rxn(17) = +1;
    nu.Col(58) = rxn;
    rates(58) = 0.03;

    // Rxn 60:
    rxn = zeros;
    rxn(27) = -1;
    rxn(13) = +1;
    rxn(26) = +1;
    nu.Col(59) = rxn;
    rates(59) = 0.03;
    
    // Rxn 61:
    rxn = zeros;
    rxn(20) = -1;
    rxn(19) = +1;
    nu.Col(60) = rxn;
    rates(60) = 0.03;

    // Dump reaction matrix
    //std::cerr << "Nu: \n" << nu << '\n';

    ReactionSet rxns(nu, Propensity, PropensityJacobian);

    // Configure solver
    SolverOptions opt;
    opt.stepsize_selector_func = SSADirect_Stepsize; //Fixed_Stepsize; //SSADirect_Stepsize;
    opt.single_step_func = SSA_SingleStep; //ImplicitTau_SingleStep; //SSA_SingleStep;
    opt.store_state_func =  Exponential_StoreState;
    opt.progress_interval = 100000;
    opt.initial_stepsize = 0.001;
    opt.absolute_tol = 1e-6;
    opt.relative_tol = 1e-5;
    Vector a = Propensity(x0);
    std::cout << "a: \n" << a << '\n'; 

    // Make the run
    SolutionHistory sln = StochRxn(x0, 0, 500, rxns, opt);

    std::cerr << "Final State: " << sln.back().State() << '\n';


    // Now, dump the history to a file.
    //
    for (unsigned int i = 0; i < sln.size(); ++i) {
      const SolutionPt& sp = sln[i];
      const Vector& st = sp.State();

      std::cout << sp.Time();
      
      for (int j = 0; j < st.Size(); ++j) {
        std::cout << ' ' << st(j);
      }
      std::cout << '\n';
    }
  }
  catch (const std::exception& ex) {
    std::cerr << "\nCaught " << ex.what() << '\n';
  }
 
  return 0;
}
  


Vector Propensity(const Vector& x)
{
  Vector a = rates;

  a(0) *= x(0) * x(1);
  a(1) *= x(2);
  a(2) *= x(3) * x(0);
  a(3) *= x(4);
  a(4) *= x(0) * x(5);
  a(5) *= x(6);
  a(6) *= x(3) * x(13);
  a(7) *= x(14);
  a(8) *= x(15) * x(13);
  a(9) *= x(16);
  a(10) *= x(2) * x(5);
  a(11) *= x(7);
  a(12) *= x(4) * x(5);
  a(13) *= x(8);
  a(14) *= x(2) * x(9);
  a(15) *= x(11);
  a(16) *= x(4) * x(10);
  a(17) *= x(12);
  a(18) *= x(14) * x(17);
  a(19) *= x(18);
  a(20) *= x(12);
  a(21) *= x(21);
  a(22) *= x(21);
  a(23) *= x(13);
  a(24) *= x(16);
  a(25) *= x(14);
  a(26) *= x(18);
  a(27) *= x(27);
  a(28) *= x(12);
  a(29) *= x(22);
  a(30) *= x(22);
  a(31) *= x(17);
  a(32) *= x(18);
  a(33) *= x(11);
  a(34) *= x(24);
  a(35) *= x(24);
  a(36) *= x(3);
  a(37) *= x(18);
  a(38) *= x(20);
  a(39) *= x(27);
  a(40) *= x(12);
  a(41) *= x(23);
  a(42) *= x(23);
  a(43) *= x(19);
  a(44) *= x(20);
  a(45) *= x(3) * x(19);
  a(46) *= x(20);
  a(47) *= x(12);
  a(48) *= x(25);
  a(49) *= x(25);
  a(50) *= x(26);
  a(51) *= x(27);
  a(52) *= x(14) * x(26);
  a(53) *= x(27);
  a(54) *= x(4);
  a(55) *= x(12);
  a(56) *= x(8);
  a(57) *= x(14);
  a(58) *= x(18);
  a(59) *= x(27);
  a(60) *= x(20);

  return a;
}


Matrix PropensityJacobian(const Vector& x)
{
  Matrix j(61, 28, 0.0);

  // a(0) *= x(0) * x(1);
  j(0,0) = rates(0) * x(1);
  j(0,1) = rates(0) * x(0);
  
  // a(1) *= x(2);
  j(1,2) = rates(1);

  // a(2) *= x(3) * x(0);
  j(2,3) = rates(2) * x(0);
  j(2,0) = rates(2) * x(3);

  // a(3) *= x(4);
  j(3,4) = rates(3);

  // a(4) *= x(0) * x(5);
  j(4,0) = rates(4) * x(5);
  j(4,5) = rates(4) * x(0);

  // a(5) *= x(6);
  j(5,6) = rates(5);

  // a(6) *= x(3) * x(13);
  j(6,3) = rates(6) * x(13);
  j(6,13) = rates(6) * x(3);

  // a(7) *= x(14);
  j(7,14) = rates(7);

  // a(8) *= x(15) * x(13);
  j(8,15) = rates(8) * x(13);
  j(8,13) = rates(8) * x(15);

  // a(9) *= x(16);
  j(9,16) = rates(9);

  // a(10) *= x(2) * x(5);
  j(10,2) = rates(10) * x(5);
  j(10,5) = rates(10) * x(2);

  // a(11) *= x(7);
  j(11,7) = rates(11);

  // a(12) *= x(4) * x(5);
  j(12,4) = rates(12) * x(5);
  j(12,5) = rates(12) * x(4);

  // a(13) *= x(8);
  j(13,8) = rates(13);

  // a(14) *= x(2) * x(9);
  j(14,2) = rates(14) * x(9);
  j(14,9) = rates(14) * x(2);

  // a(15) *= x(11);
  j(15,11) = rates(15);

  // a(16) *= x(4) * x(10);
  j(16,4) = rates(16) * x(10);
  j(16,10) = rates(16) * x(4);

  // a(17) *= x(12);
  j(17,12) = rates(17);

  // a(18) *= x(14) * x(17);
  j(18,14) = rates(18) * x(17);
  j(18,17) = rates(18) * x(14);

  // a(19) *= x(18);
  j(19,18) = rates(19);

  // a(20) *= x(12);
  j(20,12) = rates(20);

  // a(21) *= x(21);
  j(21,21) = rates(21);

  // a(22) *= x(21);
  j(22,21) = rates(22);

  // a(23) *= x(13);
  j(23,13) = rates(23);

  // a(24) *= x(16);
  j(24,16) = rates(24);

  // a(25) *= x(14);
  j(25,14) = rates(25);

  // a(26) *= x(18);
  j(26,18) = rates(26);

  // a(27) *= x(27);
  j(27,27) = rates(27);

  // a(28) *= x(12);
  j(28,12) = rates(28);

  // a(29) *= x(22);
  j(29,22) = rates(29);

  // a(30) *= x(22);
  j(30,22) = rates(30);

  // a(31) *= x(17);
  j(31,17) = rates(31);

  // a(32) *= x(18);
  j(32,18) = rates(32);

  // a(33) *= x(11);
  j(33,11) = rates(33);

  // a(34) *= x(24);
  j(34,24) = rates(34);

  // a(35) *= x(24);
  j(35,24) = rates(35);

  // a(36) *= x(3);
  j(36,3) = rates(36);

  // a(37) *= x(18);
  j(37,18) = rates(37);

  // a(38) *= x(20);
  j(38,20) = rates(38);

  // a(39) *= x(27);
  j(39,27) = rates(39);

  // a(40) *= x(12);
  j(40,12) = rates(40);

  // a(41) *= x(23);
  j(41,23) = rates(41);

  // a(42) *= x(23);
  j(42,23) = rates(42);


  // a(43) *= x(19);
  j(43,19) = rates(43);

  // a(44) *= x(20);
  j(44,20) = rates(44);

  // a(45) *= x(3) * x(19);
  j(45,3) = rates(45) * x(19);
  j(45,19) = rates(45) * x(3);

  // a(46) *= x(20);
  j(46,20) = rates(46);

  // a(47) *= x(12);
  j(47,12) = rates(47); 

  // a(48) *= x(25);
  j(48,25) = rates(48);

  // a(49) *= x(25);
  j(49,25) = rates(49);

  // a(50) *= x(26);
  j(50,26) = rates(50);

  // a(51) *= x(27);
  j(51,27) = rates(51);

  // a(52) *= x(14) * x(26);
  j(52,14) = rates(52) * x(26);
  j(52,26) = rates(52) * x(14);

  // a(53) *= x(27);
  j(53,27) = rates(53);

  // a(54) *= x(4);
  j(54,4) = rates(54);

  // a(55) *= x(12);
  j(55,12) = rates(55);

  // a(56) *= x(8);
  j(56,8) = rates(56);

  // a(57) *= x(14);
  j(57,14) = rates(57);

  // a(58) *= x(18);
  j(58,18) = rates(58);

  // a(59) *= x(27);
  j(59,27) = rates(27);

  // a(60) *= x(20);  
  j(60,20) = rates(20);

  return j;
}
