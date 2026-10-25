#ifndef __PID_H
#define	__PID_H

#include "main.h"
#define motor_max 9000
#define motor_min 0
#define NUM_SENSORS 8       // 灰度传感器数量
void ReadGyroscopeData(void);
void ReadSensors(uint8_t* values);
int CalculateOffset(uint8_t* sensors);
int CalculateOffset_IMU(int yaw);
float ComputePID(float current_offset);
float angle_PID_Controller(float offset_Yaw, float dt);
void AdjustMotorSpeed(float correction_value);

#endif
