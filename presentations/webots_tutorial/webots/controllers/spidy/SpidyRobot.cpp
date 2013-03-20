#include "SpidyRobot.h"

#include <device/robot.h>
#include <device/supervisor.h>
#include <device/servo.h>
#include <device/receiver.h>
#include <device/emitter.h>

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

using namespace std;

/*
 * List of servos
 */
const char * SpidyRobot::SERVO_NAMES[MAX_SERVOS+1] = {
    "RF_BASE_FH",
    "RF_BASE_UD",
    "RF_KNEE_UD",
    "RM_BASE_FH",
    "RM_BASE_UD",
    "RM_KNEE_UD",
    "RH_BASE_FH",
    "RH_BASE_UD",
    "RH_KNEE_UD",
    "LF_BASE_FH",
    "LF_BASE_UD",
    "LF_KNEE_UD",
    "LM_BASE_FH",
    "LM_BASE_UD",
    "LM_KNEE_UD",
    "LH_BASE_FH",
    "LH_BASE_UD",
    "LH_KNEE_UD",
     NULL
  }; 



float SpidyRobot::initial_position_and_orientation[7] = {0, 0.2, 0, 0, 0, 1, 0};



SpidyRobot::SpidyRobot(const char *name){
  _name = name;
  _x = 0.0; 
  _z = 0.0; 
  _distance = 0;
  _controlStep = SIMULATION_STEP_DURATION;
  _stepCount = 0;
  _selectedServoId = 0; 
  _offset = 1; 
  
  int i = 0; 
  for (i = 0; SpidyRobot::SERVO_NAMES[i]; i++) {
	servos[i] = robot_get_device(SERVO_NAMES[i]); 
  	assert(servos[i]);
  	servo_enable_position(servos[i], (unsigned int)_controlStep);
  }
  
  _emitter = robot_get_device("EMITTER");
  _receiver = robot_get_device("RECEIVER");
  
  _noderef = supervisor_node_get_from_def("SPIDY");
  _groundref = supervisor_node_get_from_def("GROUND");
  robot_step((unsigned int)_controlStep);
  supervisor_field_get(_noderef, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, _currentPosition, (unsigned int)_controlStep);
  supervisor_field_get(_groundref, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, groundCurrentPosition, (unsigned int)_controlStep);
  
  
  receiver_enable(_receiver, (unsigned int)_controlStep);
  emitter_set_channel(_emitter,0);
  changeAdhesion = 0;
}

/*
 * Set servo position
 */
void SpidyRobot::setServoPosition(int servoId, float value){
	servo_set_position(servos[servoId], value);
}

/*
 * Get servo position
 */
float SpidyRobot::getServoPosition(int servoId){
	return servo_get_position(servos[servoId]);
}

/*
 * Enable the 'servo_get_position' function for the provided servo
 */
void SpidyRobot::enableServoPosition(int servoId){
	servo_enable_position(servos[servoId], (unsigned int)_controlStep);
}

/*
 * Reset robot position
 */
void SpidyRobot::reset(){
	robot_console_printf("Robot::reset\n"); 
	supervisor_field_set(this->_noderef, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, initial_position_and_orientation);
}

void SpidyRobot::rotateGround(){
	if(rotateGroundLeft || rotateGroundRight){
		if(rotateGroundRight){
			groundCurrentPosition[6]-=0.0002;
		}else{
			groundCurrentPosition[6]+=0.0002;
		}			
		supervisor_field_set(this->_groundref,SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, groundCurrentPosition);		
	}
}


void SpidyRobot::up(){
	robot_console_printf("Robot::up\n");
	int i;
	for(i = 1; i<18;i+=3){
		//NodeRef temp = supervisor_node_get_from_def(SERVO_NAMES[i]);
		//robot_console_printf("test\n");
		//supervisor_field_set(temp, SUPERVISOR_FIELD_ROTATION_X, initial_angle);
		setServoPosition(i,getServoPosition(i) + M_PI/8 );
		
	}
}
/*
 * Run simulation for X seconds
 */
void SpidyRobot::wait(float x){
	//number of iteration
	float num = x / ( _controlStep / 1000);
	for (int i=0; i < num; i++){
		robot_step((unsigned int)_controlStep);
	} 
}

