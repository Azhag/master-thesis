#ifndef CSE_MATH_RANDOM_H
#define CSE_MATH_RANDOM_H
//*****************************************************************************|
//*  FILE:    Random.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: January 15, 2003
//*
//*  LAST MODIFIED: Aug 11, 2004 
//*             BY: Yang Cao
//*             TO: ADD Normal Random Number Generator
//*
//*  SUMMARY:
//*
//*
//*  NOTES:
//*
//*
//*  TO DO:
//*
//*
//*****************************************************************************|
//       1         2         3         4         5         6         7         8
//345678901234567890123456789012345678901234567890123456789012345678901234567890
#include "Vector.h"


namespace CSE {
  namespace Math {

    //void Seeder(time_t curTime);   
    void Seeder(int RanSeed, time_t curTime);

    double UniformRandom();

    double NormalRandom();
   
    double NormalRandom(double mean, double var); 

    double PoissonRandom(double mean);

    Vector PoissonRandom(const Vector& means);


  } // Close CSE::Math namespace
} // Close CSE namespace

#endif // CSE_MATH_RANDOM_H
