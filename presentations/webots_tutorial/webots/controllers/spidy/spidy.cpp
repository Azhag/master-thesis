/***************************************************************************
  
  spidy -- controller of the bioloid quadruped robot
  Authors: Simon Ruffieux  Date: 2008

***************************************************************************/

#include "SpidyRobot.h"


#include <stdlib.h>
#include <time.h>
#include <stdio.h>

using namespace std;

static SpidyRobot *robot = NULL;

static char *commands[] ={
    "Welcome to spidy robot simulation\n",
    "Use:\n,"
    "- Q/W to increase/decrease Base_UpDown servos\n",
    "- A/S to increase/decrease Knees_UpDown servos\n",
    "- Y/X to increase/decrease Base_FrontHind servos\n",
    NULL
  };

static void reset(void)
{
    robot_console_printf("reset()\n");
    const char *robotName = robot_get_name(); 
    robot = new SpidyRobot(robotName);

    // enable keyboard
    robot_keyboard_enable(SIMULATION_STEP_DURATION);
    return;
}

int main() {
	srand(time(NULL));

	robot_live(reset);
	
	//robot->up();
	
  	//supervisor_simulation_physics_reset();
	//robot->standing();
	
	robot->wait(1);
	for(int c=0;commands[c]!=NULL;c++) robot_console_printf(commands[c]);
	robot->wait(1);
	
	robot->interactiveMode();
	
   delete robot; 

  return 0;
}
