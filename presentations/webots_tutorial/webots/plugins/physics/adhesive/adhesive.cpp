/*
 * File:         adhesive.cpp
 * Date:         February 18th, 2008
 * Description:  A custom ODE physics implementing adhesion mechanism
 *           
 * Author:       Simon Ruffieux
 *
*/


#include "adhesive.h"


/*
 * This function is called every time ODE is started, this is the place to
 * initialize the variables and to create the objects we want to handle. It is also
 * useful to store the general ODE variables here.
*/

Adhesive::Adhesive(dWorldID w, dSpaceID s, dJointGroupID j){
	
	world =w;
	space = s;
	contact_joint_group = j;

	const char * OBJ_NAMES[MAX_STICKOBJECTS+1]= { 
	 "RF_FOOT",
    "RM_FOOT",
    "RH_FOOT",
    "LF_FOOT",
    "LM_FOOT",
    "LH_FOOT",
    NULL
  };  
	
	const char * infosONOFF[2]= { 
		"OFF",
		"ON"
	}; 	
	
	/*Output the initialisation in the Webots log*/    
	dWebotsConsolePrintf("Adhesive physics plugin's started ! Adhesion is %s ", infosONOFF[ADHESION]);
	if(DEBUG || DEBUG_LIGHT){
		dWebotsConsolePrintf("DEBUG: %s  DEBUG_LIGHT: %s! ", infosONOFF[DEBUG], infosONOFF[DEBUG_LIGHT]);
	}
                         
   /*
     * Here we get all the geoms associated with DEFs in Webots. 
     * A Geom corresponds to the boundingObject node of the object specified
     * by the DEF name.
     * That is why you can retreive here only DEFs of nodes which contains a
     * boundingObject node.
   */
     
	ground = dWebotsGetGeomFromDEF("GROUNDBASE"); //n.b. "DEFNAME" must be the one defined in Webots for the corresponding node of the robot
	wall = dWebotsGetGeomFromDEF("GROUND");

      
   /* Create and initialize all the sticky objects */
   int i = 0; 
   for(i=0; OBJ_NAMES[i]; i++){
   	stickObj[i] = new StickyObj(OBJ_NAMES[i]);
   }
   /*
    * Then using these Geoms we can get the bodies.
    * A Body corresponds to the physics node of the object which contains the
    * boundingObject corresponding to the Geom.
    * In this particular case, we need to use the dSpaceGetGeom(geom, 0) 
    * function as the Geom of the robot is in fact a Space. As all the Geoms
    * of the Space have the same Body, we use the argument 0 to use
    * always the first Geom to get the Body.
    * You retrieve a Space instead of a Geom when there are various shapes in
    * the same boundingObject node. If you no not know wheter the geom is a
    * space or not, you can use the dGeomIsSpace(geom) function.
   */
   
   if(ground){
   	if(DEBUG)dWebotsConsolePrintf("ground geom has been found !! ");
   	groundBody = dGeomGetBody(ground);
   }
   if(wall){
   	if(DEBUG)dWebotsConsolePrintf("wall geom has been found !! ");
   	wallBody = dGeomGetBody(wall);
   }
   
   
   /*Also initialize any other variables here*/
   
   
   counterPrint = 0;
   
   for(int i=0; i<2;i++){
   	oldPos[i].setX(-1);
   	oldPos[i].setY(-1);
   	oldPos[i].setZ(-1);
   }
}

Adhesive::~Adhesive(){
	//cout << "Destroy adhesive " << endl;
	//delete(stickObj[0]);
}

/*
 * This function is called at every step of ODE.
 * We check at each iteration if the sticky object is adhering (attached) and execute the required detachment test for those object  
 *
*/
void Adhesive::step(){
	int state;
	int i;
	for(i=0; i < MAX_STICKOBJECTS; i++ ){
		state = stickObj[i]->getTheState();
		//if object is attached
		if(state == 1){
			stickObj[i]->computeAndUpdateFeedback();
			if(stickObj[i]->testAdhesionEfficiency()){
				//object detached due to pulling forces
				stickObj[i]->printInfos();
			}
		}else if(state == 2){
			stickObj[i]->elapseDetaching();
		}else if(state == 3){
			stickObj[i]->elapseAttaching();		
		}	
	}
	
	if(counterPrint > PRINT_STEP){ 
		dWebotsConsolePrintf("***************************");
		for(i=0; i< MAX_STICKOBJECTS;i++){
			if(stickObj[i]->isAttached() && (DEBUG || DEBUG_LIGHT)){
				stickObj[i]->computeAndUpdateFeedback();
				stickObj[i]->printInfos();
			}
		}
		dWebotsConsolePrintf("###########################\n");						
		counterPrint = 0;
	}
	counterPrint++;
	
}

