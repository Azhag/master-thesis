/*
* File:         piece_simple.c
	* Date:         March 7th 2008
	* Description:  Controller for pieces, locking the connectors if available.
	* Author:       Loic Matthey
	* Modifications: 
*
*/

#include <string.h>
#include <device/robot.h>
#include <device/connector.h>

#define KEYB_OFF 2

#define TIME_STEP 64

#define UNLOCKED 0
#define LOCKED 1

#define MAX_COUNT_BADLOCK 100
#define MAX_COUNT_UNLOCK 100

#define LIM_CONNECTORS 10
const char *robot_name;

static DeviceTag connectors[LIM_CONNECTORS];

int connector_status[LIM_CONNECTORS];
int connector_presence[LIM_CONNECTORS];
bool unlock[LIM_CONNECTORS];
int count_badlock[LIM_CONNECTORS];
int count_unlock[LIM_CONNECTORS];

int piece_type_nb_connectors[] = {10, 8, 5, 8};
int max_connector_number = 0;

/* Handle the keyboard actions */
void keyboard() {
	int key;
	key = robot_keyboard_get_key();
	
	// if (key != 0) {
	// 		robot_console_printf("key: %d\n", key);
	// 	}
	
	if(key == (69-KEYB_OFF)) { // C
		for (int i=0; i< max_connector_number; i++){
			if (!unlock[i] && connector_status[i] == LOCKED){
				unlock[i] = true;
				robot_console_printf("Piece unlock %d\n", i);
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
	}
}

static void reset(void)
{
	int i;
	robot_name = robot_get_name();
	char connector_name[10];

	// Enable the Connectors
	
	// connectors[0] = robot_get_device("con1");
	// 	connectors[1] = robot_get_device("con2");
	// 	connector_enable_presence(connectors[0], TIME_STEP);
	// 	connector_enable_presence(connectors[1], TIME_STEP);
	int piece_type = robot_name[1] - '0' -1;
	max_connector_number = piece_type_nb_connectors[piece_type];
	
	//robot_console_printf("======= %d, %d\n", piece_type, nb_connectors);
	for (i = 0; i< max_connector_number; i++) {
		// Construct the names of connectors
		//sprintf(connector_name, "%s_c%d", robot_name, 2*i+1);
		sprintf(connector_name, "c%d", i+1);
		robot_console_printf("connector %d: %s \n", i+1, connector_name);
		
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


	robot_console_printf("The %s robot is reset, it uses %d connectors\n",
		robot_name, max_connector_number);

	return;
}

static int run(int ms)
{
	int i;
	
	// Handle keyboard
	keyboard();
	
	for (i = 0; i < max_connector_number; i++) {
		connector_presence[i] = connector_get_presence(connectors[i]);
	}
	//robot_console_printf("Connectors : %d %d\n", connector_presence[0], connector_presence[1]);
	
	// Lock automatically
	for (i = 0; i < max_connector_number; i++) {
		//Should lock automatically
		if (!unlock[i] && connector_presence[i] &&
			!connector_status[i]) {
			robot_console_printf("Lock %d !\n", i);
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
				robot_console_printf("Unlock %d..\n", i);
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
				robot_console_printf("Bad position %d %d\n", i, count_badlock[i]);
			}
		} else {
			count_badlock[i] = 0;			
		}
		
		// if (!unlock && connector_presence[2*i] && connector_presence[2*i+1] && 
		// 			!connector_status[2*i] && !connector_status[2*i+1]) {
		// 			//Should lock
		// 			robot_console_printf("Lock %d !\n", i);
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
		// 			robot_console_printf("Unlock %d..\n", i);
		// 		}
		// 		
		// 		// Unlock if bad position
		// 		if ((connector_status[2*i] == LOCKED && !connector_presence[2*i] )||
		// 		(connector_status[2*i+1]== LOCKED && !connector_presence[2*i+1])){
		// 			unlock = true;
		// 		}
	}

	
	return TIME_STEP;           /* run for TIME_STEP ms */
}

int main()
{
	robot_live(reset);
	robot_run(run);             /* this function never returns */

	return 0;
}
