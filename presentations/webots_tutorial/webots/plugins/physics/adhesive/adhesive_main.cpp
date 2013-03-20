// physics.cpp : Defines the entry point for the DLL application.
//
#include "adhesive.h"


Adhesive *adhe_physics = NULL;

//Initialize the resources needed for the simulation
void webots_physics_init(dWorldID world, dSpaceID space ,dJointGroupID joint)
{
    adhe_physics = new Adhesive(world,space,joint);
}

//Called before every simulation steps
void webots_physics_step()
{
    adhe_physics->step();
}

//Called when geometries collides obviously.
int webots_physics_collide(dGeomID id0,dGeomID id1)
{
    return adhe_physics->collide(id0, id1);
}

//Called to clean up any allocation.
void webots_physics_cleanup()
{
	int i;
	for(i=0; i< 1;i++){
			adhe_physics->stickObj[i]->cleanUp();	
	}

    delete adhe_physics;    
    adhe_physics = NULL;
}

//Called to draw additional object
void webots_physics_draw(){
	adhe_physics->draw();
}

//Called before drawing the world
void webots_physics_predraw()
{
}
