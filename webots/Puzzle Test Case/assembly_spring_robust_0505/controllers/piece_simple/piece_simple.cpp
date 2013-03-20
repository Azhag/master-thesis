/*
* File:         piece_simple.c
	* Date:         March 7th 2008
	* Description:  Controller for pieces, locking the connectors if available.
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

#define KEYB_OFF 2

#define TIME_STEP 32

#define UNLOCKED 0
#define LOCKED 1

#define MAX_COUNT_BADLOCK 100
#define MAX_COUNT_UNLOCK 100
#define COMMUNICATION_CHANNEL 1

#define LIM_CONNECTORS 10

#define DIVIDER_MOVES 5
#define EPS_X 0.02
#define EPS_Z 0.03
#define WAIT_TIME 200

const char *robot_name;

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
int state5Known = 0;  // to shut off messages from piece
int state6Known = 0;
int state7Known = 0;
int state8Known = 0;
char nearbyRobot;
const float *direction = NULL;

int piece_type_nb_connectors[] = {2, 4, 3, 2};
// connectors that should face outward:
int piece_to_piece_connectors[] = {1, 1, 1, 1};
int max_connector_number = 0;
int nb_moves = DIVIDER_MOVES;
bool autolock = true;

// piece states
enum { FREE, CLAIMED, ROTATING, CARRIED, PIECE_5, PIECE_6, PIECE_7, PIECE_8, NULL_STATE};
int state = FREE;

const char* state2char(int s)
{
	switch (s) {
		case 0:
			return "0";
		case 1:
			return "1";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		default:
			return "\0";
   }
}

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
		robot_console_printf("Status %s: unlock=%d %d %d %d, presence: %d %d %d %d, status: %d %d %d %d \n", 		robot_name, unlock[0], unlock[1], unlock[2], unlock[3],
				connector_presence[0], connector_presence[1], connector_presence[2], connector_presence[3],
				//connector_presence[4],connector_presence[5],connector_presence[6],connector_presence[7],
				connector_status[0],connector_status[1],connector_status[2],connector_status[3]
				//connector_status[4],connector_status[5],connector_status[6],connector_status[7]
				);
	} else if(key == (78-KEYB_OFF)) { // L
		autolock = !autolock;
		//robot_console_printf("Locking: %d", autolock);
	}
}

static void reset(void)
{
	int i;
	int channel;
	robot_name = robot_get_name();
	char connector_name[10];

	// Enable the Connectors

	// connectors[0] = robot_get_device("con1");
	// 	connectors[1] = robot_get_device("con2");
	// 	connector_enable_presence(connectors[0], TIME_STEP);
	// 	connector_enable_presence(connectors[1], TIME_STEP);
	int piece_type = robot_name[1] - '0' -1;
	max_connector_number = piece_type_nb_connectors[piece_type];

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

	receiver_enable(commReceiver, TIME_STEP);


	//robot_console_printf("The %s robot is reset, it uses %d connectors\n", robot_name, max_connector_number);

	return;
}

static int run(int ms)
{
	int i, j;
  int robotNear;
	char sendMessage[20];  // message that robot sends
	char lockMessage[20];
	char str[20];
	float dxEmit, dzEmit;

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
		for (i = 0; i < max_connector_number; i++) {
			//Should lock automatically
			if (!unlock[i] && connector_presence[i] &&
				!connector_status[i]) {
				//robot_console_printf("Lock %d !\n", i);
				connector_lock(connectors[i]);
				connector_status[i] = LOCKED;
			}

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

			// if (!unlock && connector_presence[2*i] && connector_presence[2*i+1] &&
			// 			!connector_status[2*i] && !connector_status[2*i+1]) {
			// 			//Should lock
			// 			//robot_console_printf("Lock %d !\n", i);
			// 			connector_lock(connectors[2*i]);
			// 			connector_lock(connectors[2*i+1]);
			// 			connector_status[2*i] = LOCKED;
			// 			connector_status[2*i+1] = LOCKED;
			// 		}
			//
			// 		// Unlock requested
			// 		if (unlock && (connector_status[2*i] || connector_status[2*i+1])) {
			// 			connector_unlock(connectors[2*i]);
			// 			connector_unlock(connectors[2*i+1]);
			// 		}
			//
			// 		// Allow locking when out of presence
			// 		if (unlock && !connector_presence[2*i] && !connector_presence[2*i+1]
			// 		&& connector_status[2*i] && connector_status[2*i+1]) {
			// 			unlock= !unlock;
			// 			connector_status[2*i] = UNLOCKED;
			// 			connector_status[2*i+1] = UNLOCKED;
			// 			//robot_console_printf("Unlock %d..\n", i);
			// 		}
			//
			// 		// Unlock if bad position
			// 		if ((connector_status[2*i] == LOCKED && !connector_presence[2*i] )||
			// 		(connector_status[2*i+1]== LOCKED && !connector_presence[2*i+1])){
			// 			unlock = true;
			// 		}
		}
	}

	const char *robotID = robot_get_name();
	int piece_type = robotID[1] - '0' -1;
	strcpy(sendMessage, robotID);
	strcat(sendMessage, state2char(state));
//	robot_console_printf("message: %s\n", sendMessage);
	// emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);


	switch(state){

		case FREE:

			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

			while (receiver_get_queue_length(commReceiver) > 0) {
				/* Read current packet's data */
				const void *buffer = receiver_get_data(commReceiver);
				const char *recMessage = (char*) buffer;

				if (recMessage[0] == 'r' && (recMessage[2] == '1' ||
				    recMessage[2] == '2' || recMessage[2] == '3') &&
				    recMessage[3] == robot_name[1] && recMessage[4] == robot_name[3]){
					// piece is claimed
					// NOTE: '1' corresponds to robot state ALIGN_WITH_PIECE
					//		 '2' corresponds to robot state APPROACH_PIECE
          			waitCtr = 0;
					state = CLAIMED;
				}

				receiver_next_packet(commReceiver);
   		}

		break;

		case CLAIMED:

     // robotNear = 0;

      waitCtr += 1;

		    emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

      if (waitCtr > WAIT_TIME) {
        waitCtr = 0;
        state = FREE;
        }
