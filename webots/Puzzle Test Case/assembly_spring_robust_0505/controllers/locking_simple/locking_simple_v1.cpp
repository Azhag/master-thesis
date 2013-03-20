/*
* File:         locking_simple.c
	* Date:         April 2008
	* Description:  Controller locking the connectors if available.
	* Author:       Spring Berman (modified from file by Loic Matthey)
	* Modifications:
*
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <device/robot.h>
#include <device/differential_wheels.h>
#include <device/distance_sensor.h>
#include <device/connector.h>
#include <device/emitter.h>
#include <device/receiver.h>
#include <device/gps.h>
#include <device/servo.h>

#define KEYB_OFF 2

#define TIME_STEP 32
#define MAX_SENSOR_NUMBER 16
#define MAX_CONNECTOR_NUMBER 1
#define RANGE (1024 / 2)
#define COMMUNICATION_CHANNEL 1
#define EPS_X 0.04
#define EPS_Z 0.03
#define ABORT 700
#define WAIT_TIME 20

#define UNLOCKED 0
#define LOCKED 1

#define SPEED_INCR 100.0
#define INITIAL_SPEED 10000

static DeviceTag commEmitter, commReceiver, gps;
//static robot_types robot_type;
static int found_robot;

static DeviceTag arm_servo;
static DeviceTag sensors[MAX_SENSOR_NUMBER];
static DeviceTag connectors[MAX_CONNECTOR_NUMBER];
static float matrix[MAX_SENSOR_NUMBER][2];
static int sensor_number;
static int range;
static char robot_num;
const char *robot_name;
const float *direction = NULL;
double turnAngle = 0;
char robot1, robot2;
char pieceType = '0';  // type of piece the robot is carrying
char pieceTypeNew = '0';
char pieceNum;   // number of the piece of a type that the robot is carrying
int ctr = 0;
int abortCtr = 0;  // if this goes too high then abort the state
int waitCtr = 0;   // use to wait for communication

int connector_status[MAX_CONNECTOR_NUMBER];
bool unlock = false;
bool braitenberg = true;

float speed_bias[2];

FILE* out_file;
float elapsed_time = 0.0;

// robot states
enum { SEARCH_FOR_PIECE, ALIGN_WITH_PIECE, APPROACH_PIECE, ROTATE_PIECE, SEARCH_FOR_ROBOT,
      ALIGN_WITH_ROBOT, WAIT, APPROACH_ROBOT, SEPARATE_ROBOTS };
int state = SEARCH_FOR_PIECE;
// SEARCH_FOR_ROBOT: Robot looks for an appropriate piece
// ALIGN_WITH_ROBOT: Robot aligns with another robot with complementary piece
// WAIT: Robot waits for other robot to align

void velBraitenberg(unsigned short *sensVal, float *speed)
// The Braitenberg algorithm is really simple, it simply computes the
	// 	speed of each wheel by summing the value of each sensor multiplied by
	// 	its corresponding weight. That is why each sensor must have a weight
	// 	for each wheel.
{
	int i, j;

	for (i = 0; i < 2; i++) {
		speed[i] = speed_bias[i];

		if (braitenberg) {
			for (j = 0; j < sensor_number; j++) {
			/*
			* We need to recenter the value of the sensor to be able to get
			* negative values too. This will allow the wheels to go
			* backward too.
			*/
			////robot_console_printf("%f ",(float) sensor_number_value[j]);

			speed[i] += 3.0 * matrix[j][i] * (1 - ((float) sensVal[j] / range));
			}
		}
	}
	return;
}

