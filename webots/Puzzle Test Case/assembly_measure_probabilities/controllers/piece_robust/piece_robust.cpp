/*
* File:         piece_robutst.cpp
* Date:         June 2008
* Description:  Controller for pieces, according to some assembly plan.
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
#include <vector>
#include <map>


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

using namespace std;

char *robot_name;
int piece_num;
int piece_type;
int carrying_robot_num;
int configuration_complete; 	// Current configuration that piece is part of
int configuration_atom;			// Position in current configuration.
int reaction_occuring;
int new_configuration_complete;
int new_carrying_robot_num;
int configuration_reaction_occuring = 0;

static DeviceTag commEmitter, commReceiver;
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
// connectors that should face outward:
int piece_to_piece_connectors[] = {1, 1, 1, 1};
int max_connector_number = 0;
int nb_moves = DIVIDER_MOVES;
bool autolock = true;
bool assembling_locking = false;
bool debug = false;

bool robot_alignment = false;

// REACTIONS STUFF
vector<vector<float> > pieces_reactions_angles; // Piece i -> angle of reaction j
vector<vector<bool> > pieces_reactions_locking; // Piece i -> has to lock during reaction j
vector<vector<int> > pieces_reactions_connectors; // Current piece -> which connector has to lock during reaction j
map<pair<int, int>, int> configuration_during_reaction; // For the virtual configurations during some reactions


// robot states
typedef enum { SEARCH_FOR_PIECE, ALIGN_WITH_PIECE, APPROACH_PIECE, ROTATE_PIECE, SEARCH_FOR_ROBOT, ROTATE_PIECE_ASSEMBLY,
    ALIGN_WITH_ROBOT, WAIT, APPROACH_ROBOT, UPDATE_PIECES_KNOWLEDGE, SEPARATE_ROBOTS } RobotState;
// piece states
typedef enum { FREE, CLAIMED, ROTATING, CARRIED, ROTATING_ASSEMBLY, ASSEMBLING, ASSEMBLING_LOCKED, KNOWLEDGE_UPDATED, NULL_STATE} PieceState;
PieceState state = FREE;


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
		debug = !debug;
	} else if(key == (78-KEYB_OFF)) { // L
		autolock = !autolock;
		//robot_console_printf("Locking: %d", autolock);
	}
}

void parseMessage(char* recMessage, int msgValues[10]) {
	// Parse the message, format:
	// robot: 0 robot_num state piece_type piece_num configuration_complete configuration_other_assembling
	// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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

static int run(int ms)
{
	int i;
	char sendMessage[30];  // message that robot sends
	float dzEmit;
	int msgValues[10];
	char *recMessage;

	//float next_position[2];

	// Handle keyboard
	keyboard();

	// Connector status
	for (i = 0; i < max_connector_number; i++) {
		connector_presence[i] = connector_get_presence(connectors[i]);
	}
	////robot_console_printf("Connectors : %d %d\n", connector_presence[0], connector_presence[1]);


	// Lock automatically
	if (autolock) {
		// 0 should lock automatically (top connector) when a robot is trying to approach it.
		if (state == CLAIMED && !unlock[0] && connector_presence[0] && !connector_status[0]) {
			//robot_console_printf("Lock %d !\n", i);
			connector_lock(connectors[0]);
			connector_status[0] = LOCKED;
		}
		
		// Others should only lock when applicable
		if (assembling_locking) {
			for (i = 1; i < max_connector_number; i++) {
				//Should lock automatically
				if (!unlock[i] && connector_presence[i] && !connector_status[i]) {
					//robot_console_printf("Lock %d !\n", i);
					connector_lock(connectors[i]);
					connector_status[i] = LOCKED;
				}
			}
		}
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
	
	//===== Message format:
	// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
	// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle

	sprintf(sendMessage, "1 %d %d %d %d %.3f", configuration_complete, piece_num, state, carrying_robot_num, 0.0);
    
	switch(state){

	case FREE:
	
		// assembling_locking = false;
		
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
		
		while (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
			const void *buffer = receiver_get_data(commReceiver);
			recMessage = (char*) buffer;
			
			// Parse the message
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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
				
				if (debug) {
					robot_console_printf("Claimed by %d\n", carrying_robot_num);
				}
				
				receiver_next_packet(commReceiver);
				break;
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
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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
	  	}

		break;

   case CARRIED:
		
		while (receiver_get_queue_length(commReceiver) > 0) {
        	const void *buffer = receiver_get_data(commReceiver);
   	  		recMessage = (char*) buffer;

			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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
					// robot_console_printf("Should rotate of : %f %f %f %f\n", turnAngle, turnAngle2, turnAngle3, turnAngle4);
					
					sprintf(sendMessage, "1 %d %d %d %d %.3f", configuration_complete, piece_num, state, carrying_robot_num, turnAngle);
					emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
				} else {
			    	state = ASSEMBLING;
			
					if (debug) {
						robot_console_printf("To Assembling\n");
					}
					
					// receiver_next_packet(commReceiver);
					break;
     			}
				// robot_console_printf("dzEmit err %f %f\n", dzEmit, EPS_Z);
				
     		}

     		receiver_next_packet(commReceiver);
	  	}
		break;
	
	case ASSEMBLING:
		
		// If we have to lock, check the status of the connector to lock
		if (assembling_locking) {
			
			// Confirm to my carrying robot that the rotation is finished
			while (receiver_get_queue_length(commReceiver) > 0) {
	     		const void *buffer = receiver_get_data(commReceiver);
		  		recMessage = (char*) buffer;

				// Parse the received message.
				// Format:
				// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
				// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
				parseMessage(recMessage, msgValues);

				// Carrying robot still waiting on alignment
		  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE_ASSEMBLY){
		  			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
	     		}

	     		receiver_next_packet(commReceiver);
		  	}
			
			
			// robot_console_printf("%d %d %d\n", configuration_reaction_occuring, pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1],  connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]]);

			if (connector_status[pieces_reactions_connectors[configuration_reaction_occuring-1][reaction_occuring-1]] == LOCKED) {
				// robot_console_printf("Locked\n");
				state = ASSEMBLING_LOCKED;
				
				if (debug) {
					robot_console_printf("To Assembling Locked\n");
				}
			}
		
		} else {
			
			// If we do nothing, just wait for a message by the carrying robot
			while (receiver_get_queue_length(commReceiver) > 0) {
	     		const void *buffer = receiver_get_data(commReceiver);
		  		recMessage = (char*) buffer;

				// Parse the received message.
				// Format:
				// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
				// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
				parseMessage(recMessage, msgValues);

				// Carrying robot still waiting on alignment
		  		if (isMsgFromRobotToMe(msgValues) && msgValues[2] == ROTATE_PIECE_ASSEMBLY){
		  			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
	     		}

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

	     		receiver_next_packet(commReceiver);
		  	}
		}
		
		break;
		
	case ASSEMBLING_LOCKED:
		// Only used by the piece that should lock the other configuration
		
		// Tell our robot that the connection is finished
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

		while (receiver_get_queue_length(commReceiver) > 0) {
     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// robot_console_printf("Robot message: %s\n", recMessage);
			
			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
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

     		receiver_next_packet(commReceiver);
	  	}
	
		break;
	
	case KNOWLEDGE_UPDATED:
		// Tell our robot that the knowledge is updated
		emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
		
		while (receiver_get_queue_length(commReceiver) > 0) {
     		const void *buffer = receiver_get_data(commReceiver);
	  		recMessage = (char*) buffer;

			// robot_console_printf("Robot message: %s\n", recMessage);
			// robot_console_printf("Ass, %d, %d, %d, %d\n", configuration_complete, carrying_robot_num, new_configuration_complete, new_carrying_robot_num);
			
			// Parse the received message.
			// Format:
			// robot: 0 | robot_num | state | piece_type | piece_num | configuration_complete | reaction_occuring | new_configuration | new_carrying_robot_num 
			// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
			parseMessage(recMessage, msgValues);

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

     		receiver_next_packet(commReceiver);
	  	}
		break;
		
	   case NULL_STATE:
	   		break;

   }

	return TIME_STEP;           /* run for TIME_STEP ms */
}