void SpidyRobot::interactiveMode(){
	
	int i=0; 	
	int prev_key = -1, key = -1; 
	bool display_info = false; 
	
	const char *message = "ADHESION_ON";
   emitter_send_packet(_emitter, message, strlen(message) + 1);
	while(true){

		//get keyboard
		prev_key = key; 
		key = robot_keyboard_get_key();

		if(key != prev_key){
			//update var according to 'key' value
			switch (key){
				case ROBOT_KEYBOARD_UP:
					rotateGroundRight = (rotateGroundRight == 0) ? 1 : 0;
					rotateGroundLeft = 0;
					robot_console_printf("RIGHT");					
					break;
					
				case ROBOT_KEYBOARD_DOWN:
					rotateGroundLeft = (rotateGroundLeft == 0) ? 1 : 0;
					rotateGroundRight = 0;
					break;
					 
				case 'Q':
					for(i=1; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) - M_PI/64);
					}
					break;
					 
				case 'W':
					for(i=1; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) + M_PI/64);
					}				
					break;
					
				case 'A':
					for(i=2; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) - M_PI/64);
					}					
					break;	
								
				case 'S':
					for(i=2; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) + M_PI/64);
					}
					break;
					
				case 'Y':
					for(i=0; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) - M_PI/64);
					}					
					break;	
								
				case 'X':
					for(i=0; i< 18; i+=3){
						setServoPosition(i,getServoPosition(i) + M_PI/64);
					}
					break;	
				case 'D':
						setServoPosition(RM_BASE_UD,getServoPosition(RM_BASE_UD) - M_PI/256);
						//setServoPosition(LM_BASE_UD,getServoPosition(LM_BASE_UD) - M_PI/64);										
					break;	
								
				case 'F':
						setServoPosition(RM_BASE_UD,getServoPosition(RM_BASE_UD) + M_PI/256);
						//setServoPosition(LM_BASE_UD,getServoPosition(LM_BASE_UD) +	 M_PI/64);															
						break;		
				case 'B':
						changeAdhesion = 1;
						break;
				default:
					break; 
			}
			
		}
		
		//rotate the ground
		rotateGround();
		
		if(changeAdhesion){
			changeAdhesion = 0;
			emitter_send_packet(_emitter, message, strlen(message) + 1);
		}
		
		//Receive message from physic plugin
		if (receiver_get_queue_length(_receiver) > 0) {
			/* Read current packet's data */
            const char *recBuffer = (const char*)receiver_get_data(_receiver);
            /* print null-terminated message */
            robot_console_printf("Communicating: received \"%s\"\n",recBuffer);       
            /* fetch next packet */
            receiver_next_packet(_receiver);
		}
		
		// display any information 
		
		if (display_info){ 
			//Foot attached
			
			
		} 
		
		
		//display elapsed time
		sprintf(label_buffer, "%02d:%02d", (int) (_stepCount / 60), (int) _stepCount % 60);
    	supervisor_set_label(2, label_buffer, 0.45, 0.01, 0.07, 0x000000);   /* black */
    	
		
		
		
		// simulator step
  		robot_step((unsigned int)_controlStep);
  		_stepCount++;
  		
	}
}

/*
 * Set the robot into walking position
 */
void SpidyRobot::standing(){
	int i;
	for(i=1; i< 18; i+=3){
		//robot_console_printf("i = %d\n", i); 
		setServoPosition(i,getServoPosition(i) - M_PI/32);
		setServoPosition(i+1,getServoPosition(i) - M_PI/32);
	}
	
	wait(1);
	//TO DO
	//Send message start ADHESION
}



/*
 * Make the robot walk with CPG (using euler integration)
 */
// void SpidyRobot::walk(){
// 	
// 	
// 	float om[NB_OSCI];
// 	float r2[NB_OSCI];
// 	float dydt[NB_OSCI*3];
// 	float y[NB_OSCI*3];
// 	
// 	double curr_state[NB_OSCI];
// 	double curr_speed[NB_OSCI];
// 	
// 	while(true){
// 		//Euler params (to stay consistent with the simulation code)
// 		mu = myParam[0]; // amplitude
// 		om_st = myParam[2];
// 		alpha = myParam[3];
// 		DT = (double)time_step/1000.0; 
// 
// 		// Retrieve values
// 		//get_positions();
// 		//get_sensor_values();
// 		//get_accelero_values();
// 
// 		/*	
// 		EULER INTEGRATION
// 		*/
// 		for(int i = 0;i<NB_OSCI;i++){
// 			  om[i] = om_st/(exp(-b*y[2*i+1])+1) + y[8+i]/(exp(b*y[2*i+1])+1);
// 			  r2[i] = y[2*i]*y[2*i] + y[2*i+1]*y[2*i+1];
// 			  
// 			  dydt[2*i] = DT*(alpha*(mu - r2[i])*y[2*i] - om[i]*y[2*i+1]); // correspond to dx
// 			  dydt[2*i+1] = DT*(10.0*alpha*(mu - r2[i])*y[2*i+1] + om[i]*y[2*i]); // correspond to dy
// 		}
// 
// 		//coupling
// 		for(int i = 0;i<NB_OSCI;i++)
// 		 for(int j = 0;j<NB_OSCI;j++)
// 			dydt[2*i+1] += DT*((double)(current_gait[4*i+j])*y[2*j+1]); // coupling modifies dy
// 
// 
// 		//frequency adaptation
// 		for(int i=0;i<NB_OSCI;i++)
// 		 dydt[8+i] = 0.0;
// 		 
// 
// 		//update y[i]
// 		for(int i=0;i<3*NB_OSCI;i++){
// 		y[i] += dydt[i];
// 
// 		}
// 
// 
// 
// 		int i;
// 		for (i=0;i<NB_OSCI;i++)
// 		 {
// 			curr_state[i] = y[2*i];
// 			curr_speed[i] = y[2*i+1];
// 		 }
// 
// 		//Set position of legs !
// 
// 		}//endWhile(true)
// 
// 	}
// 
// 
// 
// }



