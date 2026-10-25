#ifndef PWM_H
#define PWM_H

#include "ti_msp_dl_config.h"




#define PI 3.14159265							//PI圆周率
#define Control_Frequency  200.0	//编码器读取频率
#define Diameter_67  67.0 				//轮子直径67mm 
#define EncoderMultiples   4.0 		//编码器倍频数
#define Encoder_precision  500.0 	//编码器精度 500线
#define Reduction_Ratio  30.0			//减速比30
#define Perimeter  210.4867 			//周长，单位mm
// 设置PWM输出
void Set_PWM(int motor_left, int motor_right);
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right);
#endif // PWM_H