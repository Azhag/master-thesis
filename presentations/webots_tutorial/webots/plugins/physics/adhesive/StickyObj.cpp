/*
 * File:         adhesive.cpp
 * Date:         February 18th, 2008
 * Description:  A sticky object, represent a sticky material
 *           
 * Author:       Simon Ruffieux
 *
*/


#include "StickyObj.h"

int sgn(dReal num);


StickyObj::StickyObj(const char * string){
	name = string;
	geomID = dWebotsGetGeomFromDEF(name);
	if(geomID){
   	if(DEBUG)dWebotsConsolePrintf("%s geom has been found !! ", name);
		bodyID = dGeomGetBody(geomID);
		dMass dmass;
		dBodyGetMass(bodyID, &dmass);
		mass = dmass.mass;
   
   }else{
	   dWebotsConsolePrintf("ERROR %s geom has NOT been found !! ERROR ", name);
   }
   
	linkJoint = dBodyGetJoint(bodyID,0);
   dJointSetFeedback(linkJoint, &linkJointFeedback);
     
	adhesiveForce = Vector3D(0.0,0.0,0.0);
	adheringPoints = 0;
	
	surfaceArea = 0.0;
	collidingPoints = 0;
	state = 0;
	elapsedDetachingTimer = 0;	
	elapsedAttachingTimer = 0;
	rho = 1;
}


StickyObj::~StickyObj(){
}

void StickyObj::cleanUp(){
	dJointDestroy(linkJoint);
	dJointDestroy(adhesionJoint);
}

float StickyObj::getSurfaceArea(){
	return surfaceArea;
}

void StickyObj::setNumberOfCollidingPoints(int value){
	this->collidingPoints = value;
}

int StickyObj::getNumberOfCollidingPoints(){
	return this->collidingPoints;
}

void StickyObj::setAdhesiveForce(){
	adhesiveForce = ADHESION_FORCE_FACTOR*surfaceArea*normal*(adheringPoints/MAX_ADHESIONPOINTS);
}

void StickyObj::setCollidingPointPos(int i, Vector3D pos){
	dVector3 temp;
	dBodyVectorFromWorld(bodyID,pos.getX(),pos.getY(),pos.getZ(),temp);
	collidingPointPos[i] = Vector3D(temp[0],temp[1],temp[2]);
}

void StickyObj::setNormal(Vector3D vec){
	normal = -1*vec;
}

dGeomID StickyObj::getGeomID(){
	return this->geomID;
}

dBodyID StickyObj::getBodyID(){
	return this->bodyID;
}
const char* StickyObj::getName(){
	return this->name;
}

Vector3D StickyObj::getPosition(){
	const dReal *pos = dBodyGetPosition(bodyID);
	return Vector3D(pos[0],pos[1],pos[2]);
}

float StickyObj::getLinkJointForce(){
	return (this->linkJointForce.length());
}

int StickyObj::getTheState(){
	return state;
}

void StickyObj::attach(){
	state = 3;
}
void StickyObj::detach(){
	//dJointDestroy(adhesionJoint);
	state = 2;
}

void StickyObj::elapseDetaching(){
	elapsedDetachingTimer++;
	if(elapsedDetachingTimer > 25){ //100 ms elapsed
		state = 0;
		elapsedDetachingTimer = 0;
	}
}

void StickyObj::elapseAttaching(){
	elapsedAttachingTimer++;
	if(elapsedAttachingTimer > 25){ //100 ms elapsed
		state = 1;
		elapsedAttachingTimer = 0;
	}
}

void StickyObj::resetAll(){
	dBodySetForce(this->bodyID,0,0,0);
	dBodySetTorque(this->bodyID,0,0,0);
	dBodySetLinearVel(this->bodyID,0,0,0);
	dBodySetAngularVel(this->bodyID,0,0,0);
}

int StickyObj::isAttached(){
	return (this->state == 1);
}


