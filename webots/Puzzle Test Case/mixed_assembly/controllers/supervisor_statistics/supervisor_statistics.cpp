/*
 * File:         supervisor_statistics.cpp
 * Date:         June 12th, 2008
 * Description:  Supervisor for Puzzle Test Case. Receive messages for statistics purpose.
 * Author:       Loic Matthey
 *
 * Copyright (c) 2008 Loic Matthey
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <device/robot.h>
#include <device/supervisor.h>
#include <device/receiver.h>
#include <time.h>
#include <vector>

#define TIME_STEP 32

// Activate the experiments
#define ACTIVATE_EXPERIMENTS 1

// Activate the statistics
#define ACTIVATE_STATISTICS 0

// Write down the number of experiments desired.
#define NB_EXPERIMENTS 100
// Simulation time in seconds
#define TIME_LIMIT (60*60)

#define MAX_PIECE_STUCK 500


//======== CHOOSE THE EXPERIMENT HERE =============
 #define EXP1PUZZLE
// #define EXP3PUZZLE
// #define EXP3PUZZLE10ROBOTS

#ifdef EXP1PUZZLE
	// The number of pieces that can be assembled maximum
	#define NB_ASSEMBLIES 1
	// The number of robots
	#define NB_ROBOTS 0
	// The number of pieces
	#define NB_PIECES 5
	// The arena radius
	#define ARENA_RADIUS 2*0.95
	// Should prevent stucks
	#define STUCK_PREVENT false
#endif
#ifdef EXP3PUZZLE
	// The number of pieces that can be assembled maximum
	#define NB_ASSEMBLIES 3
	// The number of robots
	#define NB_ROBOTS 0
	// The number of pieces
	#define NB_PIECES 15
	// The arena radius
	#define ARENA_RADIUS 3*0.95
#endif
#ifdef EXP3PUZZLE10ROBOTS
	// The number of pieces that can be assembled maximum
	#define NB_ASSEMBLIES 3
	// The number of robots
	#define NB_ROBOTS 0
	// The number of pieces
	#define NB_PIECES 15
	// The arena radius
	#define ARENA_RADIUS 3*0.95
#endif
	
// The number of types of pieces
#define NB_PIECES_TYPES 4

// Channel for communicating with robots and pieces
#define SUPERVISOR_CHANNEL 2

using namespace std;

static DeviceTag receiver;
char time_string[64];

float elapsed_time = 0.0;
int current_experiment = 0;
FILE* file_state;
FILE* file_output;

int nbAssemblies = 0;

NodeRef robotsNodeRefList[NB_ROBOTS];
NodeRef piecesNodeRefList[NB_PIECES];
vector<pair<float, float> > usedPositions;
float pieces_positions[NB_PIECES][7];
float piece_old_positions[NB_PIECES][2];
int cnt_piece_stuck[NB_PIECES];

void parseMsgFinalAssembly(char* recMessage, float* time_encountering, int* is_assembly) {
	// Parse the message, format:
	// robot: 0 robot_num state piece_type piece_num configuration_complete configuration_other_assembling
	// piece: 1 | piece_type | piece_num | state | carrying_robot_num | angle
	int i=0;
	
	char* parsedMsg = strtok(recMessage," ");
	
	// First is a float, pass it
	*time_encountering = atof(parsedMsg);
	parsedMsg = strtok(NULL, " ");
	
	// Check possible final assemblies
	*is_assembly = 0;
	while(i< 15 && parsedMsg != NULL) {
		if (i == 12 || i == 14) {
			// 1 here means a new final assembly
			*is_assembly += atoi(parsedMsg);
		}
		parsedMsg = strtok(NULL, " ");
		i++;
	}
}


// Return true if the position is inside the Hexagon arena
bool isInsideArena(float x, float y) {
	return (sqrt(pow(x,2.0)+pow(y,2.0)) <= ARENA_RADIUS*sin(M_PI/3.0));
}

void vrml_to_rotation_3x3(float rx, float ry, float rz, float alpha, float m[9]) {
	float c = cos(alpha);
	float s = sin(alpha);

	float t1 = 1.0 - c;
	float t2 = rx * rz * t1;
	float t3 = rx * ry * t1;
	float t4 = ry * rz * t1;

	m[0] = rx * rx * t1 + c;
	m[3] = t3 - rz * s;
	m[6] = t2 + ry * s;
	m[1] = t3 + rz * s;
	m[4] = ry * ry * t1 + c;
	m[7] = t4 - rx * s;
	m[2] = t2 - ry * s;
	m[5] = t4 + rx * s;
	m[8] = rz * rz * t1 + c;
}

void run_supervisor() {
	char *recMessage;
	char toParseMsg[50];
	int new_assembly_finished = 0;
	float delta_time_encountering = 0.0;
	float last_time = 0.0;
	float new_pos[4];
	float piece_rotation[9];
	
	// Showing Simulation Clock
	sprintf(time_string, "%02d:%02d", (int) (elapsed_time / 60), (int) elapsed_time % 60);
	supervisor_set_label(0, time_string, 0.06, 0.01, 0.07, 0x000000);
	
	/*
		TODO Possibility to output other statistics, by sending informations to robots for example. For now it's on the robot side.
	*/
	if (ACTIVATE_STATISTICS && receiver_get_queue_length(receiver) > 0) {
	   	// flush the queue
		const void *buffer = receiver_get_data(receiver);
		recMessage = (char*) buffer;

		strcpy(toParseMsg,recMessage);
		
		parseMsgFinalAssembly(toParseMsg, &delta_time_encountering, &new_assembly_finished);
		
		// Check if we overlap with the previous time (which is bad for analysis)
		float new_time = elapsed_time-delta_time_encountering;
		if (new_time == last_time) {
			new_time += (float)TIME_STEP/1000.0;
		}
		
		// Print the received message in the file
		// Format:
		// time reaction_occured {populations change}
		// robot_console_printf("%.3f %.3f\n", new_time, elapsed_time);
		fprintf(file_output, "%.5f %s\n", new_time, recMessage);
		
		nbAssemblies += new_assembly_finished;
		
		new_assembly_finished = 0;
		last_time = new_time;
		
	   	receiver_next_packet(receiver);
	}
	
	// Make sure that the pieces are not upside down or stuck in the walls
	// Iterate over all pieces
	for(int i = 0; i < NB_PIECES; ++i) {
		
		// Check if the piece is stuck
		if (fabs(pieces_positions[i][0] - piece_old_positions[i][0]) < 0.001 && 
			fabs(pieces_positions[i][2] - piece_old_positions[i][1]) < 0.001) {
			
			cnt_piece_stuck[i]++;
		} else {
			cnt_piece_stuck[i]=0;
		}
		if (STUCK_PREVENT && cnt_piece_stuck[i] > MAX_PIECE_STUCK) {
			// robot_console_printf("Piece %d stuck, resetting position.\n", i+1);
			
			new_pos[0] = 0.5*(rand()/(RAND_MAX+1.0)-0.5);
			new_pos[1] = 0.5*(rand()/(RAND_MAX+1.0)-0.5);
			new_pos[2] = 2*M_PI*(rand()/(RAND_MAX+1.0));
			
			supervisor_field_set(piecesNodeRefList[i], SUPERVISOR_FIELD_TRANSLATION_X |
					                               SUPERVISOR_FIELD_TRANSLATION_Z |
					                               SUPERVISOR_FIELD_ROTATION_ANGLE,
					                               new_pos);
		}
		
		// Check if piece outside arena
		if (sqrt(pow(pieces_positions[i][0],2.0)+pow(pieces_positions[i][2],2.0)) > ARENA_RADIUS/0.7*sin(M_PI/3.0)){
			// Outside...
			
			new_pos[0] = 0.25*(rand()/(RAND_MAX+1.0)-0.5);
			new_pos[1] = 0.25*(rand()/(RAND_MAX+1.0)-0.5);
			new_pos[2] = 2*M_PI*(rand()/(RAND_MAX+1.0));
			
			supervisor_field_set(piecesNodeRefList[i], SUPERVISOR_FIELD_TRANSLATION_X |
					                               SUPERVISOR_FIELD_TRANSLATION_Z |
					                               SUPERVISOR_FIELD_ROTATION_ANGLE,
					                               new_pos);
		}
		
		
		
		// Check the orientation of the piece
		vrml_to_rotation_3x3(pieces_positions[i][3], pieces_positions[i][4], pieces_positions[i][5], pieces_positions[i][6], piece_rotation);
		if (piece_rotation[4] < -0.5) { // This should be 1 if correct orientation
			// robot_console_printf("Piece %d upside down !\n", i+1);
			
			new_pos[0] = 0.0;
			new_pos[1] = 1.0;
			new_pos[2] = 0.0;
			new_pos[3] = 2*M_PI*(rand()/(RAND_MAX+1.0));
			
			supervisor_field_set(piecesNodeRefList[i], SUPERVISOR_FIELD_ROTATION,
					                               new_pos);
		}
		
		piece_old_positions[i][0] = pieces_positions[i][0];
		piece_old_positions[i][1] = pieces_positions[i][2];
	}
	
	elapsed_time += (float)TIME_STEP / 1000.0;
}

