/*----------------------------------------------------------
  File:         ghostdog.cpp
  Date:         Octobre 12th, 2006
  Description:  Running quadruped with active hip joints and
                passive (spring & damper) knee joints
                Lab exercise for the course:
                  Models of Biological Sensory-Motor Systems
  Author:       Looc Matthey, Cidric Favre
----------------------------------------------------------*/
#include <stdio.h>
 
#include <device/robot.h>
#include <device/servo.h>
#include <math.h>
#include <stdlib.h>
#include <device/supervisor.h>
// control step duration in millisec
#define TIME_STEP 16
#define TIME_STEP_GET_POS (TIME_STEP - 2)
#define TIME 1000000  //30000
#define NB_AMPL 8
#define NB_FREQ 10
#define FREQ_STEP (6.0 / (NB_FREQ + 1))
#define FREQ_FIRST (FREQ_STEP)
#define AMPL_STEP (M_PI_2 / (NB_AMPL + 1))
#define AMPL_FIRST (AMPL_STEP)
#define pi M_PI
#define DRIVE_STEP 0.05
#define MAX_AMPL_HIP 0.6 //0.4
#define MAX_AMPL_KNEE 0.45 //0.3
#define MAX_FREQ 3.0

#define NB_CPG 19

#define LIMIT_WALK 1.6
#define LIMIT_TROT 2.2
#define LIMIT_GALLOP MAX_FREQ
// number of DOFs of the robot
#define MAX_SERVOS 12
#define KEYB_OFF 2

DeviceTag servos[MAX_SERVOS];

NodeRef ghostdog = NULL;

// elapsed simulation time
float elapsed_time = 0.0;

// current spine angle
float spine_angle = 0.0;  // straight

float current_position[7];
float old_pos[7];
float delta_tot = 0.0;

enum movement {walk=0, trot, gallop};
enum movement current_movement = walk;

/*float phi_mov[3][NB_CPG]= {{ -(3*pi)/4, -pi/2, pi/2, pi/4, pi/2, pi/2, pi/2, pi/2, (3*pi)/8},
                      { pi/2, pi/2, -pi/2, -pi/2, pi/2, pi/2, pi/2, pi/2, -pi/2},
                      { pi/2, 0, 0, pi/2, 0, 0, 0, 0, 0} };*/

float abs(float a) {
  if (a >= 0) {
    return a;
  } else {
    return -a;
  }
}
static void reset(void)
{
   
     ghostdog = supervisor_node_get_from_def("GHOSTDOG");
    // hip and spine servo names
    // we don't control the knee servo: knees are passive
    const char *SERVO_NAMES[] = {
      "hip2",
      "hip3",
      "hip0",
      "hip1",
      "spine",
      "knee2",
      "knee3",
      "knee0",
      "knee1",
      NULL
    };
  
    // get servo device tags
    for (int i = 0; SERVO_NAMES[i]; i++) {
      servos[i] = robot_get_device(SERVO_NAMES[i]);
      if (! servos[i]) {
        printf("could not find servo: %s\n", SERVO_NAMES[i]);
      }
    }
    supervisor_field_get(ghostdog, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION, current_position, TIME_STEP_GET_POS);
    //position reset
    
    // enable keyboard
    robot_keyboard_enable(TIME_STEP);

    // Print informations
    supervisor_set_label(0, "Drive:", 0.01,0.01,0.04, 0x000000); // Speed in black
    supervisor_set_label(1, "Movement:", 0.01,0.045, 0.04, 0x000000); //Movement in black;
    supervisor_set_label(2, "Speed:", 0.01,0.08,0.04, 0x000000); // Speed in black
    return;
}
int saturate(int i, movement current_movement)
{
  switch(current_movement)
  {
    case walk: 
      return 1;
    case trot:
      if(i == 9 || i == 10 || i == 18)
      {
        return 0;
      }
      else 
      {
        return 1;
      }
    case gallop:
      if(i > 7)
      {
        return 0;
      }
      else 
      {
        return 1;
      }
    default:
      robot_console_printf("ERROR\n");
  }
  return 0;
}

