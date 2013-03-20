#ifndef Robot_H
#define Robot_H
/* 
   Modular robot implementation for Webots simulator
   Author: Yvan Bourquin
*/

#include <device/robot.h>


// number of DOFs of the robot
#define MAX_SERVOS 18
#define POSITION_ARRAY_SIZE 7

#define SIMULATION_STEP_DURATION 8


#define RF_BASE_FH     0
#define RF_BASE_UD     1
#define RF_KNEE_UD	  2

#define RM_BASE_FH     3
#define RM_BASE_UD     4
#define RM_KNEE_UD	  5

#define RH_BASE_FH     6
#define RH_BASE_UD     7
#define RH_KNEE_UD	  8

#define LF_BASE_FH     9
#define LF_BASE_UD     10
#define LF_KNEE_UD	  11

#define LM_BASE_FH     12
#define LM_BASE_UD     13
#define LM_KNEE_UD	  14

#define LH_BASE_FH     15
#define LH_BASE_UD     16
#define LH_KNEE_UD	  17



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define rad_2_deg(X) ( X / M_PI * 180.0 )
#define deg_2_rad(X) ( X / 180.0 * M_PI )


class SpidyRobot {
 public:
  // constructor: create robot according to specification
  SpidyRobot(const char *name);

  // destructor
  virtual ~SpidyRobot() {};
   
  const char *getName() const { return _name; }
  
  void enableServoPosition(int servoId);  
  void setServoPosition(int servoId, float value); 
  float getServoPosition(int servoId); 
  void rotateGround();
  
  void standing();
  void up();
  void interactiveMode();
  
  
  
  
  void wait(float x); 
  
  char label_buffer[64]; 
  
  static const char * SERVO_NAMES[MAX_SERVOS+1]; 
  static float initial_position_and_orientation[7]; 
  float groundCurrentPosition[7];
  int rotateGroundLeft;
  int rotateGroundRight;
  int changeAdhesion;
  
 private:
  NodeRef _noderef,_groundref;
  const char *_name;          // robot name
  float _controlStep;         // simulation step size in milliseconds
  int _stepCount;             // number of simulation steps so far
  float _currentPosition[POSITION_ARRAY_SIZE];
  float _prevPosition[POSITION_ARRAY_SIZE]; 
  
  DeviceTag servos[MAX_SERVOS];  
  DeviceTag _emitter, _receiver;
  
  float _x;
  float _z;
  float _distance;
  int _selectedServoId; //servo being updated
  int _offset;          //offset applied to the selected servo

  void reset(); 
};

#endif