void StickyObj::printInfos(){
	const char * states[4] = {
  		"detached",
  		"attached",
  		"detaching",
  		"attaching"
  };	
  
	dWebotsConsolePrintf("** Name: %s ** State: %s -- Area: %f  -- AdhePoints: %d, -- rho: %f", name, states[state], surfaceArea,adheringPoints,rho);
	//if(isAttached()){
		//dWebotsConsolePrintf("AdhesionForce: %f  %f  %f ", adhesiveForce.getX(), adhesiveForce.getY(), adhesiveForce.getZ());
		dWebotsConsolePrintf("JointForce: %f %f %f -- JointTorque: %f  %f  %f",linkJointForce.getX(),linkJointForce.getY(),linkJointForce.getZ(),linkJointTorque.getX(),linkJointTorque.getY(),linkJointTorque.getZ());
		
		dWebotsConsolePrintf("XLeft  --> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f\n",sheerAndStress[0][BOUND/2].getX(), sheerAndStress[0][BOUND/2].getY(),sheerAndStress[0][BOUND/2].getZ() ,vonMisesFunction(sheerAndStress[0][BOUND/2]) );
	/*	
		dWebotsConsolePrintf("XMiddle--> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f",sheerAndStress[BOUND/2][BOUND/2].getX(), sheerAndStress[BOUND/2][BOUND/2].getY(),sheerAndStress[BOUND/2][BOUND/2].getZ() ,vonMisesFunction(sheerAndStress[BOUND/2][BOUND/2]) );
		
		dWebotsConsolePrintf("XRight --> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f\n",sheerAndStress[BOUND-1][BOUND/2].getX(), sheerAndStress[BOUND-1][BOUND/2].getY(),sheerAndStress[BOUND-1][BOUND/2].getZ() ,vonMisesFunction(sheerAndStress[BOUND-1][BOUND/2]) );
		//dWebotsConsolePrintf("VonMises XLeft %f XMiddle %f XRight %f",vonMisesFunction(sheerAndStress[1][BOUND/2]), vonMisesFunction(sheerAndStress[BOUND/2][BOUND/2]),vonMisesFunction(sheerAndStress[BOUND-2][BOUND/2]));
	
		dWebotsConsolePrintf("YUp  --> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f",sheerAndStress[BOUND/2][0].getX(), sheerAndStress[BOUND/2][0].getY(),sheerAndStress[BOUND/2][0].getZ() ,vonMisesFunction(sheerAndStress[BOUND/2][0]) );
	
		dWebotsConsolePrintf("XMiddle--> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f",sheerAndStress[BOUND/2][BOUND/2].getX(), sheerAndStress[BOUND/2][BOUND/2].getY(),sheerAndStress[BOUND/2][BOUND/2].getZ() ,vonMisesFunction(sheerAndStress[BOUND/2][BOUND/2]) );
		
		dWebotsConsolePrintf("YDown  --> SigmZ: %f TauX: %f TauY: %f --> VonMises: %f\n",sheerAndStress[BOUND/2][BOUND-1].getX(), sheerAndStress[BOUND/2][BOUND-1].getY(),sheerAndStress[BOUND/2][BOUND-1].getZ() ,vonMisesFunction(sheerAndStress[BOUND/2][BOUND-1]) );
	*/	
	//}
}
void StickyObj::printName(int attach){
	if(attach){
		dWebotsConsolePrintf("Name: %s attached", name);
	}else{
		dWebotsConsolePrintf("Name: %s detached", name);
	}
}


/*
 * This function computes the force and torque applied by the object on the ground with respect to the center of the contacting surface 
 * It tehn updates the values stored in locally in the object
 *
*/
void StickyObj::computeAndUpdateFeedback(){
	dVector3 jointTempForce;	
	dVector3 jointTempTorque;

	dBodyVectorFromWorld(bodyID,linkJointFeedback.f1[0],linkJointFeedback.f1[1],linkJointFeedback.f1[2],jointTempForce);
	dBodyVectorFromWorld(bodyID,linkJointFeedback.t1[0],linkJointFeedback.t1[1],linkJointFeedback.t1[2],jointTempTorque);

	// Update objects vectors
	linkJointForce = Vector3D(jointTempForce[0],jointTempForce[1],jointTempForce[2]);
	linkJointTorque = Vector3D(jointTempTorque[0],jointTempTorque[1],jointTempTorque[2]);	
}

