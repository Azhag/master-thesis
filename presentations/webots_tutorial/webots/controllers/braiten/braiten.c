//******************************************************************************
//  Name:   e-puck.c
//  Author: Yvan Bourquin <yvan.bourquin@epfl.ch>
//  Date:   November 20th, 2006
//******************************************************************************

#include <device/robot.h>
#include <device/differential_wheels.h>
#include <device/distance_sensor.h>
#include <math.h>

#define NB_SENSORS 8
#define TIME_STEP 64
#define MAX_SPEED 1000
#define MAX_SENS 3800

DeviceTag ps[NB_SENSORS];

//-----------------------------------------------fitness evaluation-------------
// The fitness evaluation functions are provided to help you testing the
// performance of your controller.

// fitness variables
int step_count;
double fit_speed;
double fit_diff;
double sens_val[NB_SENSORS];

// reset fitness
void fitness_reset() {
  step_count = 0;
  fit_speed = 0.0;
  fit_diff = 0.0;
  int i;
  for (i = 0; i < NB_SENSORS; i++)
    sens_val[i] = 0.0;
}

// update fitness variables
void fitness_step(float left_speed, float right_speed, const double ds_value[NB_SENSORS]) {
  // average speed
  fit_speed += (fabs(left_speed) + fabs(right_speed)) / (2.0 * MAX_SPEED);
  // difference in speed
  fit_diff += fabs(left_speed - right_speed) / (2.0 * MAX_SPEED);
  // sensor values
  int i;
  for (i = 0; i < NB_SENSORS; i++)
    sens_val[i] += ds_value[i] / MAX_SENS;

  step_count++;
}

// compute current fitness
// the higher the number the better is the obstacle avoidance
// a good controller is expected to give a performance above 0.8
double fitness_compute() {
  double fit_sens = 0.0;
  int i;  
  for (i = 0; i < NB_SENSORS; i++)
    if (sens_val[i] > fit_sens)
      fit_sens = sens_val[i];

  fit_sens /= step_count;

  return fit_speed / step_count * (1.0 - sqrt(fit_diff / step_count)) * (1.0 - fit_sens);
}
//------------------------------------------------------------------------------

// controller initialization
static void reset(void) {
  fitness_reset();
  int i;
  char name[] = "ps0";
  for(i = 0; i < NB_SENSORS; i++) {
    ps[i]=robot_get_device(name); // get sensor handle
    // perform distance measurements every TIME_STEP millisecond
    distance_sensor_enable(ps[i], TIME_STEP);
    name[2]++; // increase the device name to "ps1", "ps2", etc.
  }
}

// controller main loop
static int run(int ms) {
  static double ds_value[NB_SENSORS];
  int i;
  for (i = 0; i < NB_SENSORS; i++)
    // read sensor values
    ds_value[i] = distance_sensor_get_value(ps[i]); // range: 0 (far) to 3800 (close)

  // choose behavior
  double left_speed, right_speed;
  int duration;
  
  double brait[NB_SENSORS][2] = {       {-0.5,0.5},  //Front right
                                        {-0.5, 0.5}, //F right
                                        {-0.4,0.0}, //Right
                                        {1,1},  // Behind right
                                        {1,1},  // Behind left
                                        {0.0,-0.4},  // Left
                                        {0.5,-0.5},  // F left
                                        {0.5,-0.5} //Front left
                                        };
  
  left_speed = 0.0;
  right_speed = 0.0;
  
  // Matrice multiplication
  for (i=0; i<NB_SENSORS;i++){
    left_speed += ds_value[i]*brait[i][0];
    right_speed += ds_value[i]*brait[i][1];
  }
  duration = TIME_STEP;
  
  // update fitness
  fitness_step(left_speed, right_speed, ds_value);

  // actuate wheel motors
  differential_wheels_set_speed(left_speed, right_speed);

  // compute and display fitness every 1000 controller steps
  if (step_count % 1000 == 999) {
    double fitness = fitness_compute();
    printf("fitness = %f\n", fitness);
    fitness_reset();
  }

  return duration;
}

int main() {
  robot_live(reset); // controller initialization
  robot_run(run);    // controller start
  return 0;
}
