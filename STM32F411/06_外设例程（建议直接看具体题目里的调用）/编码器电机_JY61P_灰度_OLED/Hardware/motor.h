#include "tim.h"
#include "main.h"
#include "usart.h"
#include "encoder.h"
#include "control.h"
#include "stm32f4xx.h"  

#ifndef __MOTOR_H 
#define __MOTOR_H 

#define PI 3.14159265							//PI圆周率
#define Control_Frequency  200.0	//编码器读取频率
#define Diameter_67  67.0 				//轮子直径67mm 
#define EncoderMultiples   4.0 		//编码器倍频数
#define Encoder_precision  500.0 	//编码器精度 500线
#define Reduction_Ratio  30.0			//减速比30
#define Perimeter  210.4867 			//周长，单位mm


void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right);
void Set_Pwm(int motor_left,int motor_right);
int PWM_Limit(int IN,int max,int min);
float Velocity_PI_left(float Velocity,float Speed, int flag);
float Velocity_PI_right(float Velocity,float Speed, int flag);
int Turn_PID(float yaw, float Turn_Target, int flag);

#endif