/*
 * This function computes the sheer and stress on the surface of the object at discrete local points taken from 
 * the center of the contacting surface (assuming a rectangular shape !)
 * Return: (sigmaY, tauX, tauZ)
*/
Vector3D StickyObj::computeSheerAndStress(dReal x, dReal z){
	dReal sigmaY, tauX, tauZ; 
	dReal bSquared = pow(2*collidingPointPos[0].getX(),2);
	dReal hSquared = pow(2*collidingPointPos[0].getZ(),2);	 
	
	//sigmaY = Ry/(b*h) + Mx*z/(b*h³/12)+ Mz*x/h*b³/12  
	sigmaY = -linkJointForce.getY()/surfaceArea + linkJointTorque.getX()*z/(surfaceArea*hSquared/12) + linkJointTorque.getZ()*x/(surfaceArea*bSquared/12);  
	
	//tauX = (Rx/(h*b³/6)) * ((h²/4) - y²)
	tauX = (linkJointForce.getX()/(surfaceArea*bSquared/6))*((hSquared/4)-(z*z));
	
	//tauZ = (Rz/(h³*b/6)) * ((b²/4) - x²)
	tauZ = (linkJointForce.getZ()/(surfaceArea*hSquared/6))*((bSquared/4)-(x*x));
	
	return Vector3D(-sigmaY, tauX, tauZ); // -sigmaY because the computation assume Y towards the ground, it is the opposite in Webots
}


/*
 * Test if adhesions points should unattach due to (pulling) forces (vs AdhesionForce)
 * Two different case : if stress pushing or stress pulling 
 *	Update the number of points attached (and by effet de bord, the AdhesionForce)
 * Return 1 if detached , else 0
 */
int StickyObj::testAdhesionEfficiency(){
	//For each adhesionPoint:
	//	if (pull force + stress)*func(sheer) > Fadhesion --> then detach this point 

	dReal lengX = absdReal(2*collidingPointPos[0].getX()); // approximate the width of the conctact surfac with one point (should be done with all points ..)
	dReal lengZ = absdReal(2*collidingPointPos[0].getZ()); // approximate the height of the conctact surfac with one point (should be done with all points ..)
	dReal stepX = lengX/BOUND;
	dReal stepZ = lengZ/BOUND; 
	dReal dispX = -lengX/2+stepX/2;
	dReal dispZ = -lengZ/2+stepZ/2;	
	rho= adheringPoints/MAX_ADHESIONPOINTS;
	dReal sigmaVonMisesThreshold = 200*ADHESION_FORCE_FACTOR*surfaceArea*rho; // = 1000 exp data from Webots
	dReal thetaVonMisesThreshold = 50*ADHESION_FORCE_FACTOR*surfaceArea*rho; // guessed value
	dReal mu = 0.5;
	
	
	int adhePoints = MAX_ADHESIONPOINTS;
	
	dReal max =0.0;
	dReal maxTemp;
	int i;
	int j;
	for(i=0; i< BOUND; i++){
		for(j=0; j<BOUND;j++){
			sheerAndStress[i][j] = computeSheerAndStress(dispX+stepX*i ,dispZ+stepZ*j);
			maxTemp = 0.01*vonMisesFunction(sheerAndStress[i][j]);
			if(sheerAndStress[i][j].getX() > 0){	
				if(maxTemp > sigmaVonMisesThreshold){	
					adhePoints--;
				}
			}else{
				if(maxTemp > mu*-1*sheerAndStress[i][j].getX()*rho + thetaVonMisesThreshold){	
					adhePoints--;
				}
			}
			if(maxTemp > max){
				max = maxTemp;
			}
		}
	}	
	
	adheringPoints = adhePoints;
	if(adheringPoints == 0){
		dWebotsConsolePrintf("Detached due to forces");
		printInfos();
		
		detach();
		dJointDestroy(adhesionJoint);
		return 1;
	}
	return 0;
}


