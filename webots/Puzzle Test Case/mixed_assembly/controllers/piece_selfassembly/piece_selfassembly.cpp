/*
* File:         piece_selfassembly.cpp
* Date:         June 2008
* Description:  Controller for pieces, according to some assembly plan. Pieces move and assemble alone
* Author:       Loic Matthey
* Modifications:
*
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <device/robot.h>
#include <device/custom_robot.h>
#include <device/connector.h>
#include <device/emitter.h>
#include <device/receiver.h>
#include <device/gps.h>
#include <vector>
#include <map>


//======== CHOOSE THE EXPERIMENT HERE =============
#define EXP1PUZZLE

#ifdef EXP1PUZZLE
	// The arena radius
	#define ARENA_RADIUS 1.9
	#define SELFASSEMBY false
#endif

// =================================================

#define KEYB_OFF 2

#define TIME_STEP 32

#define UNLOCKED 0
#define LOCKED 1

#define MAX_COUNT_BADLOCK 100
#define MAX_COUNT_UNLOCK 100

// Communication channels and ranges
#define COMMUNICATION_CHANNEL 1
#define COMMUNICATION_RANGE 0.4 // Different from robot on purpose
#define SUPERVISOR_CHANNEL 2
#define SUPERVISOR_RANGE -1

#define LIM_CONNECTORS 10

#define DIVIDER_MOVES 5
#define EPS_X 0.02
#define EPS_Z 0.03
#define WAIT_TIME 200

#define ROBOT 0
#define PIECE 1

#define DELAY_TURN 25
#define INITIAL_SPEED 8
#define MAX_REPULSION_WALL 30000

using namespace std;

// DEBUG
bool debug = true;

char *robot_name;
int piece_num;
int piece_type;
int carrying_robot_num;
int new_configuration_complete;
int new_carrying_robot_num;
int configuration_reaction_occuring = 0;

static DeviceTag commEmitter, commReceiver, gps;
static DeviceTag connectors[LIM_CONNECTORS];

int connector_status[LIM_CONNECTORS];
int connector_presence[LIM_CONNECTORS];
bool unlock[LIM_CONNECTORS];
int count_badlock[LIM_CONNECTORS];
int count_unlock[LIM_CONNECTORS];
int rotate_ctr = 0;
int otherPiece = 0;
int waitCtr = 0;
const float *direction = NULL;

float target_angle = 0.0;

int piece_type_nb_connectors[] = {2, 4, 3, 2};
int max_connector_number = 0;
int nb_moves = DIVIDER_MOVES;
bool autolock = true;
bool assembling_locking = false;

bool robot_alignment = false;

// REACTIONS STUFF
vector<vector<int> > pieces_reactions_connectors; // Current piece -> which connector has to lock during reaction j
map<pair<int, int>, int> configuration_during_reaction; // For the virtual configurations during some reactions
int configuration_complete = 0;
int configuration_other_assembling = 0;
int reaction_occuring = 0;
int piece_type_virtual = 0;

// Assembly Plan
vector<vector<float> > pieces_reactions_angles; // Piece i -> angle of reaction j
vector<vector<bool> > pieces_reactions_locking; // Piece i -> has to lock during reaction j
map<pair<int, int>, int> configurations_to_reaction; // Indirection mapping from configurations to reaction
map<pair<int, int>, bool> configurations_first_kept_carried; // After two configurations assembly, is the first configuration carried ? (if not, then the other one is)
map<pair<int, int>, int> reactions_configurations_change; // Reaction i and configuration j => new configuration k
vector<vector<int> > effect_reactions; // String representing the effects of the reactions on the populations
map<pair<int, int>, int> backward_reactions_configurations_change; // Maps the current configuration to the previous ones.
vector<float> backward_reaction_probabilities; // Stores the desired probabilities of disassembling.

//Random movement stuff
int turn_delay = 0;
float movement_direction[2];
float speed_bias[2] = {INITIAL_SPEED/10000.0, INITIAL_SPEED/10000.0};
int stuck = 0;
float x;
float y;
float x_prec = 0.0;
float y_prec = 0.0;
float dxEmit;
float dzEmit;
bool attractor = true;
bool move = true;
bool approach_piece = false;

// robot states
typedef enum { SEARCH_FOR_PIECE, ALIGN_WITH_PIECE, APPROACH_PIECE, ROTATE_PIECE, SEARCH_FOR_ROBOT, ROTATE_PIECE_ASSEMBLY,
    ALIGN_WITH_ROBOT, WAIT, APPROACH_ROBOT, UPDATE_PIECES_KNOWLEDGE, SEPARATE_ROBOTS } RobotState;
// piece states
typedef enum { FREE, CLAIMED, ROTATING, CARRIED, ROTATING_ASSEMBLY, ASSEMBLING, ASSEMBLING_LOCKED, KNOWLEDGE_UPDATED, NULL_STATE} PieceState;
PieceState state = FREE;

float elapsed_time = 0.0;


/* Handle the keyboard actions */
void keyboard() {
	int key;
	key = robot_keyboard_get_key();

	/*if (key != 0) {
		//robot_console_printf("key: %d\n", key+KEYB_OFF);
	}*/

	if(key == (69-KEYB_OFF)) { // C
		for (int i=0; i< max_connector_number; i++){
			if (!unlock[i] && connector_status[i] == LOCKED){
				unlock[i] = true;
				//robot_console_printf("Piece unlock %d\n", i);
				break;
			}
		}
	} else if(key == (85-KEYB_OFF)) { // S
		// debug = !debug;
		robot_console_printf("state: %d, config: %d, reaction: %d, attractor: %d, robot: %d\n", state, configuration_complete, reaction_occuring, attractor, carrying_robot_num);
	} else if(key == (78-KEYB_OFF)) { // L
		autolock = !autolock;
		//robot_console_printf("Locking: %d", autolock);
	} else if(key == (34-KEYB_OFF)) { // Space
		move = !move;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	}
}