// ================================================================================================================

void populateAssemblyPlan() {
	const int nb_reactions = 4;
	const int nb_pieces = 5;
	
	// float angles[nb_pieces][nb_reactions] = {{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 3.14}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 1.6}};
	float angles[nb_pieces][nb_reactions] = {{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 3.14, -1.57}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 3.14}};
	bool locks[nb_pieces][nb_reactions] = {{true, false, false, false}, {true, false, true, true}, {false, true, true, false}, {false, true, false, false}, {false, false, false, true}};
	int connectors[nb_pieces][nb_reactions] = {{1, 0, 0, 0}, {1, 0, 3, 2}, {0, 1, 2, 0}, {0, 1, 0, 0}, {0, 0, 0, 3}};

	/*
		TODO Should depend on some external file...
	*/
	
	for(int i = 0; i < nb_pieces; ++i) {
		pieces_reactions_angles.push_back(vector<float> (angles[i], angles[i] + sizeof(angles[i]) / sizeof(float)));
		pieces_reactions_locking.push_back(vector<bool> (locks[i], locks[i] + sizeof(locks[i]) / sizeof(bool)));
		pieces_reactions_connectors.push_back(vector<int> (connectors[i], connectors[i] + sizeof(connectors[i]) / sizeof(int)));
	}
	
	// for(int i = 0; i < nb_pieces; ++i) {
	// 	if (i==(piece_type-1)) {
	// 		for(int j = 0; j < nb_reactions; ++j) {
	// 			piece_reactions_angles.push_back(angles[i][j]);
	// 			piece_reactions_locking.push_back(locks[i][j]);
	// 			piece_reactions_connectors.push_back(connectors[i][j]);
	// 		}
	// 	}
	// }
	
	configuration_during_reaction[pair<int, int> (2,4)] = 5; // Configuration 2, reaction 4 => Configuration 5
	
	// for(int i = 0; i < nb_reactions; ++i) {
	// 		robot_console_printf("%f ", piece_reactions_angles[i]);
	// 		robot_console_printf("%d ", (int)piece_reactions_locking[i]);
	// 	}
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
	configuration_atom = 0;
	reaction_occuring = 0;
	
	// robot_console_printf("name: %s, type: %d, num: %d", robot_name, piece_type, piece_num);
	
	
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


	//robot_console_printf("The %s robot is reset, it uses %d connectors\n", robot_name, max_connector_number);

	// Read the assembly plan
	/*
		TODO Read Plan from file ?
	*/
	populateAssemblyPlan();
	
	return;
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