// Return true if the position is already in used by another object (small occupancy radius around the positions)
bool isPositionUsed(float x, float y) {
	float occupancy_radius = 0.1;
	
	for (vector<pair<float, float> >::iterator it = usedPositions.begin(); it!=usedPositions.end(); ++it) {
		// *it = pair<float, float>. it->first = x, it->second = y
		if( (x >= (it->first - occupancy_radius) && x <= (it->first +occupancy_radius)) &&
			(y >= (it->second - occupancy_radius) && y <= (it->second +occupancy_radius))
		) {
			// We are into an occupied position, reject
			return true;
		}
	}
	
	return false;
}

// Put the robots and pieces at random positions in the arena
void replaceRobotsAndPiecesRandomly() {
	float x, y, theta;
	float new_pos[3];
	
	// Clear positions
	usedPositions.clear();
	
	// Iterate over all robots
	for(int i = 0; i < NB_ROBOTS; ++i) {
		
		// Pick a random position not used
		do {
			x = ARENA_RADIUS*2.0*(rand()/(RAND_MAX+1.0)-0.5);
			y = ARENA_RADIUS*2.0*(rand()/(RAND_MAX+1.0)-0.5);
			theta = 2*M_PI*(rand()/(RAND_MAX+1.0));			
		}while(!isInsideArena(x,y) || isPositionUsed(x,y));
		
		new_pos[0] = x;
		new_pos[1] = y;
		new_pos[2] = theta;
		
		// robot_console_printf("New position for robot %d: %f, %f, %f\n", i, x, y, theta);
		// Put the robot at this position
		supervisor_field_set(robotsNodeRefList[i], SUPERVISOR_FIELD_TRANSLATION_X |
		                               SUPERVISOR_FIELD_TRANSLATION_Z |
		                               SUPERVISOR_FIELD_ROTATION_ANGLE,
		                               new_pos);
		
		// Fill the position used list
		usedPositions.push_back(pair<float, float> (x,y));
	}
	
	// Iterate over all pieces
	for(int i = 0; i < NB_PIECES; ++i) {
		
		// Pick a random position not used
		do {
			x = ARENA_RADIUS*2.0*(rand()/(RAND_MAX+1.0)-0.5);
			y = ARENA_RADIUS*2.0*(rand()/(RAND_MAX+1.0)-0.5);
			theta = 2*M_PI*(rand()/(RAND_MAX+1.0));			
		}while(!isInsideArena(x,y) || isPositionUsed(x,y));
		
		new_pos[0] = x;
		new_pos[1] = y;
		new_pos[2] = theta;
		
		// robot_console_printf("New position for piece %d: %f, %f, %f\n", i, x, y, theta);
		// Put the robot at this position
		supervisor_field_set(piecesNodeRefList[i], SUPERVISOR_FIELD_TRANSLATION_X |
		                               SUPERVISOR_FIELD_TRANSLATION_Z |
		                               SUPERVISOR_FIELD_ROTATION_ANGLE,
		                               new_pos);
		
		// Fill the position used list
		usedPositions.push_back(pair<float, float> (x,y));
	}
}


