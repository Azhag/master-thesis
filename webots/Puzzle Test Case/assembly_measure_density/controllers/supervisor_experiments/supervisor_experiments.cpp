/*
 * File:         supervisor_experiments.cpp
 * Date:         April 10th, 2008
 * Description:  Supervisor for Puzzle test case. Restarts the experiments several times.
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

#define TIME_STEP 32

// Write down the number of experiments desired.
#define NB_EXPERIMENTS 5

// Simulation time in seconds
#define TIME_LIMIT 600

// Activate the experiments
#define ACTIVATE_EXPERIMENTS 1

static DeviceTag emitter;
static float *buffer;
char time_string[64];

float elapsed_time = 0.0;
int current_experiment = 0;
FILE* file_state;

static void reset(void)
{
	char saved_state[10];
	
    emitter = robot_get_device("emitter");
    buffer = (float *) emitter_get_buffer(emitter);

	// We need to save and reload the experiments state in a File, because the supervisor is resetted also.
	file_state = fopen("supervisor_state", "r+");
	if (file_state) {
		fgets(saved_state, 10, file_state);
		current_experiment = atoi(saved_state);
		
		// We'll overwrite directly on the same position
		fseek(file_state, 0, SEEK_SET);
	} else {
		file_state = fopen("supervisor_state", "w");
	}
	
	srand(time(NULL));

    return;
}

void run_supervisor() {
	// Actual simulation
	
	elapsed_time += (float)TIME_STEP / 1000.0;

	// Showing Simulation Clock
	sprintf(time_string, "%02d:%02d", (int) (elapsed_time / 60), (int) elapsed_time % 60);
	supervisor_set_label(0, time_string, 0.06, 0.01, 0.07, 0x000000);
	
	/*
		TODO Possibility to output other statistics, by sending informations to robots for example. For now it's on the robot side.
	*/
}

void run_experiments()
{
	// Current experiment
	robot_console_printf("Experiment number %d out of %d\n", current_experiment+1, NB_EXPERIMENTS);
	
	// Reset the experiment variables
	elapsed_time = 0.0;
	
	// Simulate for the desired time
	while(elapsed_time < TIME_LIMIT){
		run_supervisor();
		
		robot_step(TIME_STEP);
	}
	
	// Save the current supervisor state
	current_experiment++;
	fprintf(file_state, "%d", current_experiment);
	fclose(file_state);
	
	// Reset the simulation if needed
	if (current_experiment < NB_EXPERIMENTS) {
		supervisor_simulation_revert();
		robot_step(TIME_STEP);
	} else {
		// Experiments finished
		remove("supervisor_state");
		
		// supervisor_simulation_quit();
		// robot_step(TIME_STEP);
	}
}

static int run(int time) {
	run_supervisor();
	
	return TIME_STEP;
}

int main()
{
    robot_live(reset);

	if (ACTIVATE_EXPERIMENTS) {
    	run_experiments(); // Run the experiments		
	} else {
		robot_run(run);
	}


    return 0;
}