void fcn (float y[NB_CPG], float dy[2*NB_CPG],float v,  float R[NB_CPG], movement current_movement )
{
 // Coupled Pattern Generator
  int i, j, sat;
  float sum;
  
  int iw = 20, it = 10, ig = 6;
  int kt = 5, kg = 1;
  int sw = 10, st = 5;

  
  //4xhip - 4x knees - 1xspine - HIDEN : 5x phi hip - 4x phi knees - 2x phi spine
                        //  1   2   3   4   5   6   7   8   9   10  11    12  13    14  15  16  17  18  19
  int w[NB_CPG][NB_CPG] = {{0,  ig, ig, 0,  kg, 0,  0,  0,  st, iw,  0,   it,  0,   0,  kt, 0,  0,  0,  sw},
                           {ig, 0,  0,  ig, 0,  kg, 0,  0,  0,  iw,  0,   0,  it,   0,  0,  kt, 0,  0,  0},
                           {ig, 0,  0,  ig, 0,  0,  kg, 0,  0,  0,   iw,  it,  0,   it, 0,  0,  kt, 0,  0}, 
                           {0,  ig, ig, 0,  0,  0,  0,  kg, 0,  0,   iw,  0,  it,   it, 0,  0,  0,  kt, 0},
                           {kg, 0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  kt,  0,  0,  0, 0},
                           {0,  kg, 0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  kt,  0,  0, 0},
                           {0,  0,  kg, 0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  kt,  0, 0},
                           {0,  0,  0,  kg, 0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  kt, 0},
                           {st, 0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  sw},
                           {iw, iw, 0,  0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  0,  iw, iw, 0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {it, 0,  it, 0,  0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  it, 0,  it, 0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  0,  it, it, 0,  0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {kt, 0,  0,  0,  kt, 0,  0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  kt, 0,  0,  0,  kt, 0,  0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  0,  kt, 0,  0,  0,  kt, 0,  0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {0,  0,  0,  kt, 0,  0,  0,  kt, 0,  0,   0,   0,  0,    0,  0,  0,  0,  0,  0},
                           {sw, 0,  0,  0,  0,  0,  0,  0,  sw, 0,   0,   0,  0,    0,  0,  0,  0,  0,  0}};

  float coef_conv = 8;
  
  //float a[NB_CPG] = {coef_conv, coef_conv, coef_conv, coef_conv, coef_conv, coef_conv, coef_conv, coef_conv, coef_conv};
  //4xhip - 4x knees - 1xspine - 5x phi hip - 4x phi knees - 2x phi spine
                            //  1     2       3     4     5     6     7     8     9     10    11    12      13    14    15      16    17    18    19
  float phi[NB_CPG][NB_CPG] = {{0,    -0.5,   0,    0,    0,    0,    0,    0,    0.25, -0.75, 0,   -0.5,   0,    0,    0.25,    0,    0,    0,    3.0/8.0},
                               {0.5,  0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0,      0.5,  0,    0,      0.25,  0,    0,    0},
                               {0,    0,      0,    -0.5, 0,    0,    0,    0,    0,    0,    0.25, 0,      0,    0.5,  0,      0,    0.25,  0,    0}, 
                               {0,    0,      0.5,  0,    0,    0,    0,    0,    0,    0,    0,    0,      0,    0,    0,      0,    0,    0.25,    0},
                               {0,    0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0,      0,    0,    0,      0,    0,    0,      0},
                               {0,    0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {-0.25,  0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0.75, 0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      -0.25,0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0.5,  0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    -0.5,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      -0.5, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {-0.25, 0,      0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    -0.25,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      -0.25, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {0,    0,      0,    -0.25, 0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
                               {-3.0/8.0, 0,  0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0}};

  
  
  
  for (i = 0 ; i < NB_CPG ; ++i)
  {
    sum = 0;
    for(j = 0 ; j < NB_CPG; ++j)
    {
      sum += y[2*j+1]*w[i][j]*sin(y[2*j]-y[2*i]-2*pi*phi[i][j]);
    }
    sat = saturate(i,current_movement);
    dy[2*i] = 2*pi*v*sat + sum;
     dy[2*i+1] = coef_conv*(R[i]*sat-y[2*i+1]);
    //dy[2*i+1] = a[i]*(R[i]-y[2*i+1]);
  }
}

void next_step(float* x, float* y)
{
  float off[NB_CPG] = {0.0, -0.3, 0.0, -0.3,
                  0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i;
  for( i = 0 ; i<NB_CPG ; ++i )
  {
     x[i] = off[i] + y[2*i+1] * cos(y[2*i]);
  }
  /*robot_console_printf("Phi12 : %f, Phi13 : %f, Phi14 : %f, Phi24 : %f\n", 
    y[2]-y[0], y[4]-y[0], y[6]-y[0], y[6]-y[2]);*/
} 

void euler(float* y, float* dy, float delta_t)
{
  int i = 0;
  
  delta_t = delta_t / 1000; 
  for(i=0 ; i< (2*NB_CPG); ++i)
  {
    y[i] = y[i]+(delta_t*dy[i]); 
  }
}

movement getMovement(float drive)
{
  if(drive > LIMIT_TROT)
  {
    robot_console_printf("movement : gallop\n");  
    return gallop;
  }
  if(drive > LIMIT_WALK)
  { 
    robot_console_printf("movement : trot\n");  
    return trot;
  }
  robot_console_printf("movement : walk\n");  
  return walk;
}

char* getCurrentMovementString() {
  switch (current_movement) {
    case gallop:
      return "Gallop";
    case trot:
      return "Trot";
    case walk:
      return "Walk";
  }
  return NULL;
}
float getDrive(float drive)
{
  int new_key = robot_keyboard_get_key();
  if(new_key == (317-KEYB_OFF) && drive < MAX_FREQ) //arrow up
  {
    drive += DRIVE_STEP;
    robot_console_printf("drive : %f\n", drive);
    current_movement = getMovement(drive);
  }
  if(new_key == (319-KEYB_OFF) && drive > 0.0)  //arrow down
  {
    drive -= DRIVE_STEP;
    robot_console_printf("drive : %f\n", drive);  
    current_movement = getMovement(drive);
  }
  return drive;
}
 void set_ampl(float v, float R[NB_CPG])
 {
    int i;
    float ampl_hips, ampl_knees, ampl_spine;
    switch(current_movement)
    {
      case walk :
        ampl_hips = (v/LIMIT_WALK)*(0.3);
        ampl_knees = (v/LIMIT_WALK)*(0.3);
        ampl_spine = (v/LIMIT_WALK)*0.15;
        break;
    
      case trot :
        ampl_hips = (v/LIMIT_TROT)*(0.4);
        ampl_knees = (v/LIMIT_TROT)*(0.3);
        ampl_spine = (v/LIMIT_TROT)*0.2;
        break;
    
      case gallop :
        ampl_hips = (v/LIMIT_GALLOP)*(0.5);
        ampl_knees = (v/LIMIT_GALLOP)*(0.3);
        break;
    }
    
    for(i = 0; i < NB_CPG; ++i)
    {
      if(i<4)
      {
        R[i]=ampl_hips;
      }
      else if(i<8)
      {
        if(i%2)
          R[i]=ampl_knees*1.5;
        else
          R[i]=ampl_knees;
      }
      else if (i == 8)
      {
        R[i]=ampl_spine;
      }
      else
      {
        R[i]=1;
      }
    }
 }

static void run(void)
{
  robot_console_printf("\n\n\n");
 
  int i;
  float y[2*NB_CPG];
  float dy[2*NB_CPG];
  float x[NB_CPG];
  float delta = 0.0;
  float speed = 0.0;
  float drive =0.0;
  char speed_char[10];
  char drive_char[10];
  
  /*for (i = 0;i<7;i++) {
    old_pos[i] = current_position[i];
  }*/
  /*float initial_conditions[2*NB_CPG]={0.0, 0.0, 0.0, 0.0,
                                0.1, 0.0, 0.0, 0.0,
                                0.0, 0.0, 0.0, 0.1,
                                0.0, 0.0, 0.0, 0.0,
                                0.2, 0.1};*/
  //base values
  
  
  //FILE* fileID =  fopen("recherche_syst.m", "w");
  //fprintf(fileID, "Z = [ ");
 
  float   v;
   
  float R[NB_CPG];
   
  for(i=0; i<2*NB_CPG; ++i)
  {
    y[i] = 0.2; //initial_conditions[i]; // (float)(9-(i/2))/10.0;
    dy[i] = 0.0;
  }
  
  // run TIME miliseconds 
  for(i = 0 ; i < TIME ; i += TIME_STEP) 
  {
    drive=getDrive(drive);
    v = drive;
    set_ampl(v, R);
    fcn(y, dy, v, R, current_movement);
    euler(y,dy,TIME_STEP);
    next_step(x,y);
    //fprintf(fileID," %f %f %f %f %f %f ;\n", y[2]-y[0], y[4]-y[0], y[6]-y[2], y[6]-y[4], y[6]-y[0], y[2]-y[4]);
    
    // Calculate speed
    if (!(i%(TIME_STEP*50))) {
      //Every 50 TIME_STEP to avoid oscillations (= 800ms)
      delta = pow(pow((current_position[0]- old_pos[0]),2) + pow((current_position[2]-old_pos[2]),2), 0.5);
      old_pos[0] = current_position[0];
      old_pos[2] = current_position[2];
      speed = 1000*delta/((float)50*TIME_STEP);
    }
    //robot_console_printf("Speed : %d, ,%f, %f, %f, %f\n", i, delta, speed, old_pos[0], current_position[0]);
    
    elapsed_time += (float) TIME_STEP / 1000;    
    /*if (elapsed_time > 5.0 && abs(delta) < 0.000001)  {//abs(current_position[6]) > 2) {
      break;
    }*/
    
    
    // Print informations
    sprintf(drive_char, "%.2f", drive);
    sprintf(speed_char, "%.2f", speed);
    
    
    supervisor_set_label(3, drive_char, 0.16, 0.01, 0.04, 0x0000ff); /* blue movement */
    supervisor_set_label(4, getCurrentMovementString(), 0.16, 0.045, 0.04, 0x0000ff); /* blue movement */
    supervisor_set_label(5, speed_char, 0.16, 0.08, 0.04, 0xff0000); /* red speed */
    
    // actuate front legs
    servo_set_position(servos[0], x[0]);
    servo_set_position(servos[2], x[2]);
    
    //actuate front kmees
    servo_set_position(servos[5], x[4]);
    servo_set_position(servos[7], x[6]);
    
    // actuate back legs
    servo_set_position(servos[1], x[1]);
    servo_set_position(servos[3], x[3]);
    
    //actuate back knees
    servo_set_position(servos[6], x[5]);
    servo_set_position(servos[8], x[7]);
    
    //actuate spine
    servo_set_position(servos[4], x[8]);
    
    robot_step(TIME_STEP);
  }
  
  
  
  
  // Reset
  servo_set_position(servos[0], 0);
  servo_set_position(servos[1], 0);
  servo_set_position(servos[2], 0);
  servo_set_position(servos[3], 0);
  servo_set_position(servos[4], 0);
  servo_set_position(servos[5], 0);
  servo_set_position(servos[6], 0);
  servo_set_position(servos[7], 0);
  servo_set_position(servos[8], 0);
  
  const float FLOAT_POS[7] = { 0, 1000, 0, 0, 1, 0, 0 };
  supervisor_field_set(ghostdog, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION,(void*)FLOAT_POS);
    
  robot_step(10*TIME_STEP);
      
  const float INITIAL_POSITION_AND_ROTATION[7] = { 0, 0.3, 0, 0, 1, 0, 0 };
  supervisor_field_set(ghostdog, SUPERVISOR_FIELD_TRANSLATION_AND_ROTATION,(void*)INITIAL_POSITION_AND_ROTATION);
  
  elapsed_time = 0.0;
  //fclose(fileID);
}


int main() {
  robot_live(reset);
  run();
  return 0;
}
