#ifndef __PID_H
#define __PID_H


float PID_Calculate_X(float measurement);
float PID_Calculate_Y(float measurement);
void Move_Gimbal(float speed_x, float speed_y);
void Gimbal_Update_Position(void);
void Stepper_Motor_Update_Position(void);
void Gimbal_Set_Target(float target_x, float target_y);

#endif // !__PID_H
