/*
* File:         robot_measuredensity_chemotaxis.cpp
* Date:         May 2008
* Description:  Controller measuring the density of population, chemotaxis movement.
* Author:       Loïc Matthey
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
char pieceType;  // type of piece the robot is carrying
char pieceNum;   // number of the piece of a type that the robot is carrying

int connector_status[MAX_CONNECTOR_NUMBER];
bool unlock = false;
bool braitenberg = true;

float speed_bias[2];

FILE* out_file;
float elapsed_time = 0.0;

// robot states
enum { SEARCH_FOR_PIECE, ALIGN_WITH_PIECE, APPROACH_PIECE, ROTATE_PIECE, SEARCH_FOR_ROBOT,
	ALIGN_WITH_ROBOT, WAIT, APPROACH_ROBOT };
int state = SEARCH_FOR_PIECE;
// SEARCH_FOR_ROBOT: Robot looks for an appropriate piece
// ALIGN_WITH_ROBOT: Robot aligns with another robot with complementary piece
// WAIT: Robot waits for other robot to align


/**
* The Braitenberg algorithm is really simple, it simply computes the
	*  speed of each wheel by summing the value of each sensor multiplied by
	*  its corresponding weight. That is why each sensor must have a weight
	*  for each wheel.
*/
void velBraitenberg(unsigned short *sensVal, float *speed)
{
	int i, j;

	for (i = 0; i < 2; i++) {
		speed[i] = speed_bias[i];

		if (braitenberg) {
			for (j = 0; j < sensor_number; j++) {
				// We need to recenter the value of the sensor to be able to get
				// 	negative values too. This will allow the wheels to go
				// 	backward too.
				
				speed[i] += 3.0 * matrix[j][i] * (1 - ((float) sensVal[j] / range));
			}
		}
	}
	return;
}

/* Output the signals for matlab */
void file_output() 
{
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
	// fprintf(out_file, "%f ", 1.0);

	// Position
	const float* matrix = gps_get_matrix(gps);
	fprintf(out_file, "%f %f", gps_position_x(matrix), gps_position_z(matrix));
	// robot_console_printf("%f %f", gps_position_x(matrix), gps_position_z(matrix));

	fprintf(out_file, "\n");

	fflush(out_file);
}

/* Handle the keyboard actions */
void keyboard() 
{
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

	// If the channel is not the good one, we change it. 
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
	int i;
	unsigned short sensors_value[MAX_SENSOR_NUMBER];
	float speed[2];

	// Update the keyboard
	keyboard();

	// Update the distance sensors
	for (i = 0; i < sensor_number; i++) {
		sensors_value[i] = distance_sensor_get_value(sensors[i]);
	}

	// Braitenberg obstacle avoidance
	velBraitenberg(sensors_value, speed);



	/////*********** IF NEW PIECE
	// A new piece is done, output the current time
	file_output();

	/* Set the motor speeds */

//robot_console_printf("Speeds: %d %d\n", (int) speed[0], (int) speed[1]);
	differential_wheels_set_speed((int) speed[0], (int) speed[1]);

	elapsed_time += (float)TIME_STEP / 1000.0;
	return TIME_STEP;           /* run for TIME_STEP ms */
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}