// Send a message on the channel of the supervisor and then go back to Communication channel
void sendMessageToSupervisor(char* message) {
	emitter_set_channel(commEmitter, SUPERVISOR_CHANNEL);
	emitter_set_range(commEmitter, SUPERVISOR_RANGE);
	
	emitter_send_packet(commEmitter, message, strlen(message) + 1);
	
	emitter_set_channel(commEmitter, COMMUNICATION_CHANNEL);
	emitter_set_range(commEmitter, COMMUNICATION_RANGE);
}

void sendStatisticReaction(int reactionOccured, float reaction_time) {
	char message[100];
	
	// Write the message in a Matlab loadable format
	sprintf(message, "%.5f %d ", 0.0, reactionOccured);
	for (vector<int>::iterator it = effect_reactions[reactionOccured-1].begin(); it!=effect_reactions[reactionOccured-1].end(); ++it) {
		sprintf(message, "%s%d ", message, *it);
	}
	
	robot_console_printf("%.5f %s\n", elapsed_time, message);
	
	// Send that to the supervisor to regroup everything.
	sendMessageToSupervisor(message);
}

void parseMessage(char* recMessage, int msgValues[10]) {
	// Parse the message, format:
	// robot: 0 robot_num state piece_type piece_num configuration_complete configuration_other_assembling
	// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
	int i=0;
	
	char* parsedMsg = strtok(recMessage," ");
	
	// Just put everything into a int array, assume the format is known
	while(i< 10 && parsedMsg != NULL) {
		msgValues[i] = atoi(parsedMsg);
		parsedMsg = strtok(NULL, " ");
		i++;
	}
}

int isMsgFromRobotToMe(int msgValues[10]) {
	return (msgValues[0] == ROBOT && msgValues[5] == configuration_complete && (carrying_robot_num == 0 || msgValues[1] == carrying_robot_num));
}


/**
 *	Perform Random like movement pattern:
 *		- Every DELAY_TURN timesteps, change the velocity randomly
 *
 **/
void movement_random()
{
	float theta;
	float repulsion_wall = 0.0;
	// const float arena_limit = 0.6*ARENA_RADIUS*sin(M_PI/3.0);
	const float arena_limit = 0.0;
	float force_x, force_y;
	// float rotation[3];
	
	// Boundary repulsions
	//  => bounded 1/x^2 repulsion, with x = distance to theoretical arena walls (incircle of hexagon, radius specified)
	const float* matrix = gps_get_matrix(gps);
	x = gps_position_x(matrix);
	y = gps_position_z(matrix);
	if (fabs(x) >= arena_limit || fabs(y) >= arena_limit) {
		repulsion_wall = 1./(pow((sqrt(pow(x,2.0)+pow(y,2.0)) - ARENA_RADIUS*sin(M_PI/3.0)),2.0));
		if (repulsion_wall >= MAX_REPULSION_WALL) {
			repulsion_wall = MAX_REPULSION_WALL;
		}
		repulsion_wall /= 5200.0;
		
		// robot_console_printf("%f %f %f\n", x, y, repulsion_wall);
	}
	// robot_console_printf("rep: %f\n", repulsion_wall);
	
	force_x = movement_direction[0]*speed_bias[0] -x*repulsion_wall*(0.75+0.25*(rand()/(RAND_MAX+1.0)));
	force_y = movement_direction[1]*speed_bias[1] - y*repulsion_wall*(0.75+0.25*(rand()/(RAND_MAX+1.0)));
	
	if (turn_delay <= 0) {
		// Turn now
		
		// Make sure we move, when near the boundary		
		do {
			// Should turn randomly (no backward movement)
			theta = (M_PI/3.0)*((2.0*(rand()/(RAND_MAX+1.0)))-1.0); // [-M_PI/2:M_PI/2]

			// Rotate the direction vector		
			float new_direction_x = cos(theta)*movement_direction[0] - sin(theta)*movement_direction[1];
	 		float new_direction_y = cos(theta)*movement_direction[0] + sin(theta)*movement_direction[1];
			movement_direction[0] = new_direction_x;
			movement_direction[1] = new_direction_y;

			// movement_direction[0] = cos(theta);
			// movement_direction[1] = sin(theta);

			// Normalize it
			float norm= sqrt(pow(movement_direction[0],2) + pow(movement_direction[1],2));
			movement_direction[0] /= norm;
			movement_direction[1] /= norm;
			
			force_x = movement_direction[0]*speed_bias[0] -x*repulsion_wall*(0.75+0.25*(rand()/(RAND_MAX+1.0)));
			force_y = movement_direction[1]*speed_bias[1] - y*repulsion_wall*(0.75+0.25*(rand()/(RAND_MAX+1.0)));
			
		}while(fabs(force_x)<= 0.0005 && fabs(force_y) <= 0.0005);
		
		// Restart the counter to tumble
		turn_delay = DELAY_TURN + DELAY_TURN*(rand()/(RAND_MAX+1.0) - 0.5);

	}
	turn_delay--;
	
	// Restrain the maximal force
	while (sqrt(pow(force_x,2.0) + pow(force_y, 2.0)) >= 0.002) {
		force_x *= 0.5;
		force_y *= 0.5;
					
		// robot_console_printf("%f, %f, %f\n", force_x, force_y, sqrt(pow(force_x,2.0) + pow(force_y, 2.0)));
	}
	
	if (approach_piece) {
		// Restrain the force more when approaching another piece
		while (sqrt(pow(force_x,2.0) + pow(force_y, 2.0)) >= 0.0007) {
			force_x *= 0.5;
			force_y *= 0.5;

			// robot_console_printf("%f, %f, %f\n", force_x, force_y, sqrt(pow(force_x,2.0) + pow(force_y, 2.0)));
		}
	}
				
	// Set the force
	// custom_robot_set_abs_force_and_torque(-x*repulsion_wall, 0.0, -y*repulsion_wall, 0.0, 0.0, 0.0);
	custom_robot_set_abs_force_and_torque(force_x, 0.0, force_y, 0.0, 0.0, 0.0);
	// robot_console_printf("%f, %f, %f\n", force_x, force_y, sqrt(pow(force_x,2.0) + pow(force_y, 2.0)));
	
}