/*
		  while (receiver_get_queue_length(commReceiver) > 0) {
				const void *buffer = receiver_get_data(commReceiver);
				const char *recMessage = (char*) buffer;

				// TO DO: add more conditions
				if (recMessage[0] == 'r') {
          robotNear = 1;
				//	state = FREE;
				}
				receiver_next_packet(commReceiver);
			}
			if (receiver_get_queue_length(commReceiver) == 0) {
				// robot went out of range
         if (waitCtr > WAIT_TIME) {
          state = FREE;
        }
				//state = FREE;
			}

      if (robotNear == 0) {
        state = FREE;
      } */
      //if (receiver_get_queue_length(commReceiver) == 0) {

      //}

			if (connector_status[0] == LOCKED) {
				// The first connector is the one that attaches to the robot arm
        		waitCtr = 0;
				state = ROTATING;
			}


		break;

		case ROTATING:

   ////robot_console_printf("Rotating--\n");
     // wait until piece has first snapped into place
     //if (rotate_ctr < 10) {
	//	  rotate_ctr += 1;
   //  }

     //j = piece_to_piece_connectors[piece_type-1];
     if (receiver_get_queue_length(commReceiver) > 0) {

     	const void *buffer = receiver_get_data(commReceiver);
	  	const char *recMessage = (char*) buffer;

	  	if (recMessage[0] == 'r' && recMessage[3] == robot_name[1] &&
          recMessage[4] == robot_name[3]){
            // state 3 for robot is ROTATE_PIECE

	  		direction = receiver_get_emitter_direction(commReceiver);
	  		float xEmit = direction[0];
	  		float zEmit = direction[2];
	  		float turnAngle = atan2(xEmit, zEmit); // angle piece has to turn
			// convert angle to string
			strcat(sendMessage, "A");
     		sprintf(str, "%.3g", turnAngle);
     		strcat(sendMessage, str);
			//robot_console_printf("message: %s\n", sendMessage);
			emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);

	  		//dxEmit = direction[0]; // x distance from emitter of piece
			// receiver z axis faces away from connector that should be
			//    attached to another connector
			dzEmit = direction[2]-1;
			////robot_console_printf("direction: %f %f %f\n", direction[0], direction[1], direction[2]);
			//robot_console_printf("dzEmit: %f\n", fabs(dzEmit));
			if (fabs(dzEmit) <= EPS_Z) {
				//robot_console_printf("Lock before CARRIED\n");
				//connector_lock(connectors[0]);
				//if (recMessage[2] == '4') {  // robot is is next state
			    	state = CARRIED;
				//}
     		}
     	}

     	receiver_next_packet(commReceiver);
	  }
	  // else?

	  if (connector_status[0] == UNLOCKED) {
	  			state = FREE;
	  }

		break;

   case CARRIED:

   	while (receiver_get_queue_length(commReceiver) > 0) {
        const void *buffer = receiver_get_data(commReceiver);
   	  	const char *recMessage = (char*) buffer;

   	  	if (recMessage[0] == 'r' && recMessage[2] == '3' &&
            recMessage[3] == robot_name[1] && recMessage[4] == robot_name[3]){
   	  	  // only send state until robot recognizes that it can search for a robot
          emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);
        }

  // add conditions?
		// for piece 2 to distinguish which state to go into
		if (recMessage[0] == 'r' && recMessage[2] == '7' && recMessage[3] == '1') {
			otherPiece = 1;
		}
		if (recMessage[0] == 'r' && recMessage[2] == '7' && recMessage[3] == '7') {
			otherPiece = 7;
		}

		receiver_next_packet(commReceiver);
	  }

	 if (connector_status[1] == LOCKED) {
	  // The second connector attaches to another piece
	  // should only bond with another appropriate piece--TO DO: check other piece?
	  		// new states of 2-piece conglomerates
	  		switch (piece_type+1) {
				// don't want pieces unnecessarily broadcasting messages
				case 1:
					state = NULL_STATE; // PIECE_5;
					break;
				case 2:
					if (otherPiece > 0) {
						if (otherPiece == 1) {
							state = PIECE_5;
						}
						if (otherPiece == 7) {
							state = PIECE_8;
						}
						otherPiece = 0;
					}
					break;
				case 3:
					state = PIECE_6;
					break;
				case 4:
					state = NULL_STATE; // PIECE_6;
					break;
		    }
   	 }

   break;

   case PIECE_5:  // 5th type of piece: 1 and 2

	while (receiver_get_queue_length(commReceiver) > 0) {
        const void *buffer = receiver_get_data(commReceiver);
   	  	const char *recMessage = (char*) buffer;

   	  	if (recMessage[0] == 'r' && recMessage[2] == '7' && state5Known == 0) {
			if (recMessage[3] == '1' || recMessage[3] == '2') {
   	  	  	// only send state until robot recognizes that it can detach
   	  	  	// robot does not yet think its piece type is state 5
   	  	    	robot_console_printf("Nearby robot: %c\n", recMessage[1]);
   	  	    	sprintf(lockMessage, "Lock5%c", recMessage[1]); // send robot ID for uniqueness
				emitter_send_packet(commEmitter, lockMessage, strlen(lockMessage) + 1);
			}
			else if (recMessage[3] == '5') {
      robot_console_printf("STATE 5 KNOWN \n");
				state5Known = 1;
			}
		}
		receiver_next_packet(commReceiver);
	  }

	if (piece_type+1 == 2 && connector_status[3] == LOCKED) {
		// only piece 2 goes into this state
		state = PIECE_7;
	}

   break;

   case PIECE_6:  // 6th type of piece: 3 and 4

   	while (receiver_get_queue_length(commReceiver) > 0) {
           const void *buffer = receiver_get_data(commReceiver);
      	  	const char *recMessage = (char*) buffer;

      	  	if (recMessage[0] == 'r' && recMessage[2] == '7' && state6Known == 0) {
      	  		if (recMessage[3] == '3' || recMessage[3] == '4'){
      	  	  // only send state until robot recognizes that it can detach
      	  	  // robot does not yet think its piece type is state 6
   			 		sprintf(lockMessage, "Lock6%c", recMessage[1]); // send robot ID for uniqueness
					emitter_send_packet(commEmitter, lockMessage, strlen(lockMessage) + 1);
				}
				else if (recMessage[3] == '6') {
					state6Known = 1;
				}
   			}
   		receiver_next_packet(commReceiver);
   	  }

   	if (piece_type+1 == 3 && connector_status[2] == LOCKED) {
			// only piece 3 goes into this state
			state = PIECE_7;
	}

   break;

   case PIECE_7:  // 7th type of piece: 1, 2, 3, 4

   	  while (receiver_get_queue_length(commReceiver) > 0) {
		  	const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;

			if (recMessage[0] == 'r' && recMessage[2] == '7' && state7Known == 0) {
				if (recMessage[3] == '5' || recMessage[3] == '6'){
			  // only send state until robot recognizes that it can detach
			  // robot does not yet think its piece type is state 7
		 		sprintf(lockMessage, "Lock7%c", recMessage[1]); // send robot ID for uniqueness
				emitter_send_packet(commEmitter, lockMessage, strlen(lockMessage) + 1);
				}
				else if (recMessage[3] == '7') {
					state7Known = 1;
				}
			}
			receiver_next_packet(commReceiver);
   	  }

   	  if (piece_type+1 == 2 && connector_status[2] == LOCKED) {
	  			// only piece 2 goes into this state
	  		state = PIECE_8;
	}

   break;

   case PIECE_8:

	while (receiver_get_queue_length(commReceiver) > 0) {
		const void *buffer = receiver_get_data(commReceiver);
		const char *recMessage = (char*) buffer;

		if (recMessage[0] == 'r' && recMessage[2] == '7' && state8Known == 0) {
			if (recMessage[3] == '7' || recMessage[3] == '2'){
		  // only send state until robot recognizes that it can detach
		  // robot does not yet think its piece type is state 8
			sprintf(lockMessage, "Lock8%c", recMessage[1]); // send robot ID for uniqueness
			emitter_send_packet(commEmitter, lockMessage, strlen(lockMessage) + 1);
			}
			else if (recMessage[3] == '8') {
				state8Known = 1;
			}
		}
		receiver_next_packet(commReceiver);
   	}

   break;

   case NULL_STATE:

   break;

   }

	return TIME_STEP;           /* run for TIME_STEP ms */
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
