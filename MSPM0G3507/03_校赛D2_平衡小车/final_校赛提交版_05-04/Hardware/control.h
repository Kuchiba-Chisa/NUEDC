#ifndef __CONTROL_H
#define __CONTROL_H

#include "ti_msp_dl_config.h"
#include "encoder.h"
#include "motor.h"
#include "uart.h"

int Balance(float Angle,float Gyro);
int Velocity(int encoder_left,int encoder_right,int flag, float Target);
int Turn(float Yaw, float gyro, float Target);
float Cam_Sensor(void);
void State(void);
int Chasu_PID (int error,int target);
#endif
