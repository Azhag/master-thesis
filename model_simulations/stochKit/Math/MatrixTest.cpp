#include <iostream>
#include <sstream>
#include <exception>
#include "Matrix.h"
#include "Vector.h"
#include "VectorOps.h"
#include "IEEE.h"
#include "Errors.h"


template <class DestType>
DestType fromString(const std::string& strVal)
{
  std::istringstream instr(strVal);
  DestType t;
  instr >> t;

  if (instr.fail()) {
    throw CSE::FormatError();
  }
  else {
    return t;
  }
}


using namespace CSE::Math;

int main(int argc, const char* argv[])
{
  try {
    int rows, cols;
    double initVal;
    
    Vector vec(5);
    vec(0) = 1;
    vec(1) = 2;
    vec(2) = 3;
    vec(3) = -4;
    vec(4) = -23.435e10;

    std::cout << "vec = " << vec << '\n';

    std::cout << "norm(vec, 2) = " << Norm(vec, 2) << '\n';
    std::cout << "norm(vec, 1) = " << Norm(vec, 1) << '\n';
    std::cout << "norm(vec, Inf) = " << Norm(vec, Inf()) << '\n';
    std::cout << "norm(vec, -Inf) = " << Norm(vec, -Inf()) << '\n';


    /*

    if (argc != 4) {
      std::cerr << "Usage: MatrixTest <rows> <cols> <initval>\n";
      std::exit(EXIT_FAILURE);
    }
    else {
      rows = fromString<int>(argv[1]);
      cols = fromString<int>(argv[2]);
      initVal = fromString<double>(argv[3]);
    }

    
    
    CSE::Matrix mat1(rows, cols, initVal);
    CSE::Matrix mat2(rows, cols);
    CSE::Matrix mat3;
    CSE::Matrix mat4(mat1);
    mat4(0,0) = 4.4;
  
    std::cout << "Matrix(" << rows << ", " << cols << ", " << initVal 
              << ") = \n" << mat1 << '\n';
    
    std::cout << "Matrix(" << rows << ", " << cols << ") = \n"
              << mat2 << '\n';
    
    std::cout << "Matrix() = \n" << mat3 << '\n';
    
    std::cout << "mat4(mat1); mat4(0,0) = 4.4; = \n" << mat4 << '\n';
    std::cout << "mat1 = \n" << mat1 << '\n';

    mat3(0,0) = 34.5;

    mat3 = mat1;
    std::cout << "mat3 = mat1 =\n" << mat3 << '\n';

    mat3(3,3) = 56;
    */
  }
  catch (std::exception& ex) {
    std::cerr << "Caught Exception: \n" << ex.what() << '\n';
  }
  return 0;
}