static int run(int ms)
{
	int i;
	char sendMessage[30];  // message that robot sends
	int msgValues[10];
	char *recMessage;

	//float next_position[2];

	// Handle keyboard
	keyboard();

	// Connector status
	for (i = 0; i < max_connector_number; i++) {
		connector_presence[i] = connector_get_presence(connectors[i]);
	}
	// Lock automatically
	if (autolock) {
		// 0 should lock automatically (top connector) when a robot is trying to approach it.
		if (state == CLAIMED && !unlock[0] && connector_presence[0] && !connector_status[0]) {
			//robot_console_printf("Lock %d !\n", i);
			connector_lock(connectors[0]);
			connector_status[0] = LOCKED;
		}
		
		//Should lock automatically the good connector
		if (assembling_locking && reaction_occuring > 0 && !unlock[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] && 
				connector_presence[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] && 
				!connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]]) {
			//robot_console_printf("Lock %d !\n", i);
			connector_lock(connectors[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]]);
			connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] = LOCKED;
		}
		
		// Handle unlocking OLD
		for (i = 0; i < max_connector_number; i++) {
			// Unlock requested
			if (unlock[i]) {
				// Unlocking event
				if (connector_status[i]) {
					connector_unlock(connectors[i]);
				}

				// Allow a new locking when out of presence
				if (!connector_presence[i]
				&& connector_status[i] || (count_unlock[i] > MAX_COUNT_UNLOCK)) {
					count_unlock[i] = 0;
					unlock[i]= !unlock[i];
					connector_status[i] = UNLOCKED;
					//robot_console_printf("Unlock %d..\n", i);
				}

				count_unlock[i] ++;
			} else {
				count_unlock[i] = 0;
			}

			// Unlock if bad position
			if ((connector_status[i] == LOCKED && !connector_presence[i] )){
				count_badlock[i] ++;

				if (count_badlock[i] > MAX_COUNT_BADLOCK) {
					count_badlock[i] = 0;
					unlock[i] = true;
					//robot_console_printf("Bad position %d %d\n", i, count_badlock[i]);
				}
			} else {
				count_badlock[i] = 0;
			}	
		}	
	}
	
	
	// Randomly mixed movement
	if (SELFASSEMBY) {
		if (move) {
			movement_random();
		} else {
			custom_robot_set_abs_force_and_torque(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			custom_robot_set_rel_force_and_torque(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			custom_robot_move(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		}
	}	
	
	//===== Message format:
	// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
	// piece: 1 | configuration_complete | configuration_complete | state | carrying_robot_num | attractor | angle
	sprintf(sendMessage, "1 %d %d %d %d %d %.3f", configuration_complete, piece_num, state, carrying_robot_num, attractor, 0.0);
    
	switch(state){

	case FREE:
	
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
		
		approach_piece = false;
		
		
		if (SELFASSEMBY && reaction_occuring != 0 && carrying_robot_num == 0) {
			// Put the two pieces away after an unsuccesfull assembly
			waitCtr += 1;
			if (waitCtr > WAIT_TIME+50) {
				if (debug) {
					robot_console_printf("Finished pushing\n");	
				}
				
				dxEmit = 0.0;
				dzEmit = 0.0;
				
				waitCtr = 0;
				custom_robot_set_rel_force_and_torque(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			} else if (waitCtr > WAIT_TIME) {
		    	configuration_other_assembling = 0;
				reaction_occuring = 0;
				assembling_locking = false;
				
				// robot_console_printf("pushing %f, %f\n", dxEmit, dzEmit);	
		    	
				state = FREE;
				approach_piece = false;
				
				// Let's put those guys away
				// if (attractor) {
					// robot_console_printf("OUUT");
					custom_robot_set_rel_force_and_torque(-dxEmit*0.002, 0.0, -dzEmit*0.002, 0.0, 0.0, 0.0);
				// }
			}
	    }
		
		while (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
			const void *buffer = receiver_get_data(commReceiver);
			recMessage = (char*) buffer;
			
			// Parse the message
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);
			
			
			// If this is sent by a robot to me, in some states, accept
			if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ALIGN_WITH_PIECE){
				// piece is claimed
				// NOTE: '1' corresponds to robot state ALIGN_WITH_PIECE
				//		 '2' corresponds to robot state APPROACH_PIECE
        		waitCtr = 0;
				state = CLAIMED;
				carrying_robot_num = msgValues[1];
				robot_alignment = true;
				
				move = false;
				
				if (debug) {
					robot_console_printf("Claimed by %d\n", carrying_robot_num);
				}
				
				receiver_next_packet(commReceiver);
				break;
			}
			
			// This is a compatible piece alone, try to assemble !
			if(SELFASSEMBY && msgValues[0] == PIECE && msgValues[3] == FREE) {
				
				configuration_other_assembling = msgValues[1];
				
				// What is the current reaction occuring ?
				reaction_occuring = configurations_to_reaction[pair<int, int>(configuration_complete, configuration_other_assembling)];
				
				if (reaction_occuring != 0) {
					// There is a reaction, start it !
					
					waitCtr = 0;
					
					if (debug) {
						robot_console_printf("Piece nearby------ \n");
						robot_console_printf("== %d %d %d\n", configuration_complete, configuration_other_assembling, reaction_occuring);
					}
					
					// time_encoutering = elapsed_time;
					dxEmit = 0.0;
					dzEmit = 0.0;
					
					// Change the virtual configuration of the piece ?
					configuration_reaction_occuring = piece_type;
					if (configuration_during_reaction[pair<int, int>(configuration_complete, reaction_occuring)] != 0) { // Special case
						configuration_reaction_occuring = configuration_during_reaction[pair<int, int>(configuration_complete, reaction_occuring)];
						piece_type_virtual = configuration_during_reaction[pair<int, int>(configuration_complete, reaction_occuring)];
					}

					// Target angle depend on the configuration to attain
					target_angle = pieces_reactions_angles[configuration_reaction_occuring-1][reaction_occuring-1];

					// Should I lock ?
					assembling_locking = pieces_reactions_locking[configuration_reaction_occuring-1][reaction_occuring-1];		

					// New configurations after reaction finished
					new_configuration_complete = reactions_configurations_change[pair<int, int> (reaction_occuring, configuration_complete)];

					// Rotate if needed (only the attached piece could rotate)
					if (debug) {
						robot_console_printf("to Assembling: %d, %f %d %d %d\n", reaction_occuring, target_angle, (int) assembling_locking, pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1], connector_status[0]);
					}

					// Approach the piece we try to assemble to
					approach_piece = true;
					
					state = ASSEMBLING;
					receiver_next_packet(commReceiver);
					break;
					
				}
			}

			receiver_next_packet(commReceiver);
  		}

		break;
		
	case CLAIMED:

		// robotNear = 0;
		// assembling_locking = false;
	
		waitCtr += 1;

      	if (waitCtr > WAIT_TIME) {
        	waitCtr = 0;
        	state = FREE;
			move = true;
			approach_piece = false;
			carrying_robot_num = 0;
			connector_unlock(connectors[0]);
			connector_status[0] = UNLOCKED;

			if (debug) {
				robot_console_printf("to Free timeout\n");
			}
		}
		
		// Send messages for the alignment of the robot
		if (robot_alignment) {
			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);	
		}
		
		if (connector_status[0] == LOCKED) {			
			// The first connector is the one that attaches to the robot arm
        	waitCtr = 0;
			state = ROTATING;
			if (debug) {
				robot_console_printf("Piece locked\n");
			}
		}
		while (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
			const void *buffer = receiver_get_data(commReceiver);
			recMessage = (char*) buffer;
			
			// Parse the message
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);
			
			// Robot has finished aligning and is approaching.
			if (isMsgFromRobotToMe(msgValues) && msgValues[2] == APPROACH_PIECE) {
				robot_alignment = false;
			}
			
			// Robot that was trying to align didn't managed to do it, abort
			if (state != ROTATING && isMsgFromRobotToMe(msgValues) && msgValues[2] == SEARCH_FOR_ROBOT){
        		waitCtr = 0;
				state = FREE;
				carrying_robot_num = 0;
				connector_unlock(connectors[0]);
				connector_status[0] = UNLOCKED;

				if (debug) {
					robot_console_printf("Wrong robot, to Free\n");
				}
								
				receiver_next_packet(commReceiver);
				break;
			}

			receiver_next_packet(commReceiver);
  		}


		break;

	case ROTATING:

		// robot_console_printf("Rotating--\n");
     	// wait until piece has first snapped into place
     	//if (rotate_ctr < 10) {
		//	  rotate_ctr += 1;
   		//  }
		// assembling_locking = false;

     	//j = piece_to_piece_connectors[piece_type-1];
		while (receiver_get_queue_length(commReceiver) > 0) {

     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);

			// Message by my carrying robot
	  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE){
            	// state 3 for robot is ROTATE_PIECE

	  			direction = receiver_get_emitter_direction(commReceiver);
	  			float xEmit = direction[0];
	  			float zEmit = direction[2];
	  			float turnAngle = atan2(xEmit, zEmit); // angle piece has to turn

				//robot_console_printf("message: %s\n", sendMessage);
				if (turnAngle > 0.01 || turnAngle < -0.01) {
					sprintf(sendMessage, "1 %d %d %d %d %.3f", configuration_complete, piece_num, state, carrying_robot_num, turnAngle);
					emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
				}
				
				dzEmit = direction[2]-1;
				if (fabs(dzEmit) <= EPS_Z) {
			    	state = CARRIED;
					
					if (debug) {
						robot_console_printf("piece Carried\n");
					}
					
					receiver_next_packet(commReceiver);
					break;
     			}
     		}

     		receiver_next_packet(commReceiver);
	  	}

	  	if (connector_status[0] == UNLOCKED) {
			carrying_robot_num = 0;
			connector_unlock(connectors[0]);
			connector_status[0] = UNLOCKED;
  			state = FREE;
			approach_piece = false;
	  	}

		break;

   case CARRIED:
		
		while (receiver_get_queue_length(commReceiver) > 0) {
        	const void *buffer = receiver_get_data(commReceiver);
   	  		recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);

			// Message from robot in state ROTATE_PIECE
   	  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE) {
   	  	  		// only send state until robot recognizes that it can search for a robot
          		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
        	}
			
			// Check if we are starting an assembly
			if(isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE_ASSEMBLY) {

				//============== Let's look into the plan for what to do ===
				
				// What is the current reaction occuring ?
				reaction_occuring = msgValues[6];
				
				// Change the virtual configuration of the piece ?
				configuration_reaction_occuring = configuration_during_reaction[pair<int, int>(configuration_complete, reaction_occuring)];
				if (configuration_reaction_occuring == 0) {
					configuration_reaction_occuring = piece_type;
				}
				
				// Target angle depend on the configuration to attain
				target_angle = pieces_reactions_angles[configuration_reaction_occuring-1][reaction_occuring-1];
				
				// Should I lock ?
				assembling_locking = pieces_reactions_locking[configuration_reaction_occuring-1][reaction_occuring-1];		
				
				// Rotate if needed (only the attached piece could rotate)
				if (connector_status[0]) {
					
					if (debug) {
						robot_console_printf("to Rotating Assembling: %d, %d %f %d %d %d\n", reaction_occuring, configuration_reaction_occuring, target_angle, (int) assembling_locking, pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1], connector_status[0]);
					}
					
					state = ROTATING_ASSEMBLY;
				} else {
					if (debug) {
						robot_console_printf("to Assembling: %d, %f %d %d %d\n", reaction_occuring, target_angle, (int) assembling_locking, pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1], connector_status[0]);
					}
					
					state = ASSEMBLING;
				}
				
			}
			

			receiver_next_packet(commReceiver);
	  	}

		break;
		
	case ROTATING_ASSEMBLY:
		
		while (receiver_get_queue_length(commReceiver) > 0) {
     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);
			
			// robot_console_p rintf("PIECE : ROTATING_ASSEMBLY\n");
			// Message by my carrying robot
	  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE_ASSEMBLY){
		
	  			direction = receiver_get_emitter_direction(commReceiver);
	  			float xEmit = direction[0];
	  			float zEmit = direction[2];
	  			float currentAngle = atan2(xEmit, zEmit);

				float turnAngle = currentAngle - target_angle;	
				float turnAngle2 = target_angle - currentAngle;
				float turnAngle3 = currentAngle - target_angle + 2.*M_PI;
				float turnAngle4 = currentAngle - target_angle - 2.*M_PI;
				
				if (fabs(turnAngle2) < fabs(turnAngle)) {
					turnAngle = turnAngle2;
				} else if(fabs(turnAngle3) < fabs(turnAngle)) {
					turnAngle = turnAngle3;
				} else if(fabs(turnAngle4) < fabs(turnAngle)) {
					turnAngle = turnAngle4;
				} 

				//robot_console_printf("message: %s\n", sendMessage);
				if (turnAngle > 0.01 || turnAngle < -0.01) {
					robot_console_printf("Should rotate of : %f %f %f %f\n", turnAngle, turnAngle2, turnAngle3, turnAngle4);
					
					sprintf(sendMessage, "1 %d %d %d %d %.3f", configuration_complete, piece_num, state, carrying_robot_num, turnAngle);
					emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
				} else {
			    	state = ASSEMBLING;
			
					if (debug) {
						robot_console_printf("To Assembling\n");
					}
					
					receiver_next_packet(commReceiver);
					break;
     			}
				// robot_console_printf("dzEmit err %f %f\n", dzEmit, EPS_Z);
				
     		}

     		receiver_next_packet(commReceiver);
	  	}
		break;
	
	case ASSEMBLING:
	
		waitCtr += 1;
		if (attractor && carrying_robot_num == 0) {
			if (waitCtr > WAIT_TIME) {
		    	state = FREE;
				
				if (debug) {
					robot_console_printf("ASSEMBLING: Attractor abort !\n");
				}
				
				break;
			}
	    }
		
		// When approaching, send heartbeat
		if (approach_piece) {
			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
		}
		
		// If we have to lock, check the status of the connector to lock
		if (assembling_locking && connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] == LOCKED) {
			// robot_console_printf("Locked\n");
			state = ASSEMBLING_LOCKED;
				
			approach_piece = false;
			custom_robot_set_rel_force_and_torque(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			
			if (debug) {
				robot_console_printf("To Assembling Locked\n");
			}
			
			break;
		}
		
		
		while (receiver_get_queue_length(commReceiver) > 0) {
	    	const void *buffer = receiver_get_data(commReceiver);
			recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);

			// Piece self-assembly or not ?
			if (approach_piece) {
				if (SELFASSEMBY) {
					// Approach our target piece
			  		if (msgValues[0] == PIECE && msgValues[1] == configuration_other_assembling && msgValues[3] == ASSEMBLING){
						// waitCtr = 0;

						// robot_console_printf("Direction: %f %f, Position: %f %f\n", dxEmit, dzEmit, x, y);
						if (attractor) {
							direction = receiver_get_emitter_direction(commReceiver);
							dxEmit = direction[0]*(0.5+1.0*(rand()/(RAND_MAX+1.0)));
					    	dzEmit = direction[2]*(0.5+1.0*(rand()/(RAND_MAX+1.0)));

							float norm = sqrt(pow(dxEmit, 2.0)+pow(dzEmit, 2.0));
							dxEmit /= norm;
							dzEmit /= norm;

							// const float* matrix = gps_get_matrix(gps);
							// x = gps_position_x(matrix);
							// y = gps_position_z(matrix);

							custom_robot_set_rel_force_and_torque(dxEmit*0.0015, 0.0, dzEmit*0.0015, 0.0, 0.0, 0.0);						
						}
		     		}
	
					// Message by other piece in the same product (our attractor), which had aborted, let's abort ourself
					if (!attractor && msgValues[0] == PIECE && msgValues[1] == configuration_complete && msgValues[5] == 1 && msgValues[3]==FREE) {

						configuration_other_assembling = 0;
						reaction_occuring = 0;
						assembling_locking = false;
						approach_piece = false;

				    	state = FREE;


						if (debug) {
							robot_console_printf("ASSEMBLING: other piece, ABORT \n");
						}

						receiver_next_packet(commReceiver);
						break;
					}
				
					// Confirmation of another piece in the same configuration that the reaction is finished.
					if (msgValues[0] == PIECE && ((msgValues[1] == configuration_complete && msgValues[3] == KNOWLEDGE_UPDATED)
					|| (msgValues[1] == new_configuration_complete && msgValues[3] == FREE))) {
						// The other piece is locked, so am I, let's update the knowledge of everybody.
						waitCtr = 0;

						if (configuration_complete > configuration_other_assembling) {
							// The smallest assembly ID attracts other pieces the random movements
							attractor = false;
						}

						configuration_complete = new_configuration_complete;

						state = FREE;

						while (receiver_get_queue_length(commReceiver) > 0) {
					    	// flush the queue
					    	receiver_next_packet(commReceiver);
						}

						if (debug) {
							robot_console_printf("-> no locking piece, Assembly finished, %d, %d\n", configuration_complete, carrying_robot_num);
						}
					
					}
				}
			} else {
				// We are carried by a robot
				
				// Carrying robot still waiting on alignment
		  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE_ASSEMBLY){
		  			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
	     		}

				// If we don't lock, just wait for a message by the carrying robot
				if (!assembling_locking) {
					// Message by my carrying robot
			  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == UPDATE_PIECES_KNOWLEDGE){
						// Update configuration_complete
						new_configuration_complete = msgValues[7];

						// Update carrying robot
						new_carrying_robot_num = msgValues[8];

						state = KNOWLEDGE_UPDATED;

						if (debug) {
							robot_console_printf("waiting piece knowledge updated\n");
						}
		     		}
				}
			}
			
	    	receiver_next_packet(commReceiver);
		}
		
		break;
		
	case ASSEMBLING_LOCKED:
		// Only used by the piece that should lock the other configuration
		if (SELFASSEMBY && attractor && carrying_robot_num == 0) {
			waitCtr += 1;
			if (waitCtr > WAIT_TIME) {
		    	configuration_other_assembling = 0;
				// reaction_occuring = 0;
				assembling_locking = false;

				state = FREE;
				approach_piece = false;

				if (debug) {
					robot_console_printf("ASSEMBLING LOCKED: Attractor abort !\n");
				}
				break;
			}
		}
		
		// Tell our robot that the connection is finished
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

		while (receiver_get_queue_length(commReceiver) > 0) {
     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// robot_console_printf("Robot message: %s\n", recMessage);
			
			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | configuration_complete | piece_num | state | carrying_robot_num | attractor | angle
			parseMessage(recMessage, msgValues);

			// Message by my carrying robot
	  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == UPDATE_PIECES_KNOWLEDGE){
				// Update configuration_complete
				new_configuration_complete = msgValues[7];
				
				// Update carrying robot
				new_carrying_robot_num = msgValues[8];
				
				state = KNOWLEDGE_UPDATED;
				
				if (debug) {
					robot_console_printf("Locked piece knowledge updated\n");
				}
				
				receiver_next_packet(commReceiver);
				break;
     		}

			// Message by other alone piece we try to lock
			if (SELFASSEMBY && msgValues[0] == PIECE && msgValues[1] == configuration_other_assembling && msgValues[3] == ASSEMBLING_LOCKED) {
				// The other piece is locked, so am I, let's update the knowledge of everybody.
				waitCtr = 0;
				
				// New configurations after reaction finished
				new_configuration_complete = reactions_configurations_change[pair<int, int> (reaction_occuring, configuration_complete)];
				
				state = KNOWLEDGE_UPDATED;
				
				if (debug) {
					robot_console_printf("Locking piece, to Knowledge updated \n");
				}
			}
			
			// Message by other piece in the same product (our attractor), which had aborted, let's abort ourself
			if (SELFASSEMBY && !attractor && msgValues[0] == PIECE && msgValues[1] == configuration_complete && msgValues[5] == 1 && msgValues[3]==FREE) {

				configuration_other_assembling = 0;
				reaction_occuring = 0;
				assembling_locking = false;
				approach_piece = false;

				state = FREE;


				if (debug) {
					robot_console_printf("ASSEMBLING LOCKED: other piece, ABORT \n");
				}

				receiver_next_packet(commReceiver);
				break;
			}
			
     		receiver_next_packet(commReceiver);
	  	}
	
		break;
	
	case KNOWLEDGE_UPDATED:
		// Tell our robot that the knowledge is updated
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

		assembling_locking = false;
		
		if (SELFASSEMBY && connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] != LOCKED) {
				
			connector_unlock(connectors[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]]);
			connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] = UNLOCKED;
  			
			state = FREE;
			configuration_other_assembling = 0;
			reaction_occuring = 0;
			assembling_locking = false;
			approach_piece = false;
			
			robot_console_printf("KNOWLEDGE_UPDATED: Abort bad lock !\n");
			
			while (receiver_get_queue_length(commReceiver) > 0) {
		    	// flush the queue
		    	receiver_next_packet(commReceiver);
			}
			break;
	  	}
		
		while (receiver_get_queue_length(commReceiver) > 0) {
     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
			parseMessage(recMessage, msgValues);

			// robot_console_printf("Robot message: %s\n", recMessage);
			// robot_console_printf("Ass, %d, %d, %d, %d\n", configuration_complete, carrying_robot_num, new_configuration_complete, msgValues[1]);


			// Message by my carrying robot
	  		if (msgValues[0] == ROBOT && msgValues[5] == new_configuration_complete && msgValues[1] == new_carrying_robot_num && msgValues[2] == SEPARATE_ROBOTS) {
				// Update configuration_complete
				configuration_complete = new_configuration_complete;
				
				// Update carrying robot
				carrying_robot_num = new_carrying_robot_num;
				
				if (debug) {
					robot_console_printf("Assembly finished, %d, %d\n", configuration_complete, carrying_robot_num);
				}
				
				state = CARRIED;
				
				receiver_next_packet(commReceiver);
				break;
     		}

			if (SELFASSEMBY && msgValues[0] == PIECE && msgValues[1] == configuration_other_assembling && msgValues[3] == KNOWLEDGE_UPDATED) {
				// Ok, let's updated the knowledge
				
				if (configuration_complete > configuration_other_assembling) {
					// The smallest assembly ID controls the random movements
					attractor = false;
				} else {
					// Smallest assembly ID sends a confirmation to the supervisor
					sendStatisticReaction(reaction_occuring+4, elapsed_time);
				}
				
				configuration_complete = new_configuration_complete;
				
				state = FREE;
				
				while (receiver_get_queue_length(commReceiver) > 0) {
			    	// flush the queue
			    	receiver_next_packet(commReceiver);
				}
				
				if (debug) {
					robot_console_printf("-> Assembly finished, %d, %d\n", configuration_complete, carrying_robot_num);
				}
				
				break;
			}

     		receiver_next_packet(commReceiver);
	  	}
		break;
		
	   case NULL_STATE:
	   		break;

   }

    elapsed_time += (float)TIME_STEP / 1000.0;
	return TIME_STEP;           /* run for TIME_STEP ms */
}


