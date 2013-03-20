#ifndef PROBLEMDEFINITION_H
#define PROBLEMDEFINITION_H
//*****************************************************************************|
//*  FILE:    ProblemDefinition.h
//*
//*  AUTHOR:  Andrew Hall
//*
//*  CREATED: August 6, 2003
//*
//*  LAST MODIFIED:
//*             BY: Hong Li Nov 16, 2004 
//*             TO: make standard models 
//*
//*  SUMMARY:
//*
//*    This file makes it easy to define a whole reaction set in one file
//*    by defining appropriate functions and linking to them.  It also
//*    serves as nice reminder of what is needed for a given problem.
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
#include "Matrix.h"

using namespace CSE::Math;

Vector Initialize();
Matrix Stoichiometry();
Matrix DependentGrapth();
Vector Propensity(const Vector& x);
Vector PartialProp(int RIndex, const Vector& x, const Matrix& dg, Vector& a, double& a0);
Matrix PropensityJacobian(const Vector& x);
void Equilibrium(Vector& x, Vector& a, int rxn);

//Vector InitialState();
//Matrix Stoichiometry();
//void Propensity(Vector& a, const Vector& x);
//void PropensityJacobian(Matrix& j, const Vector& x);

#endif // PROBLEMDEFINITION_H