void velAlign(float *speed) {
	// Sets velocity of robot that is aligning itself with another robot
	// for the purpose of putting pieces together

	float wheelRad = 0.0212;  // wheel radius
	float axleL = 0.0893;  // axle length
	float linL = 0.005; //0.02;     // feedback linearization length
	float theta;         // orientation of robot (radians)
	float Kgain = 100;          // proportional controller gain
	const float *gps_matrix;
	float euler_angles[3];
	float vel, omega;   // translational, rotational velocities of robot
	//float theta12;  // angle made by direction to emitter in robot frame
	float s12, r12, x12, y12;  // signal strength, distance between robots 1 and 2
	float x12G, y12G, x12Gdes, y12Gdes;
				// x12G, y12G are wrt inertial coordinates
   float r12des = 0.20;  // desired distance between robots 1 and 2

	/* Get GPS data */
	gps_matrix = gps_get_matrix(gps);
	gps_euler(gps_matrix, euler_angles);
	theta = euler_angles[1];
	////robot_console_printf("Orientation % .3f\n", euler_angles[1]);
	//gps_position_x(gps_matrix), gps_position_y(gps_matrix),
	s12 = receiver_get_signal_strength(commReceiver);
	r12 = 1/sqrt(s12);
	//robot_console_printf("Radius: %f\n", r12);
	x12 = direction[0]*r12;
	y12 = direction[2]*r12;
	x12G = x12*cos(theta) + y12*sin(theta);
	y12G = -x12*sin(theta) + y12*cos(theta);
	x12Gdes = -r12des*sin(theta);
	y12Gdes = -r12des*cos(theta);

	vel = Kgain*(x12G - x12Gdes)*cos(theta) + Kgain*(y12G - y12Gdes)*sin(theta);
	omega = Kgain*(x12G - x12Gdes)*(-sin(theta)/linL) + Kgain*(y12G - y12Gdes)*cos(theta)/linL;

	speed[0] = (2*vel + axleL*omega)/(-2*wheelRad);
	speed[1] = axleL*omega/wheelRad + speed[0];

}

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
		case 7:
			return "7";
		default:
			return "\0";
   }
}

int char2state(char s)
{
	return ((int)s)-48;
	// switch (s) {
	// 	case '0':
	// 		return 0;
	// 	case '1':
	// 		return 1;
	// 	case '2':
	// 		return 2;
	// 	case '3':
	// 		return 3;
	// 	case '4':
	// 		return 4;
	// 	case '5':
	// 		return 5;
	// 	case '6':
	// 		return 6;
	// 	case '7':
	// 		return 7;
	// 	default:
	// 		return -1;
	//    }
}

/* Output the signals for matlab */
void file_output() {
	if (out_file == NULL) {
		char filename[50];
		int nb_file = 0;

		// We'll open a file that doesn't exist for now.
		sprintf(filename, "output_values_%s_%d.txt", robot_name, nb_file);

		out_file = fopen(filename, "r");
		while (out_file) {
			// The file already exists, get another file name
			fclose(out_file);
			nb_file ++;
			sprintf(filename, "output_values_%s_%d.txt", robot_name, nb_file);
			out_file = fopen(filename, "r");
		}
		out_file = fopen(filename, "w");
	}

	fprintf(out_file, "%f ", elapsed_time);

	// New puzzle completed
	fprintf(out_file, "%f ", 1.0);

	fprintf(out_file, "\n");

	fflush(out_file);
}