// ================================================================================================================


void populateAssemblyPlan() {
	const int nb_reactions = 10;
	const int nb_pieces = 5;
	const int nb_species = 14;
		
	// 2 targets
	float angles[nb_pieces][nb_reactions] = {{0.0, 0.0, 0.0, 0.0, 0.5, 1.2}, 
											 {0.0, 0.0, 0.0, 0.0, -2.5, 0.0}, 
											 {0.0, 0.0, 3.14, -1.57, 0.0, 3.0}, 
											 {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 
											 {0.0, 0.0, 0.0, 3.14, 0.0, 0.0}};
	bool locks[nb_pieces][nb_reactions] =  {{true, false, false, false, false, false},
											{true, false, true, true, true, true},
											{false, true, true, false, false, true},
											{false, true, false, false, false, false},
											{false, false, false, true, false, false}};
	int connectors[nb_pieces][nb_reactions] = {{1, 0, 0, 0, 0, 0},
											   {1, 0, 3, 2, 3, 2},
											   {0, 1, 2, 0, 0, 2},
											   {0, 1, 0, 0, 0, 0},
											   {0, 0, 0, 3, 0, 0}};
	
	
	for(int i = 0; i < nb_pieces; ++i) {
		pieces_reactions_angles.push_back(vector<float> (angles[i], angles[i] + sizeof(angles[i]) / sizeof(float)));
		pieces_reactions_locking.push_back(vector<bool> (locks[i], locks[i] + sizeof(locks[i]) / sizeof(bool)));
		pieces_reactions_connectors.push_back(vector<int> (connectors[i], connectors[i] + sizeof(connectors[i]) / sizeof(int)));
	}
	
	configuration_during_reaction[pair<int, int> (2,4)] = 5; // Configuration 2, reaction 4 => Configuration 5
	
	// for(int i = 0; i < nb_reactions; ++i) {
	// 		robot_console_printf("%f ", piece_reactions_angles[i]);
	// 		robot_console_printf("%d ", (int)piece_reactions_locking[i]);
	// 	}
	
	
	// 2 targets
	
	// Effects of reactions.
	//  - Taken of pieces (4 reactions)
	//	- Assembly (6 reactions)
	// format: {free piece x4} {carried piece x4} {product x6}
	int eff_reac[nb_reactions][nb_species] = {{-1, 0, 0, 0,  1, 0, 0, 0,   0, 0, 0, 0,   0, 0 },
								  { 0,-1, 0, 0,  0, 1, 0, 0,   0, 0, 0, 0,   0, 0 },
								  { 0, 0,-1, 0,  0, 0, 1, 0,   0, 0, 0, 0,   0, 0 },
								  { 0, 0, 0,-1,  0, 0, 0, 1,   0, 0, 0, 0,   0, 0 },
								
								  {0, 0, 0, 0,  -1,-1, 0, 0,   1, 0, 0, 0,   0, 0 },
								  {0, 0, 0, 0,   0, 0,-1,-1,   0, 1, 0, 0,   0, 0 },
								  {0, 0, 0, 0,   0, 0, 0, 0,  -1,-1, 1, 0,   0, 0 },
								  {0, 0, 0, 0,   0,-1, 0, 0,   0, 0,-1, 1,   0, 0 },
								
								  {0, 0, 0, 0,   0,-1, 0, 0,  -1, 0, 0, 0,   1, 0},
								  {0, 0, 0, 0,   0, 0, 0, 0,   0,-1, 0, 0,  -1, 1}
	};
	for(int i = 0; i < nb_reactions; ++i) {
		effect_reactions.push_back(vector<int> (eff_reac[i], eff_reac[i] + sizeof(eff_reac[i]) / sizeof(int)));
	}
	
	configurations_to_reaction[pair<int, int> (1,2)] = 1;
	configurations_to_reaction[pair<int, int> (2,1)] = 1;
	configurations_to_reaction[pair<int, int> (3,4)] = 2;
	configurations_to_reaction[pair<int, int> (4,3)] = 2;
	configurations_to_reaction[pair<int, int> (5,6)] = 3;
	configurations_to_reaction[pair<int, int> (6,5)] = 3;
	configurations_to_reaction[pair<int, int> (2,7)] = 4;
	configurations_to_reaction[pair<int, int> (7,2)] = 4;
	configurations_to_reaction[pair<int, int> (5,2)] = 5;
	configurations_to_reaction[pair<int, int> (2,5)] = 5;
	configurations_to_reaction[pair<int, int> (9,6)] = 6;
	configurations_to_reaction[pair<int, int> (6,9)] = 6;
	
	
	configurations_first_kept_carried[pair<int, int> (1,2)] = true; // First piece carried on
	configurations_first_kept_carried[pair<int, int> (2,1)] = false; // First piece carried on
	configurations_first_kept_carried[pair<int, int> (3,4)] = true; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (4,3)] = false; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (5,6)] = false; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (6,5)] = true; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (7,2)] = false; // No piece carried on (stay on floor)
	configurations_first_kept_carried[pair<int, int> (2,7)] = false; // No piece carried on (stay on floor)
	configurations_first_kept_carried[pair<int, int> (2,5)] = false; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (5,2)] = true; // Second piece carried on
	configurations_first_kept_carried[pair<int, int> (9,6)] = false; // No piece carried on (stay on floor)
	configurations_first_kept_carried[pair<int, int> (6,9)] = false; // No piece carried on (stay on floor)
	
	// Rules of modifications of configurations
	reactions_configurations_change[pair<int, int> (1, 1)] = 5; 	// First is reaction number, second is current configuration
	reactions_configurations_change[pair<int, int> (1, 2)] = 5;
	
	reactions_configurations_change[pair<int, int> (2, 3)] = 6;
	reactions_configurations_change[pair<int, int> (2, 4)] = 6;
	
	reactions_configurations_change[pair<int, int> (3, 5)] = 7;
	reactions_configurations_change[pair<int, int> (3, 6)] = 7;
	
	reactions_configurations_change[pair<int, int> (4, 7)] = 8;
	reactions_configurations_change[pair<int, int> (4, 2)] = 8;
	
	reactions_configurations_change[pair<int, int> (5, 5)] = 9;
	reactions_configurations_change[pair<int, int> (5, 2)] = 9;
	
	reactions_configurations_change[pair<int, int> (6, 9)] = 10;
	reactions_configurations_change[pair<int, int> (6, 6)] = 10;
	
	//======== Backward reactions
	backward_reaction_probabilities.push_back(0.0); // => Drop single pieces (should add drop compounds)
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0); 
	backward_reaction_probabilities.push_back(0.1); // => Disassembly reactions rates
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0);
	backward_reaction_probabilities.push_back(0.0); 
	
	// The forward reaction corresponding to the actual configuration being disassembled.
	// backward_reactions_occuring[nb_species] = {0, 0, 0, 0,   1, 2, 3, 4, 5, 6};  Just current_config - 4
	
	// New configurations after backward reaction. First key is current configuration, second is piece type. 
	backward_reactions_configurations_change[pair<int, int> (5, 1)] = 1; 
	backward_reactions_configurations_change[pair<int, int> (5, 2)] = 2;
	backward_reactions_configurations_change[pair<int, int> (6, 3)] = 3;
	backward_reactions_configurations_change[pair<int, int> (6, 4)] = 4;
	backward_reactions_configurations_change[pair<int, int> (7, 1)] = 5;
	backward_reactions_configurations_change[pair<int, int> (7, 2)] = 5;
	backward_reactions_configurations_change[pair<int, int> (7, 3)] = 6;
	backward_reactions_configurations_change[pair<int, int> (7, 4)] = 6;
	backward_reactions_configurations_change[pair<int, int> (8, 1)] = 7;
	backward_reactions_configurations_change[pair<int, int> (8, 2)] = 7;
	backward_reactions_configurations_change[pair<int, int> (8, 3)] = 7;
	backward_reactions_configurations_change[pair<int, int> (8, 4)] = 7;
	backward_reactions_configurations_change[pair<int, int> (8, 5)] = 2; // maybe troublesome
	backward_reactions_configurations_change[pair<int, int> (9, 1)] = 5;
	backward_reactions_configurations_change[pair<int, int> (9, 2)] = 5;
	backward_reactions_configurations_change[pair<int, int> (9, 5)] = 2;
	backward_reactions_configurations_change[pair<int, int> (10, 1)] = 9;
	backward_reactions_configurations_change[pair<int, int> (10, 2)] = 9;
	backward_reactions_configurations_change[pair<int, int> (10, 3)] = 6;
	backward_reactions_configurations_change[pair<int, int> (10, 4)] = 6;
	backward_reactions_configurations_change[pair<int, int> (10, 5)] = 9;
	
}

