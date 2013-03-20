/*
 * File:         platform.c
 * Date:         June 2008
 * Description:  Controller for shacking of pieces.
 * Author:       Loic Matthey
 * Modifications:
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <device/robot.h>
#include <device/servo.h>

#define KEYB_OFF 2

#define TIME_STEP 32


static DeviceTag platform_servo;
float elapsed_time = 0.0;


/* Handle the keyboard actions */
void keyboard() {
    int key;
    key = robot_keyboard_get_key();

    // if (key != 0) {
    // 	//robot_console_printf("key: %d\n", key+KEYB_OFF);
    // }

    // if (key == (317-KEYB_OFF)) { // arrow up
    // 		speed_bias[0] += SPEED_INCR;
    // 		speed_bias[1] += SPEED_INCR;
    // 		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
    //     } else if(key == (319-KEYB_OFF)) { // arrow down
    // 		speed_bias[0] -= SPEED_INCR;
    // 		speed_bias[1] -= SPEED_INCR;
    // 		// speed[0] = speed[1] = 0.0;
    // 		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
    //     } else if(key == (316-KEYB_OFF)) { // arrow left
    // 		speed_bias[0] -= SPEED_INCR/2;
    // 		speed_bias[1] += SPEED_INCR/2;
    // 		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
    //     } else if(key == (318-KEYB_OFF)) { // arrow right
    // 		speed_bias[0] += SPEED_INCR/2;
    // 		speed_bias[1] -= SPEED_INCR/2;
    // 		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
    //     } else if(key == (34-KEYB_OFF)) { // Space
    // 		speed_bias[0] = speed_bias[1] = 0.0;
    // 		// //robot_console_printf("Speed : %f %f\n", speed[0], speed[1]);
    //     } else 	if(key == (70-KEYB_OFF)) { // D
    // 		if (!unlock && (connector_status[0] || connector_status[1])){
    // 	    	unlock = true;
    // 	    	//robot_console_printf("Unlocking\n");
    // 		}
    //     } else if(key == (80-KEYB_OFF)) { // N
    // 		braitenberg = false;
    // 		//robot_console_printf("No braitenberg\n");
    //     } else	if(key == (68-KEYB_OFF)) { // B
    // 		braitenberg = true;
    // 		//robot_console_printf("Braitenberg\n");
    //     }
}


static int run(int ms)
{
    // int i;
   
    // Update the keyboard
    // keyboard();

    
	// float platform_position = servo_get_position(platform_servo);
	
	float platform_amplitude = 0.1;
	float platform_omega = 1./10.;
	
	servo_set_position(platform_servo, platform_amplitude*sin(elapsed_time*2.0*M_PI*platform_omega));


    elapsed_time += (float)TIME_STEP / 1000.0;
    return TIME_STEP;           /* run for TIME_STEP ms */
}

//================================================================================================================================================
static void reset(void) {
	
	// int i,j;
	
	// Enable keyboard
	robot_keyboard_enable(TIME_STEP);

	// Enable the Arm servo
	platform_servo = robot_get_device("platform_servo");
	servo_enable_position(platform_servo, TIME_STEP);

	robot_console_printf("Moving platform resetted.");

	srand(time(NULL));

	return;
}

int main() {
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
