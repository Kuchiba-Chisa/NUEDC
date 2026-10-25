#ifndef __PID_H
#define	__PID_H

#include "ti_msp_dl_config.h"


void ReadSensors(uint8_t* values);
int CalculateOffset(uint8_t* sensors);
int CalculateOffset_IMU(int yaw);
int CalculateOffset_angle(int* sensors2);
float ComputePID(float current_offset);
float angle_PID_Controller(float offset_Yaw, float dt);
void AdjustMotorSpeed(float correction_value);
void reset_pid();
float compute_pid(float setpoint, float input, float dt);
float SpeedPID_Controller(int target_speed, float current_speed, float dt) ;
void SpeedControl(int target_left, int target_right, 
                 float current_left, float current_right, float dt);

#endif
