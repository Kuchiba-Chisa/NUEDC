#include "app.h"
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "Imu.h"
#include "key.h"
#include "pid.h"
#include "OLED.h"
#include "Buzzer.h"
#include "usart_2.h"
#include "Delay.h"
#include "math.h"
extern int Flag;

extern int IMU_flag;
extern float Yaw;
extern char target;
int turn;
int turn1;
int turn2;
int startflag=0;
extern float Yaw_Init;
int stopflag=0; //是否到终点
int findflag=1;//退出原地旋转用
uint8_t Uart_RxBuffer;
float L_raw;
int crossflag=0;//第几个路口
uint8_t BIT[8]={0,0,0,0,0,0,0,0};
uint16_t find[10]={0};
extern uint8_t Uart_RxBuffer_2;
extern int motor_left,motor_right;
extern float Velocity_Left,Velocity_Right;
extern int lookflag;
extern int lookflag2;

void problem1(void)
{	
		IMU_Proc();
		OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
		ReadSensors(BIT);
		int offset = CalculateOffset(BIT);       
		float pid_output = ComputePID(offset);   
		AdjustMotorSpeed(pid_output);

}