/* Handle the keyboard actions */
void keyboard() {
	int key;
	key = robot_keyboard_get_key();

	// if (key != 0) {
	// 	//robot_console_printf("key: %d\n", key+KEYB_OFF);
	// }

	if (key == (317-KEYB_OFF)) { // arrow up
		speed_bias[0] += SPEED_INCR;
		speed_bias[1] += SPEED_INCR;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (319-KEYB_OFF)) { // arrow down
		speed_bias[0] -= SPEED_INCR;
		speed_bias[1] -= SPEED_INCR;
		// speed[0] = speed[1] = 0.0;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (316-KEYB_OFF)) { // arrow left
		speed_bias[0] -= SPEED_INCR/2;
		speed_bias[1] += SPEED_INCR/2;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (318-KEYB_OFF)) { // arrow right
		speed_bias[0] += SPEED_INCR/2;
		speed_bias[1] -= SPEED_INCR/2;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (34-KEYB_OFF)) { // Space
		speed_bias[0] = speed_bias[1] = 0.0;
		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else 	if(key == (70-KEYB_OFF)) { // D
		if (!unlock && (connector_status[0] || connector_status[1])){
			unlock = true;
			//robot_console_printf("Unlocking\n");
		}
	} else if(key == (80-KEYB_OFF)) { // N
		braitenberg = false;
		//robot_console_printf("No braitenberg\n");
	} else	if(key == (68-KEYB_OFF)) { // B
		braitenberg = true;
		//robot_console_printf("Braitenberg\n");
	}
}


static void reset(void)
{
	int i,j;
	int channel;
	robot_name = robot_get_name();
	robot_num = robot_name[1];

	char connector_name[15];

	float khepera3_matrix[9][2] =
		{ {0, 0}, {-20000, 20000}, {-50000, 50000}, {-70001, 70000}, {70001, -70000},
		{50000, -50000}, {20000, -20000}, {0, 0}, {0, 0} };
	// { {-5000, -5000}, {-20000, 40000}, {-30000, 50000}, {-70000, 70000}, {70000, -60000},
	// {50000, -40000}, {40000, -20000}, {-5000, -5000}, {-10000, -10000} };
	char sensors_name[12];
	// float (*temp_matrix)[2];

	range = RANGE;

	sensor_number = 9;
	sprintf(sensors_name, "ds0");
	range = 2000;

	// Enable the Distance sensors and Braitenberg weights
	for (i = 0; i < sensor_number; i++) {
		sensors[i] = robot_get_device(sensors_name);
		distance_sensor_enable(sensors[i], TIME_STEP);

		if ((i + 1) >= 10) {
			sensors_name[2] = '1';
			sensors_name[3]++;

			if ((i + 1) == 10) {
				sensors_name[3] = '0';
				sensors_name[4] = (char) '\0';
			}
		} else {
			sensors_name[2]++;
		}

		for (j = 0; j < 2; j++) {
			matrix[i][j] = khepera3_matrix[i][j];
		}
	}

	// Enable the Connectors
	for (i = 0; i< MAX_CONNECTOR_NUMBER; i++) {
		// Construct the names of connectors
		sprintf(connector_name, "con%d", i+1);
		//robot_console_printf("connector %d: %s \n", i, connector_name);

		// Activate the connectors
		connectors[i] = robot_get_device(connector_name);
		connector_enable_presence(connectors[i], TIME_STEP);

		// Internal state
		connector_status[i] = UNLOCKED;
	}
		//
		// connectors[0] = robot_get_device("con1");
		// connectors[1] = robot_get_device("con2");
		// connector_enable_presence(connectors[0], TIME_STEP);
		// connector_enable_presence(connectors[1], TIME_STEP);
		//
		// for (i = 0; i< MAX_CONNECTOR_NUMBER; i++) {
		// 	connector_status[i] = UNLOCKED;
		// }

	// Basic speed at 0
	speed_bias[0] = INITIAL_SPEED;
	speed_bias[1] = INITIAL_SPEED;

	// Enable keyboard
	robot_keyboard_enable(TIME_STEP);

 // Enable the Arm servo
 arm_servo = robot_get_device("arm_servo");
 servo_enable_position(arm_servo, TIME_STEP);

	//robot_console_printf("The %s robot is reset, it uses %d sensors\n",robot_name, sensor_number);

   // Enable the emitter/receiver, gps ------------%
   commEmitter = robot_get_device("rs232_out");
   commReceiver = robot_get_device("rs232_in");
   gps = robot_get_device("gps");

   /* If the channel is not the good one, we change it. */
	channel = emitter_get_channel(commEmitter);
	if (channel != COMMUNICATION_CHANNEL) {
		emitter_set_channel(commEmitter, COMMUNICATION_CHANNEL);
	}

	receiver_enable(commReceiver, TIME_STEP);
	gps_enable(gps, TIME_STEP);

	found_robot = 0;

	return;
}

static int run(int ms)
{
	int i,j;
	unsigned short sensors_value[MAX_SENSOR_NUMBER];
	int connector_presence[MAX_CONNECTOR_NUMBER];
	float speed[2];
	float dxEmit, dzEmit;
 	char sendMessage[20];  // message that robot sends
	char angleMessage[20];  // angle to turn piece

	// Update the keyboard
	keyboard();

	// Update the distance sensors
	for (i = 0; i < sensor_number; i++) {
		sensors_value[i] = distance_sensor_get_value(sensors[i]);
	}

	// Update the connector presence sensors
	for (i = 0; i < MAX_CONNECTOR_NUMBER; i++) {
		connector_presence[i] = connector_get_presence(connectors[i]);
	}
	////robot_console_printf("Connectors : %d %d\n", connector_presence[0], connector_presence[1]);

	// Lock automatically
	if (!unlock && connector_presence[0] &&
		!connector_status[0]) {
		//Should lock
		//robot_console_printf("Lock !\n");
		connector_lock(connectors[0]);
		connector_status[0] = LOCKED;
	}

	// Unlock requested
	if (unlock) {
		connector_unlock(connectors[0]);
		connector_status[0] = UNLOCKED;
	}

	// Allow locking when out of presence
	if (unlock && !connector_presence[0]) {
		unlock= !unlock;
	}

	// Emitter/receiver
	/* Send null-terminated robotID */
	// const char *robotID = robot_get_name();
	// robot_num = robotID[1];
	sprintf(sendMessage, "%s%d%c%c", robot_name, state, pieceType, pieceNum);

 	// strcpy(sendMessage, robotID);
 	// strcat(sendMessage, state2char(state));
 	////robot_console_printf("State: %d\n", state);

	// const char stateChar = state2char(state);
	// const char *sendMessage = strcat(robotID, stateChar);
	robot_console_printf("message: %s\n", sendMessage);
	emitter_send_packet(commEmitter, sendMessage, strlen(sendMessage) + 1);


	// default velocities
	velBraitenberg(sensors_value, speed);

	switch(state){

	case SEARCH_FOR_PIECE:

			// what if it picks up a piece by accident?

			pieceType = '0';
			pieceNum = '0';

			if (receiver_get_queue_length(commReceiver) > 0) {
				/* Read current packet's data */
				const void *buffer = receiver_get_data(commReceiver);
				const char *recMessage = (char*) buffer;

				// what if recMessage is less than 5 indices long?
				if ( (recMessage[0] == 'p' && recMessage[4]=='0') ){
					// it's an unclaimed piece on the floor
					pieceType = recMessage[1];
					pieceNum = recMessage[3];
					robot_console_printf("Piece found-------\n");
	        		robot_console_printf("Message: %s\n", recMessage);
					speed[0] = 0;
					speed[1] = 0;
	        		abortCtr = 0;
	        		waitCtr = 0;
					state = ALIGN_WITH_PIECE;
				}
				receiver_next_packet(commReceiver);
	   		 }

		break;


	case ALIGN_WITH_PIECE:

		if (receiver_get_queue_length(commReceiver)) {
			const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;

			if (recMessage[0] == 'p' &&
				recMessage[1] == pieceType && recMessage[3] == pieceNum) {

				direction = receiver_get_emitter_direction(commReceiver);
				dxEmit = direction[0]; // x distance from emitter of piece
				dzEmit = direction[2]+1;
				//robot_console_printf("showMessage: %s\n", recMessage);

				if (fabs(dxEmit) > EPS_X || fabs(dzEmit) > EPS_Z) {
					//robot_console_printf("dxEmit: %f  dzEmit: %f\n", fabs(dxEmit), fabs(dzEmit));
					////robot_console_printf("Aligning with piece\n");

					//velAlign(speed);   // ALTERNATIVE
					if (dxEmit >= 0) {  // robot rotates about center point
						speed[0] = 4000;
						speed[1] = -4000;
					}
					else {
						speed[0] = -4000;
						speed[1] = 4000;
					}

				} else {
         			// move forward toward piece
         			state = APPROACH_PIECE;
				}

			}
			receiver_next_packet(commReceiver);
		}
		else {
			state = SEARCH_FOR_PIECE;
			//robot_console_printf("ALIGN_WITH_PIECE: Communication broken !\n");
		}
		//speed[0] = 0;
		//speed[1] = 0;
		break;

	case APPROACH_PIECE:

		if (connector_status[0] == UNLOCKED) {
			speed[0] = 5000;
			speed[1] = 5000;
		}
		else {
			state = ROTATE_PIECE;
		}

		break;

	case ROTATE_PIECE:

		if (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
			const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;

			//robot_console_printf("Piece message: %s\n", recMessage);

			if (recMessage[1] == pieceType && recMessage[3] == pieceNum){
					// the piece that it's carrying
				if (recMessage[5] == 'A' && turnAngle == 0) {
         		  sprintf(angleMessage, "%c%c%c%c%c", recMessage[6], recMessage[7], recMessage[8],
                 recMessage[9], recMessage[10]);
                 turnAngle = atof(angleMessage);
                 float arm_position = servo_get_position(arm_servo);
				  ////robot_console_printf("servo : %f ", arm_position);
				 servo_set_position(arm_servo, arm_position+turnAngle);
					//robot_console_printf("Angle message: %s\n", angleMessage);
				}
				if (recMessage[4] == '3') { // piece is now aligned
					// NOTE: 3 is the piece state CARRIED
					turnAngle = 0;

					// turn a little more for certain pieces
					/*if (pieceType == '3' || pieceType == '4') {
						float arm_position = servo_get_position(arm_servo);
				  		servo_set_position(arm_servo, arm_position-0.2);
					} */

					state = SEARCH_FOR_ROBOT;
				}
			}
			receiver_next_packet(commReceiver);
  		}

		speed[0] = 0;
		speed[1] = 0;

		break;

	case SEARCH_FOR_ROBOT:
		//velBraitenberg(sensors_value, speed);
		/* Is there at least one packet in the receiver's queue ? */
		if (receiver_get_queue_length(commReceiver) > 0) {
			const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;
			//robot_console_printf("Message: %s\n", recMessage);

			if (recMessage[0] == 'r' && char2state(recMessage[2])==SEARCH_FOR_ROBOT) {

				// specific assembly plan
				if ( (pieceType == '1' && recMessage[3] == '2') ||
					 (pieceType == '2' && recMessage[3] == '1') ||
					 (pieceType == '3' && recMessage[3] == '4') ||
					 (pieceType == '4' && recMessage[3] == '3') ||
					 (pieceType == '5' && recMessage[3] == '6') ||
					 (pieceType == '6' && recMessage[3] == '5') ||
					 (pieceType == '2' && recMessage[3] == '7') ||
					 (pieceType == '7' && recMessage[3] == '2')) {
			    	robot_console_printf("Robot nearby------\n");
					speed[0] = 0;
					speed[1] = 0;

					if (pieceType == '2' && recMessage[3] == '7') {
						// rotate piece 2 in new position to fit with piece 7
						float arm_position = servo_get_position(arm_servo);
				 		servo_set_position(arm_servo, arm_position-3.14);
					}
					state = ALIGN_WITH_ROBOT;
				}
			}

			receiver_next_packet(commReceiver);
		}

		if (pieceType == '7') {
			//robot_console_printf("Holding piece 7\n");
		//	speed[0] = 0;
		//	speed[1] = 0;
		}

		break;

	case ALIGN_WITH_ROBOT:

		if (receiver_get_queue_length(commReceiver) > 0) {
			// other robot is still in communication range
			/* Read current packet's data */

     		const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;

			// MORE CONDITIONS
			if (recMessage[0] == 'r'){  // it's a robot (not a piece)

				if (robot_num < recMessage[1]) {
					robot1 = robot_num;
					robot2 = recMessage[1];
				}
				else {
					robot1 = recMessage[1];
					robot2 = robot_num;
				}

				/* print null-terminated robotID */
				//robot_console_printf("Message from nearby robot: \"%s\"\n", recMessage);

				////robot_console_printf("Other robot is in state: %d\n",char2state(recMessage[2]));
				if (robot_num == robot2 && char2state(recMessage[2])==ALIGN_WITH_ROBOT) {
					// robot 2 waits while robot 1 aligns
					speed[0] = 0;
					speed[1] = 0;
					state = WAIT;
					//robot_console_printf("Robot is R2, waiting\n");
				}
				if (robot_num == robot1 || (robot_num == robot2 && char2state(recMessage[2])==WAIT)) {
					////robot_console_printf("Robot is R1, aligning\n");
					direction = receiver_get_emitter_direction(commReceiver);
					//robot_console_printf("Direction: [%f %f %f]\n", direction[0],  direction[1], direction[2]);

					dxEmit = direction[0]; // x distance from emitter of other robot
					dzEmit = direction[2]+1;

					// TO DO: replace this with more general code for multiple robots
					if (fabs(dxEmit) > EPS_X || fabs(dzEmit) > EPS_Z) {
						//robot_console_printf("dxEmit: %f  dzEmit: %f\n", fabs(dxEmit), fabs(dzEmit));
						if (dxEmit >= 0) {
							speed[0] = 4000;
							speed[1] = -4000;
						}
						else {
							speed[0] = -4000;
							speed[1] = 4000;
						}
						//velAlign(speed); // alternative
					} else {
						speed[0] = 0;
						speed[1] = 0;
						if (robot_num == robot1) {
							state = WAIT;  // wait for robot 2 to align
						}
						if (robot_num == robot2) {
							state = APPROACH_ROBOT;  // start approach to robot 1
						}
					}
				}

			}
			/* fetch next packet */
			receiver_next_packet(commReceiver);

		}
		else {
			state = SEARCH_FOR_ROBOT;
			//robot_console_printf("ALIGN_WITH_ROBOT: Communication broken !\n");
		}
		break;


	case WAIT:
		// what if it receives other robots' messages?
		if (receiver_get_queue_length(commReceiver) > 0) {
			const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;
			speed[0] = 0;
			speed[1] = 0;
			if (robot_num == robot2 && char2state(recMessage[2])==WAIT) {
				state = ALIGN_WITH_ROBOT;  // robot 1 has finished aligning
			}
			if (robot_num == robot1 && char2state(recMessage[2])==APPROACH_ROBOT) {
				state = APPROACH_ROBOT;
			}
			/* fetch next packet */
			receiver_next_packet(commReceiver);
		}
		else {
			state = SEARCH_FOR_ROBOT;
			//robot_console_printf("WAIT: Communication broken !\n");
		}
		break;

	case APPROACH_ROBOT:
		speed[0] = 3000;
		speed[1] = 3000;

		if (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
			const void *buffer = receiver_get_data(commReceiver);
			const char *recMessage = (char*) buffer;

			// pieces have locked together; numbers are the resulting
			// 	conglomerate piece states
			if (strcmp(recMessage,"Lock5")==0 ||
				strcmp(recMessage,"Lock6")==0 ||
				strcmp(recMessage,"Lock7")==0 ||
				strcmp(recMessage,"Lock8")==0 ){
					// the piece that it's carrying
				speed[0] = 0;
				speed[1] = 0;
				// pieceType of robot not carrying piece will later go back to '0'
				pieceTypeNew = recMessage[4];
				state = SEPARATE_ROBOTS;
			}
			receiver_next_packet(commReceiver);
  		}

		break;

	case SEPARATE_ROBOTS:

		//speed[0] = 0;
		//speed[1] = 0;
		//if (receiver_get_queue_length(commReceiver) > 0) {
			/* Read current packet's data */
		//	const void *buffer = receiver_get_data(commReceiver);
		//	const char *recMessage = (char*) buffer;

		//	if (recMessage[0] == 'r' &&
		//	    char2state(recMessage[2])==SEPARATE_ROBOTS) {
					if (ctr == 0 &&
					    (pieceType == '2' || pieceType == '3' || pieceType == '5')) {
						// detach from magnet
						robot_console_printf("Detach-------\n");
						unlock = true;
					}
					if (ctr < 25) { //back up
						speed[0] = -8000;
						speed[1] = -8000;
						ctr += 1;
					}
					else if (ctr >= 20 && ctr < 45) {  // turn away from other robot
						speed[0] = -8000;
						speed[1] = 8000;
						ctr += 1;
					}
					else {
						ctr = 0;
						if (pieceType == '1' || pieceType == '4') { // ||
						    //pieceType == '6') { // hold onto piece
						    pieceType = pieceTypeNew;
							state = SEARCH_FOR_ROBOT;
						}
						else if (pieceType == '6') {
							pieceType = pieceTypeNew;
							float arm_position = servo_get_position(arm_servo);
				  			servo_set_position(arm_servo, arm_position-1.57);
				  			state = SEARCH_FOR_ROBOT;
						}
						else if (pieceType == '2' || pieceType == '3' ||
						         pieceType == '5') {
							state = SEARCH_FOR_PIECE;
						}
						else if (pieceType == '7') {  // puzzle is completed
							unlock = true;
							state = SEARCH_FOR_PIECE;
						}

					}
			//}
			//receiver_next_packet(commReceiver);
  		//}

		break;

	}


	/* Set the motor speeds */

  //robot_console_printf("Speeds: %d %d\n", (int) speed[0], (int) speed[1]);
	differential_wheels_set_speed((int) speed[0], (int) speed[1]);

	// Output the current metrics into the file
	//file_output();

	elapsed_time += (float)TIME_STEP / 1000.0;
	return TIME_STEP;           /* run for TIME_STEP ms */
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
