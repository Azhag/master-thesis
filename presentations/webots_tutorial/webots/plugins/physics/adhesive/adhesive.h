
#include <stdlib.h>
#include <plugins/physics.h>
#include "Vector3D.h"
#include "StickyObj.h"


#define MAX_STICKOBJECTS 6
#define MIN_ATTACH_FORCE 0.2 



class Adhesive{
public:
	Adhesive(dWorldID worldID, dSpaceID spaceID,dJointGroupID jointID);
   ~Adhesive();
   
   void step();
   int collide(dGeomID g1, dGeomID g2);
	void draw(); 
	//Definition of the Sticky Objects
	StickyObj *stickObj[MAX_STICKOBJECTS];
	
private:
	//Personal functions
	void handleCollisionSticky(int index, dGeomID g2, Vector3D velocityInst[2]);
	Vector3D computeAverageNormal(int nbCP, Vector3D tabCP[MAX_CONTACTS]);
	dReal absdReal(dReal a);
	void drawCube(float size, Vector3D pos, Vector3D color);
	void drawLine(Vector3D pos, Vector3D fb);
/* The general ODE variables */
	dWorldID world;
	dSpaceID space;
	dJointGroupID contact_joint_group;

/* The Geoms used in the physic plugin */

	dGeomID wall;
	dGeomID ground;

/* The Bodies used in the physic plugin */
	//n.b. The Bodies correspond to the bounding object of the Geom objects

	dBodyID wallBody;
	dBodyID groundBody;
 
/* Other variables */

	//number of contact points
	int numc;
	//List of position of the contact points
	Vector3D contactPointList[MAX_CONTACTS];

	//variables to compute the velocity
	Vector3D oldPos[2];
	Vector3D velocityInst[2];


	//Counter to print values each n steps instead of each step
	int counterPrint;
	int counterForce;

	
	//Just for debug/test matters
	dReal PULLFORCE;

		
};

