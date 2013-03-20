/*
* File:         locking_simple.c
	* Date:         March 7th 2008
	* Description:  Controller locking the connectors if available.
	* Author:       Loic Matthey
	* Modifications: 
*
*/

#include <string.h>
#include <device/robot.h>
#include <device/differential_wheels.h>
#include <device/distance_sensor.h>
#include <device/connector.h>

#define KEYB_OFF 2

#define TIME_STEP 64
#define MAX_SENSOR_NUMBER 16
#define MAX_CONNECTOR_NUMBER 2
#define RANGE (1024 / 2)

#define UNLOCKED 0
#define LOCKED 1

#define SPEED_INCR 50.0

static DeviceTag sensors[MAX_SENSOR_NUMBER];
static DeviceTag connectors[MAX_CONNECTOR_NUMBER];
static float matrix[MAX_SENSOR_NUMBER][2];
static int sensor_number;
static int range;
const char *robot_name;

int connector_status[MAX_CONNECTOR_NUMBER];
bool unlock = false;
bool braitenberg = true;

float speed_bias[2];

/* Handle the keyboard actions */
void keyboard() {
	int key;
	key = robot_keyboard_get_key();
	
	// if (key != 0) {
	// 	robot_console_printf("key: %d\n", key+KEYB_OFF);
	// }
	
	if (key == (317-KEYB_OFF)) { // arrow up
		speed_bias[0] += SPEED_INCR;	
		speed_bias[1] += SPEED_INCR;
		// robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (319-KEYB_OFF)) { // arrow down
		speed_bias[0] -= SPEED_INCR;	
		speed_bias[1] -= SPEED_INCR;
		// speed[0] = speed[1] = 0.0;
		// robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (316-KEYB_OFF)) { // arrow left
		speed_bias[0] -= SPEED_INCR/2;	
		speed_bias[1] += SPEED_INCR/2;
		// robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (318-KEYB_OFF)) { // arrow right
		speed_bias[0] += SPEED_INCR/2;	
		speed_bias[1] -= SPEED_INCR/2;
		// robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else if(key == (34-KEYB_OFF)) { // Space
		speed_bias[0] = speed_bias[1] = 0.0;
		// robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
	} else 	if(key == (70-KEYB_OFF)) { // D
		if (!unlock && (connector_status[0] || connector_status[1])){
			unlock = true;
			robot_console_printf("Unlocking\n");			
		}
	} else if(key == (80-KEYB_OFF)) { // N
		braitenberg = false;
		robot_console_printf("No braitenberg\n");
	} else	if(key == (68-KEYB_OFF)) { // B
		braitenberg = true;
		robot_console_printf("Braitenberg\n");
	}
}

static void reset(void)
{
	int i,j;
	robot_name = robot_get_name();

	char connector_name[15];
		
	float khepera3_matrix[9][2] = 
		{ {0, 0}, {-20000, 20000}, {-50000, 50000}, {-70000, 70000}, {70000, -70000},
		{50000, -50000}, {20000, -20000}, {0, 0}, {0, 0} };
	// { {-5000, -5000}, {-20000, 40000}, {-30000, 50000}, {-70000, 70000}, {70000, -60000},
	// {50000, -40000}, {40000, -20000}, {-5000, -5000}, {-10000, -10000} };
	char sensors_name[12];
	// float (*temp_matrix)[2];

	range = RANGE;
	
	sensor_number = 9;
	sprintf(sensors_name, "%s_ds0", robot_name);
	range = 2000;

	// Enable the Distance sensors and Braitenberg weights
	for (i = 0; i < sensor_number; i++) {
		sensors[i] = robot_get_device(sensors_name);
		distance_sensor_enable(sensors[i], TIME_STEP);

		if ((i + 1) >= 10) {
			sensors_name[7] = '1';
			sensors_name[8]++;

			if ((i + 1) == 10) {
				sensors_name[8] = '0';
				sensors_name[9] = (char) '\0';
			}
		} else {
			sensors_name[7]++;
		}

		for (j = 0; j < 2; j++) {
			matrix[i][j] = khepera3_matrix[i][j];
		}
	}

	// Enable the Connectors
	for (i = 0; i< MAX_CONNECTOR_NUMBER; i++) {
		// Construct the names of connectors
		sprintf(connector_name, "%s_con%d", robot_name, i+1);
		robot_console_printf("connector %d: %s \n", i, connector_name);
		
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
	speed_bias[0] = 0.0;
	speed_bias[1] = 0.0;
	
	// Enable keyboard
	robot_keyboard_enable(TIME_STEP);
	

	robot_console_printf("The %s robot is reset, it uses %d sensors\n",
		robot_name, sensor_number);

	return;
}

static int run(int ms)
{
	int i,j;
	unsigned short sensors_value[MAX_SENSOR_NUMBER];
	int connector_presence[MAX_CONNECTOR_NUMBER];
	float speed[2];
	
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
	//robot_console_printf("Connectors : %d %d\n", connector_presence[0], connector_presence[1]);
	
	// Lock automatically
	if (!unlock && connector_presence[0] && connector_presence[1] && 
		!connector_status[0] && !connector_status[1]) {
		//Should lock
		robot_console_printf("Lock !\n");
		connector_lock(connectors[0]);
		connector_lock(connectors[1]);
		connector_status[0] = LOCKED;
		connector_status[1] = LOCKED;
	}
	
	// Unlock requested
	if (unlock) {
		connector_unlock(connectors[0]);
		connector_unlock(connectors[1]);
		connector_status[0] = UNLOCKED;
		connector_status[1] = UNLOCKED;
	}
	
	// Allow locking when out of presence
	if (unlock && !connector_presence[0] && !connector_presence[1]) {
		unlock= !unlock;
	}
	
	// The Braitenberg algorithm is really simple, it simply computes the
	// 	speed of each wheel by summing the value of each sensor multiplied by
	// 	its corresponding weight. That is why each sensor must have a weight 
	// 	for each wheel.
	
	for (i = 0; i < 2; i++) {
		speed[i] = speed_bias[i];
		
		if (braitenberg) {
			for (j = 0; j < sensor_number; j++) {
			/* 
			* We need to recenter the value of the sensor to be able to get
			* negative values too. This will allow the wheels to go 
			* backward too.
			*/
			//robot_console_printf("%f ",(float) sensors_value[j]);
			
			speed[i] +=  matrix[j][i] * (1 - ((float) sensors_value[j] / range));
			}			
		}
	}

	/* Set the motor speeds */
	differential_wheels_set_speed((int) speed[0], (int) speed[1]);

	return TIME_STEP;           /* run for TIME_STEP ms */
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