/*
 * This function is called every time a collision is detected between two
 * Geoms. It allows you to handle the collisions as you want.
 * Here you can either simply detect collisions for informations, disable
 * useless collisions or handle them.
 * This function is called various times for each time step.
 * For a given collision, if you return 1 this means that you have handled the
 * collision yourself and so it will be ignored by Webots. If you return 0,
 * this means that you want Webots to handle it for you.
 * The g1 and g2 parameter may refer only to placeable ODE Geoms (no Space).
 * To be able to identify who these Geoms are, you must compare them with the
 * ones you have stored. If the Geom you have stored is really a Geom too,
 * you can compare them simply using the == operator but if it is a Space, you
 * should use the dSpaceQuery(geom,gX) function.
*/
int Adhesive::collide(dGeomID g1, dGeomID g2){
	Vector3D pos1;
	Vector3D pos2;
	int i;
	int index = -1;
	for(i=0; i < MAX_STICKOBJECTS; i++){
	 	if( (g1 == (stickObj[i]->getGeomID()) && g2 == wall) || (g1 == wall && (stickObj[i]->getGeomID()) == g2)){
			index = i;
			
			// Always put the sticky object as first parameter of the function "handleCollisionSticky()"
			if(stickObj[i]->getGeomID() == g1){
				handleCollisionSticky(i,g2,0);	
   		}else{
				handleCollisionSticky(i,g1,0);
   		}
   		//If handleCollisionSticky, return 1 (means I handled the collision)
			return 1;
		}
		
	}
	
	/* Apply normal collision if the two object colliding are not of interest */
	return 0;	
}

void Adhesive::drawCube(float size, Vector3D pos, Vector3D color){
	float fPlusPosition = size;
	float fMinusPosition = 0-size;
	
	glDisable(GL_LIGHTING);
	glTranslatef(pos.getX(),pos.getY()+0.025,pos.getZ());
		glBegin(GL_QUADS);// start drawin the cube			 
			glColor3f(color.getX(),color.getY(),color.getZ()); // Color with an array of floats
			// Draw Top of the Cube
			glVertex3f( fPlusPosition, fPlusPosition, fMinusPosition);                    
			glVertex3f(fMinusPosition, fPlusPosition, fMinusPosition);                
			glVertex3f(fMinusPosition, fPlusPosition, fPlusPosition);                
			glVertex3f( fPlusPosition, fPlusPosition, fPlusPosition);                    
			// Draw Bottom of the Cube
			glVertex3f( fPlusPosition, fMinusPosition, fPlusPosition);                    
			glVertex3f(fMinusPosition,fMinusPosition, fPlusPosition);                    
			glVertex3f(fMinusPosition,fMinusPosition, fMinusPosition);                    
			glVertex3f( fPlusPosition,fMinusPosition, fMinusPosition);                
			// Draw Front of the Cube
			glVertex3f( fPlusPosition, fPlusPosition, fPlusPosition);                
			glVertex3f(fMinusPosition, fPlusPosition, fPlusPosition);                
			glVertex3f(fMinusPosition,fMinusPosition, fPlusPosition);                
			glVertex3f( fPlusPosition,fMinusPosition, fPlusPosition);        
			// Draw Back of the Cube
			glVertex3f( fPlusPosition,fMinusPosition, fMinusPosition);    
			glVertex3f(fMinusPosition,fMinusPosition, fMinusPosition);                
			glVertex3f(fMinusPosition, fPlusPosition, fMinusPosition);                    
			glVertex3f( fPlusPosition, fPlusPosition, fMinusPosition);                    
			// Draw Left of the Cube
			glVertex3f(fMinusPosition, fPlusPosition, fPlusPosition);                
			glVertex3f(fMinusPosition, fPlusPosition, fMinusPosition);                    
			glVertex3f(fMinusPosition,fMinusPosition, fMinusPosition);            
			glVertex3f(fMinusPosition,fMinusPosition, fPlusPosition);            
			// Draw Right of the Cube
			glVertex3f( fPlusPosition, fPlusPosition, fMinusPosition);            
			glVertex3f( fPlusPosition, fPlusPosition, fPlusPosition);                    
			glVertex3f( fPlusPosition,fMinusPosition, fPlusPosition);                
			glVertex3f( fPlusPosition,fMinusPosition, fMinusPosition);    
		glEnd();// end drawing the cube     
		glTranslatef(-pos.getX(),-pos.getY()-0.025,-pos.getZ());
	glEnable(GL_LIGHTING);
}

void Adhesive::drawLine(Vector3D pos, Vector3D fb){
	glDisable(GL_LIGHTING);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glColor3f(0,1,0);	
		glVertex3f(pos.getX(), pos.getY()+0.025, pos.getZ());
		glVertex3f(pos.getX()+0.01*fb.getX(),pos.getY()+0.025 + 0.01*fb.getY(), pos.getZ() + 0.01*fb.getZ());
	glEnd();
	glEnable(GL_LIGHTING);
}

