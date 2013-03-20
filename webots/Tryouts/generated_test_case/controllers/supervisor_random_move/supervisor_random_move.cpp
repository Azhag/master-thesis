/*
 * File:         supervisor_random_move.cpp
 * Date:         April 10th, 2008
 * Description:  Supervisor for Puzzle test case. Randomly moves pieces around.
 * Author:       Loic Matthey
 *
 * Copyright (c) 2008 Loic Matthey
 */

#include <stdlib.h>
#include <string.h>
#include <device/robot.h>
#include <device/supervisor.h>
#include <device/emitter.h>
#include <time.h>

#define TIME_STEP 64
#define ROBOTS 4           /* number of robots */
#define DIVIDER_MOVES 10
#define SIZE_ARENA 1.5

static const char *robot_name[ROBOTS] =
    { "P1_1", "P2_1", "P3_1", "P4_1"};
static NodeRef robot[ROBOTS];
static DeviceTag emitter;
static float position[ROBOTS * 4 + 2];
static float *buffer;
int nb_moves = DIVIDER_MOVES;

static void reset(void)
{
    int i;

    emitter = robot_get_device("emitter");
    buffer = (float *) emitter_get_buffer(emitter);

    /*
     * We try to get a handler to the robots, if this was successful we keep 
     * a track of some of the fields of the robot and we set its controller.
     */
    for (i = 0; i < ROBOTS; i++) {
        robot[i] = supervisor_node_get_from_def(robot_name[i]);

        /*
         * The call to this function is mandatory for the function
         * supervisor_node_was_found to be able to work correctly.
         */
        robot_step(0);

        if (supervisor_node_was_found(robot[i]) == 1) {
            supervisor_field_get(robot[i],
                                 SUPERVISOR_FIELD_TRANSLATION_X |
                                 SUPERVISOR_FIELD_TRANSLATION_Z |
                                 SUPERVISOR_FIELD_ROTATION_Y |
                                 SUPERVISOR_FIELD_ROTATION_ANGLE,
                                 &position[i * 4], TIME_STEP);
        } else {
            robot_console_printf("Error: node %s not found\n", robot_name[i]);
        }

    }
	srand(time(NULL));

    return;
}

int sign(float x) {
	return x >= 0 ? 1 : -1;
}

static int run(int ms)
{
    int i;
	float dx, dy;
	float next_position[2];
	
    for (i = 0; i < ROBOTS; i++) {
        buffer[3 * i] = position[4 * i];    /* robot i: X */
        buffer[3 * i + 1] = position[4 * i + 1];    /* robot i: Z */
        if (position[4 * i + 2] > 0) {
            buffer[3 * i + 2] = position[4 * i + 3];    /* robot i: alpha */
        } else {
            /* We test if Ry axis was inverted */
            buffer[3 * i + 2] = -position[4 * i + 3];
        }
    }

    /* We send the positioning datas to the robots. */
    emitter_send(emitter, (ROBOTS * 3 + 2) * sizeof(float));

	// New positions for the robots
	if (nb_moves <= 0) {
		for (i = 0; i < ROBOTS; i++) {
			// Random movement 
			dx = 0.05*((rand() / (float)RAND_MAX)-0.5);
			dy = 0.05*((rand() / (float)RAND_MAX) -0.5);


			next_position[0] = position[4*i] + dx;
			next_position[1] = position[4*i + 1] + dy;

			if (next_position[0] > SIZE_ARENA || next_position[0] < -SIZE_ARENA) {
				next_position[0] = sign(next_position[0])*(SIZE_ARENA - 0.1);
			}
			if (next_position[1] > SIZE_ARENA || next_position[1] < -SIZE_ARENA) {
				next_position[1] = sign(next_position[1])*(SIZE_ARENA - 0.1);
			}

			//robot_console_printf("dx: %f, dy: %f, next_pos: %f %f\n", dx, dy, next_position[0], next_position[1]);

	    	supervisor_field_set(robot[i],
	        					 SUPERVISOR_FIELD_TRANSLATION_X |
								 SUPERVISOR_FIELD_TRANSLATION_Z,
								 next_position);
		}
		nb_moves = DIVIDER_MOVES;
	}
	nb_moves --;

    return TIME_STEP;
}

int main()
{
    robot_live(reset);
    robot_run(run);             /* never returns */

    return 0;
}