void run_experiments()
{
	// Current experiment
	robot_console_printf("Experiment number %d out of %d\n", current_experiment+1, NB_EXPERIMENTS);
	
	// Reset the experiment variables
	elapsed_time = 0.0;
	
	// Replace the robots and the pieces
	replaceRobotsAndPiecesRandomly();
		
	while(elapsed_time <= TIME_LIMIT && nbAssemblies < NB_ASSEMBLIES){
		// ======= Simulate for the desired time =====
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

// ======================================================================================================

// Fill the list of robots NodeRef
void populateRobotList() {
	char robotDefName[50];
	NodeRef currPieceRef;
	
	// Find all the robots
	for(int i = 0; i < NB_ROBOTS; ++i) {
		sprintf(robotDefName, "KHEPERA3_%d", i+1);
		
		// Get the NodeRef
		currPieceRef = supervisor_node_get_from_def(robotDefName);
		
		// Check if there was one
		robot_step(0);
		if (supervisor_node_was_found(currPieceRef)) {
			// Yeah, populate
			robotsNodeRefList[i] = currPieceRef;
			
			// robot_console_printf("Robot %s known, %d\n", robotDefName, i);
		} else {
			robot_console_printf("Wrong number of robots, check the configuration in Supervisor_Statistics !\n");
		}
	}
	
	// if (robotsNodeRefList.size() != NB_ROBOTS) {
	// 		// There's a problem here...
	// 		robot_console_printf("Wrong number of robots, check the configuration in Supervisor_Statistics !\n");
	// 	}
}

void populatePiecesList() {
	char pieceDefName[50];
	NodeRef currPieceRef;
	
	// Find all the pieces
	for(int i = 0; i < NB_PIECES; ++i) {		
		// We have to try several names, because we don't know exactly the numerotations
		sprintf(pieceDefName, "P_%d", i+1);
				
		// Get the NodeRef
		currPieceRef = supervisor_node_get_from_def(pieceDefName);
				
		// Check if there was one
		robot_step(0);
		if (supervisor_node_was_found(currPieceRef)) {
			// Yeah, populate
			piecesNodeRefList[i] = currPieceRef;
			
			supervisor_field_get(piecesNodeRefList[i], SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, pieces_positions[i], TIME_STEP);
			
			piece_old_positions[i][0] = 0.0;
			piece_old_positions[i][1] = 0.0;
			cnt_piece_stuck[i] = 0;
		}
	}
	
	// if (piecesNodeRefList.size() != NB_PIECES) {
	// 		// There's a problem here...
	// 		robot_console_printf("Wrong number of pieces, check the configuration in Supervisor_Statistics !\n");
	// 	}
}

static void reset(void)
{
	char saved_state[10];
	char filename[20];
	int nb_file=0;
	
    receiver = robot_get_device("receiver");
	receiver_enable(receiver, TIME_STEP);
	
	/* If the channel is not the good one, we change it. */
	int channel = receiver_get_channel(receiver);
	if (channel != SUPERVISOR_CHANNEL) {
	    receiver_set_channel(receiver, SUPERVISOR_CHANNEL);
	}

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
	
	// Open the file for the statistics, with a filename that doesn't exists yet
	if (ACTIVATE_STATISTICS) {
		// We'll open a file that doesn't exist for now.
		sprintf(filename, "output_values_%d.txt", nb_file);

		file_output = fopen(filename, "r");
		while (file_output) {
			// The file already exists, get another file name
			fclose(file_output);
			nb_file ++;
			sprintf(filename, "output_values_%d.txt", nb_file);
			file_output = fopen(filename, "r");
		}
		file_output = fopen(filename, "w");
		
	}
	
	srand(time(NULL));
	
	robot_step(TIME_STEP);
	
	// Find out the robots present
	populateRobotList();
	
	// Find out the pieces present
	populatePiecesList();
	
    return;
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