static void reset(void)
{
	int i;
	int channel;
	char connector_name[10];
	
	
	// Identity of the piece
	robot_name = (char*)robot_get_name();
	char* pname_tok = strtok(robot_name, "_");
	piece_type = atoi(&pname_tok[1]);
	pname_tok = strtok(NULL, "_");
	piece_num = atoi(pname_tok);
	carrying_robot_num = 0;
	configuration_complete = piece_type;
	reaction_occuring = 0;
	
	// robot_console_printf("name: %s, type: %d, num: %d\n", robot_name, piece_type, piece_num);
	
	
	max_connector_number = piece_type_nb_connectors[piece_type-1];

	////robot_console_printf("======= %d, %d\n", piece_type, nb_connectors);
	for (i = 0; i< max_connector_number; i++) {
		// The first connector is the one that attaches to the robot arm
		// Construct the names of connectors
		//sprintf(connector_name, "%s_c%d", robot_name, 2*i+1);
		sprintf(connector_name, "c%d", i+1);
		//robot_console_printf("connector %d: %s \n", i+1, connector_name);

		// Activate the connectors
		connectors[i] = robot_get_device(connector_name);
		connector_enable_presence(connectors[i], TIME_STEP);

		// Internal state
		connector_status[i] = UNLOCKED;
		connector_presence[i] = 0;
		count_badlock[i] = 0;
	}

	// Enable keyboard
	robot_keyboard_enable(TIME_STEP);


	// Enable the emitter/receiver ------------%
	commEmitter = robot_get_device("rs232_out");
	commReceiver = robot_get_device("rs232_in");

	 /* If the channel is not the good one, we change it. */
	channel = emitter_get_channel(commEmitter);
	if (channel != COMMUNICATION_CHANNEL) {
		emitter_set_channel(commEmitter, COMMUNICATION_CHANNEL);
	}
	
	// Communication range
	float comm_range = emitter_get_range(commEmitter);
	if (comm_range != COMMUNICATION_RANGE) {
		emitter_set_range(commEmitter, COMMUNICATION_RANGE);
	}

	receiver_enable(commReceiver, TIME_STEP);
	
	gps = robot_get_device("gps");
	gps_enable(gps, TIME_STEP);
	
	srand(piece_num*time(NULL));
	//robot_console_printf("The %s robot is reset, it uses %d connectors\n", robot_name, max_connector_number);

	// Read the assembly plan
	/*
		TODO Read Plan from file ?
	*/
	populateAssemblyPlan();
	
	// Initial direction
	float theta = 2*M_PI*(rand()/(RAND_MAX + 1.0));
	movement_direction[0] = cos(theta);
	movement_direction[1] = sin(theta);
	
	robot_step(TIME_STEP);
	
	return;
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