/*
 * Compute the vonMises function for the sheerAndStress values
 * The vonMises function allows to determine (locally) if a object is gonna break;
 * in our case this is gonna be used to detemine if a adhesive bond (= local link) is gonna break
 * vonMises: sigma_v = 1/sqrt(2) * sqrt(2*Sigma_y² + 6*Tau_x² + 6*Tau_z²)
 *
 */
dReal StickyObj::vonMisesFunction(Vector3D vec){
	// TO DO : revert the threshold value to have this one squared !! (REMOVE SQRT)
	//vec corresponds to Sigma_y, Tau_x, Tau_z
	dReal sigY = vec.getX();
	if(vec.getX() < 0){
		sigY =0;
	}
	return (1/sqrt(2))*sqrt(2*sigY*sigY + 6*vec.getY()*vec.getY() + 6*vec.getZ()*vec.getZ());
}




/*
 * Ouput the local sheer and stress in 3 different matlab files 
 *
 */
void StickyObj::displayLocalSheerAndStress(){
	int i;
	int j;
	dReal lengX = absdReal(2*collidingPointPos[0].getX()); // approximate the width of the conctact surfac with one point (should be done with all points ..)
	dReal lengZ = absdReal(2*collidingPointPos[0].getZ()); // approximate the height of the conctact surfac with one point (should be done with all points ..)
	dReal stepX = lengX/BOUND;
	dReal stepZ = lengZ/BOUND; 
	dReal dispX = -lengX/2+stepX/2;
	dReal dispZ = -lengZ/2+stepZ/2;	
	Vector3D temp[BOUND][BOUND];
	
	myOutFile_SigmaY.open("/home/nomis/SFU/data/outSigmaY.m", ios::trunc);
	myOutFile_TauX.open("/home/nomis/SFU/data/outTauX.m", ios::trunc);
	myOutFile_TauZ.open("/home/nomis/SFU/data/outTauZ.m", ios::trunc);	
	//first go along x axis of the surface and then z(corresponds to y in 2d) axis 
	
	for(i=0; i< BOUND; i++){
		for(j=0; j<BOUND;j++){
			temp[i][j] = computeSheerAndStress(dispX+stepX*i ,dispZ+stepZ*j);
	
			myOutFile_SigmaY << temp[i][j].getX() << " ";
			myOutFile_TauX << temp[i][j].getY() << " ";
			myOutFile_TauZ << temp[i][j].getZ() << " ";
			
		}
			
			myOutFile_SigmaY << endl;
			myOutFile_TauX <<  endl;
			myOutFile_TauZ <<  endl;		
	}
	myOutFile_SigmaY.close();
	myOutFile_TauX.close();
	myOutFile_TauZ.close();	
}



/*
 * This function compute the area of a concave polygon with no holes in it. Works with any number of points
 * Used to determine the contacting surface between two solids from the contacting points retrieved by ODE
 *	Inspired from polygon algorithm taken at http://www.efg2.com/Lab/Library/UseNet/2001/0620b.txt
*/
void StickyObj::computeContactArea(){
	Vector3D u,v;
	float result =0;
	int j;
	
	//Special case if area = point or surface
	if(collidingPoints<=2){
		//keep surface = 0 ??
	}else{
		//if area of 3 points or more
		for(j=0; j<= collidingPoints-3; j++){
			//substract vectors
			 u = collidingPointPos[j+1] - collidingPointPos[j];
			 v = collidingPointPos[j+2] - collidingPointPos[j];
			 result += (u.cross(v)).magnitude(); 
		}
		result /= 2;
	}
	surfaceArea = result;
}



int sgn(dReal num) {
	if (num > 0) return(1);
	else if (num < 0) return(-1);
	else return(0);
}

/*
*
* Return Absolute value of a dReal
*
*/
dReal StickyObj::absdReal(dReal a){
	return (a < 0) ? -a : a;
}