void Adhesive::draw(){
	int i;
	Vector3D red = Vector3D(1,0,0); 
	Vector3D green = Vector3D(0,1,0);
	Vector3D orange = Vector3D(0.5,0,0);
	Vector3D lightGreen = Vector3D(0,0.5,0);
	
	for(i=0; i<MAX_STICKOBJECTS; i++){
		switch(stickObj[i]->getTheState()){
			// detached
			case 0: 
				drawCube(0.0015, stickObj[i]->getPosition(), red);
			break;
			//attached	
			case 1:
				
				drawCube(0.0015, stickObj[i]->getPosition(), green);
				drawLine(stickObj[i]->getPosition(), Vector3D(stickObj[i]->linkJointFeedback.f1[0],stickObj[i]->linkJointFeedback.f1[1],stickObj[i]->linkJointFeedback.f1[2]));
			break;
			//detaching
			case 2: 
				drawCube(0.0015, stickObj[i]->getPosition(), orange);
			break;
			//attaching	
			case 3:
				drawCube(0.0015, stickObj[i]->getPosition(), lightGreen);
			break;
			default:
			break;
		
		
		}
	}
}
/* 
 * This function is called to override the classical collision handling of Webots.
 * It will handle the collision so as to simulate a sticking mechanism (adhesion)
 *
 * Note that index == sticky material, g2 == object collidede by the sticky material
*/

void Adhesive::handleCollisionSticky(int index, dGeomID g2, Vector3D velocity[2]){

	dContact contact[MAX_CONTACTS];
	dMass dmass;
	
	int i;
	for(i=0; i< MAX_CONTACTS; i++){
		contact[i].surface.mode = dContactBounce | dContactSoftCFM;
		contact[i].surface.mu = 0.0; //dInfinity
		contact[i].surface.mu2 = 0;
		contact[i].surface.bounce = 0.01;
		contact[i].surface.bounce_vel = 1000;
		contact[i].surface.soft_cfm = 0.00001;			
	}

	stickObj[index]->setNumberOfCollidingPoints(dCollide(stickObj[index]->getGeomID(), g2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact)));
	numc = stickObj[index]->getNumberOfCollidingPoints();
	//if(dCollide !=0) ...meaning that contacting parts exists ... 
	if(numc){
		dBodyID b2 = dGeomGetBody(g2);
		Vector3D normalList[MAX_CONTACTS];
		const dReal *pos; 
		pos = dBodyGetPosition(stickObj[index]->getBodyID());	
		Vector3D avNormal; 
		
			
		
		/* For each contact point, create a contact joint  */
		for(i=0;i< numc; i++){			
			
			dJointID c =  dJointCreateContact(world, contact_joint_group,contact + i);
			dJointAttach(c,stickObj[index]->getBodyID(),b2);
		}
		
		/* No adhesion when less than 3 contacting points ! */
		if(numc >= 3){
	
			//Save the contacting point position and their normal				 
			for(i=0;i<numc;i++){
				stickObj[index]->setCollidingPointPos(i,Vector3D(contact[i].geom.pos[0],contact[i].geom.pos[1],contact[i].geom.pos[2])-Vector3D(pos[0],pos[1],pos[2]));	
				normalList[i] = Vector3D(contact[i].geom.normal[0],contact[i].geom.normal[1],contact[i].geom.normal[2]) ;
			}
			
			stickObj[index]->setNormal(computeAverageNormal(numc,normalList).normalise());
			stickObj[index]->computeContactArea();
			stickObj[index]->setAdhesiveForce();
		
			
			/* If contacting for the first time, then apply an adhesive force and set the object "attached" */
			if(ADHESION && (stickObj[index]->getTheState()==0)){
				if(DEBUG){
					stickObj[index]->printName(1);
				}
				/* Anchor the paw to the ground to simulate adhesion with a fixed joint! */				
				stickObj[index]->adhesionJoint = dJointCreateFixed(world,0);				
				dJointAttach(stickObj[index]->adhesionJoint, stickObj[index]->getBodyID(),b2);
				dJointSetFixed(stickObj[index]->adhesionJoint);		
				
				stickObj[index]->adheringPoints = MAX_ADHESIONPOINTS;
				
				stickObj[index]->attach();
				stickObj[index]->computeAndUpdateFeedback();				

				if(DEBUG){
					dWebotsConsolePrintf("Collision detected --> this object is now attached ");
					stickObj[index]->printInfos();	
				}
					//dWebotsConsolePrintf("Getforce: %f %f %f %d", getForce.getX()/numc,getForce.getY()/numc,getForce.getZ()/numc, numc );
					
			}
		//If only 2 contact point or less, then adhesion is off	
		}else{
			
			
			if(stickObj[index]->getTheState() ==3){
				dJointDestroy(stickObj[index]->adhesionJoint);
				stickObj[index]->detach();
				if(DEBUG){
					stickObj[index]->printName(0);
				}
			}			
		}
	}
}


/*
*
* Compute the average normal depending on all contacting points !
*
*/
Vector3D Adhesive::computeAverageNormal(int nbCP, Vector3D tabCP[MAX_CONTACTS]){
	Vector3D result = Vector3D(0,0,0);
	int i;
	for(i=0;i<nbCP; i++){
		result += tabCP[i];
	}
	return result/nbCP;
}

/*
*
* Return Absolute value of a dReal
*
*/
dReal Adhesive::absdReal(dReal a){
	return (a < 0) ? -a : a;
}
 

