#ifndef StickyObj_H
#define StickyObj_H

/* 

*/


#include "Vector3D.h"
#include <plugins/physics.h>
#include <math.h>
#include <iostream>
#include <fstream>

#define DEBUG 0
#define DEBUG_LIGHT 1
#define ADHESION 1


#define PRINT_STEP 500

#define MAX_CONTACTS 16

#define MAX_ADHESIONPOINTS 1024 // the square root must be an int !!!
#define BOUND 32  					//(int)sqrt(MAX_ADHESIONPOINTS)

#define ADHESION_FORCE_FACTOR 100000// [N/m²] should be 10000 (according to Dan's predictions)

// number of DOFs of the robot
#define MAX_SERVOS 18
#define POSITION_ARRAY_SIZE 7
#define SIMULATION_STEP_DURATION 8


class StickyObj {
 public:
  // constructor: create a sticky Object according to specification
  StickyObj(const char * name);

  // destructor
  ~StickyObj();
   
   void cleanUp();
   
 	dJointFeedback linkJointFeedback;
 	
 	dJointID adhesionJoint; 	// Fixed joint to link the paw and the ground 
	dJointID linkJoint;			// joint that links the paw to any upper part of the body (usually ankle), used to retrieve the forces acting on the paw	
 	int adheringPoints;			// Number of points that are adhering at this moment (simulated points)
 	
 	//output to file
 	ofstream myOutFile_SigmaY;
 	ofstream myOutFile_TauX;
 	ofstream myOutFile_TauZ;
 	
 	
 
  // functions 
  float getSurfaceArea();
  void setNumberOfCollidingPoints(int value);
  int getNumberOfCollidingPoints();
  void setAdhesiveForce();
  
  dGeomID getGeomID();
  dBodyID getBodyID();
  float getLinkJointForce();
  void setCollidingPointPos(int i, Vector3D pos);
  void setNormal(Vector3D vec);
  const char* getName();
  Vector3D getPosition();
  
  
  int getTheState();
  int isAttached();
  void attach();
  void detach();
  void resetAll();
  void elapseDetaching();
  void elapseAttaching();
  
  void printInfos();
  void printName(int attach);
  void computeAndUpdateFeedback();
  void computeContactArea();
  Vector3D computeSheerAndStress(dReal x, dReal z);
  void displayLocalSheerAndStress();
  int testAdhesionEfficiency();
  dReal vonMisesFunction(Vector3D vec);
  dReal absdReal(dReal a);
  
 private:
  const char * name;									// name of the geom [corresponds to the DEF in webots node hierarchy]
  dGeomID geomID; 									// dGeomID of the object
  dBodyID bodyID;										// dBodyID of the object
  float mass;											
  float surfaceArea; 								// contacting Area
 
  int collidingPoints;								// number of colliding points
  Vector3D collidingPointPos[MAX_CONTACTS];	// position of the contacting points in relative coordinates from the center of object
  Vector3D normal;									// normal vector (direction of the ground)
  
  int state; 											// int to define if object attached or not (state : 0 = detached, 1 = attached, 2 = detaching, 3 = attaching)
  int	elapsedDetachingTimer;		 				// timer to allow a detaching time delay where no attachment is possible
  int elapsedAttachingTimer;
  
  Vector3D adhesiveForce;							// Fictive adhesion force of the object !! in Global Coordinates !!
  Vector3D linkJointForce;							// The Force applied on the (ankle)joint, mainly used to retrieve pulling force 
  Vector3D linkJointTorque;						// The Torque applied on the (ankle)joint, mainly used to retrieve pulling torque 
  Vector3D sheerAndStress[BOUND][BOUND];
  
  dReal rho;
};

#endif
