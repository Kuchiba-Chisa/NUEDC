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
//		OLED_Printf(0, 0, OLED_8X16, "gw:%d,%d,%d,%d,%d,%d,%d,%d",BIT[0],BIT[1],BIT[2],BIT[3],BIT[4],BIT[5],BIT[6],BIT[7]);
		OLED_Update();	
		int offset = CalculateOffset(BIT);       
		float pid_output = ComputePID(offset);   
		AdjustMotorSpeed(pid_output);
	
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==0&&stopflag==0)//路口判断左右转
		{
			OLED_Printf(0, 48, OLED_8X16, "turn");
			crossflag=1;
			if(Uart_RxBuffer_2 =='L')
			{
				turn=0;
				OLED_Printf(0, 48, OLED_8X16, "turn:L");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(2000,-2000);
				delay_ms(500);
//				while((BIT[3]+BIT[4])==2)
//				{
//					ReadSensors(BIT);
//					delay_ms(10);
//				}
				crossflag = 1;//执行完转弯
			}
			else if(Uart_RxBuffer_2 =='R')
			{
				turn=1;
				OLED_Printf(0, 48, OLED_8X16, "turn:R");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(-2000,2000);
				delay_ms(500);
				crossflag = 1;//执行完转弯
			}
		}
		
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 1) //第一题到达终点
		{		
			if(stopflag==0) //药没被取走
			{			
//				Set_PWM(2000,2000);
//				delay_ms(200);
				Set_PWM(0,0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //红灯亮
				OLED_Printf(0, 48, OLED_8X16, "Take your medical");
				if(HAL_GPIO_ReadPin(GPIOB,XL_Pin)==0)
				{
					stopflag=1;
					OLED_Printf(0, 32, OLED_8X16, "NO Medicine");
				}
			}
			else 	//药被取走
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  //红灯灭
				OLED_Printf(0, 48, OLED_8X16, "medical taken");
				Set_PWM(0,0);
				delay_ms(1000);
				if(turn==1)
				{
					while(Yaw<=88||Yaw>=92)
					{
							IMU_Proc();
							OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
							OLED_Update();

							Set_PWM(1000,-1000);	
					}					
				}
				else
				{
					while(Yaw<=-92||Yaw>=-88)
					{
							IMU_Proc();
							OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
							OLED_Update();

							Set_PWM(-1000,1000);	
					}						
				}

				crossflag = 2;   //执行完第一题送药，掉头后准备返回
			}
		}
		
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==2&&stopflag==1) //返回路上转弯
		{
			OLED_Printf(0, 48, OLED_8X16, "Back turn");	
			if(turn ==0)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:R"); //去的时候左转，回来就右转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(-2000,2000);
				delay_ms(500);
				crossflag = 3;   //回来的时候转弯
			}
			else if(turn ==1)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:L");		//去的时候右转，回来就左转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(2000,-2000);
				delay_ms(500);
				crossflag = 3;   //回来的时候转弯
			}	
		}
		
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 3&&stopflag==1) //第一题结束
		{
			Set_PWM(0,0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  //绿灯亮
			startflag=0;
		}
}

void problem2(void)
{
//		UART1_SendChar('2');
		IMU_Proc();
		OLED_Printf(0, 0, OLED_8X16, "crossflag:%d",crossflag);
//		OLED_Printf(0, 16, OLED_8X16, "turn:%d",turn);

		ReadSensors(BIT);               
		int offset = CalculateOffset(BIT);      
		float pid_output = ComputePID(offset);   
		AdjustMotorSpeed(pid_output);
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==0)//第一个十字路口直走
		{
			crossflag=1;
			Set_PWM(2000,2000);
			delay_ms(300);
			HAL_TIM_Base_Start_IT(&htim2);
		}
		if(lookflag==1)
		{
			Set_PWM(0,0);
			UART1_SendChar('f');
			delay_ms(500);
			Set_PWM(2000,-2000);
			delay_ms(200);//往左小摆头
			Set_PWM(0,0);
			for(int i=0;i<10;i++)
			{
				UART1_SendChar('f');			
			}
			delay_ms(3000);//检测两秒
			Set_PWM(-2000,2000);
			delay_ms(200);//转回来
			Set_PWM(0,0);
			lookflag=2;			
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==1)//第二个十字路口判断左右
		{
			
			if(Uart_RxBuffer_2 == 'L')
			{
				turn=0;
				OLED_Printf(0, 48, OLED_8X16, "turn:L");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(2000,-2000);
				delay_ms(700);
				crossflag=2;

			}
			if(Uart_RxBuffer_2 =='R')
			{
				turn=1;
				OLED_Printf(0, 48, OLED_8X16, "turn:R");
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(-2000,2000);
				delay_ms(750);
				crossflag=2;
			}
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&&(crossflag == 2)) //第二题到达终点  &&(crossflag == 2)
		{	
			
			Set_PWM(0,0);
			while(stopflag==0) //药没被取走
			{			
//				Set_PWM(2000,2000);
//				delay_ms(200);
				Set_PWM(0,0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //红灯亮
				OLED_Printf(0, 48, OLED_8X16, "Take your medical");
				if(HAL_GPIO_ReadPin(GPIOB,XL_Pin)==0)
				{
					stopflag=1;
					OLED_Printf(0, 32, OLED_8X16, "NO Medicine");
				}
			}
			//药被取走
		
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  //红灯灭
			OLED_Printf(0, 48, OLED_8X16, "medical taken");
			Set_PWM(0,0);
			delay_ms(1000);
			if(turn==1)
			{
				while(Yaw<=88||Yaw>=92)
				{
						IMU_Proc();
						OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
						OLED_Update();

						Set_PWM(1000,-1000);	
				}					
			}
			else
			{
				while(Yaw<=-92||Yaw>=-88)
				{
						IMU_Proc();
						OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
						OLED_Update();

						Set_PWM(-1000,1000);	
				}						
			}

			crossflag = 3;   //执行完第一题送药，掉头后准备返回
			
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==3&&stopflag==1) //返回路上转弯
		{
			OLED_Printf(0, 48, OLED_8X16, "Back turn:%d turn");	
			if(turn ==0)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:R"); //去的时候左转，回来就右转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(-2000,2000);
				delay_ms(500);
				crossflag = 4;   //回来的时候转弯
			}
			else if(turn ==1)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:L");		//去的时候右转，回来就左转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(2000,-2000);
				delay_ms(750);
				crossflag = 4;   //回来的时候转弯
			}	
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==4)//回去遇到第一个路口直走
		{
			crossflag=5;
			Set_PWM(2000,2000);
			delay_ms(300);
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 5&&stopflag==1) //第一题结束
		{
			Set_PWM(0,0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  //绿灯亮
			startflag=0;
		}
}

void problem3(void)
{
		IMU_Proc();
		OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
		ReadSensors(BIT);               
		int offset = CalculateOffset(BIT);      
		float pid_output = ComputePID(offset);   
		AdjustMotorSpeed(pid_output);
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&(crossflag==0||crossflag==1))//前两个十字路口直走
		{
			crossflag++;
			Set_PWM(2000,2000);
			delay_ms(300);
			if(crossflag==2)
			{
				HAL_TIM_Base_Start_IT(&htim2);
				
			}
		}
		if(lookflag==1)
		{
			Set_PWM(0,0);
			delay_ms(500);
			Set_PWM(2000,-2000);
			delay_ms(300);//往左大摆头
			Set_PWM(0,0);
			for(int i=0;i<10;i++)
			{
				UART1_SendChar('f');			
			}
			delay_ms(1500);//检测三秒
			Set_PWM(-2000,2000);
			delay_ms(100);//左边近点的数字
			Set_PWM(0,0);
			for(int i=0;i<10;i++)
			{
				UART1_SendChar('g');			
			}
			delay_ms(1500);//检测三秒		
			Set_PWM(-2000,2000);
			delay_ms(200);//右边第一个数字	
			Set_PWM(0,0);
//			for(int i=0;i<10;i++)
//			{
//				UART1_SendChar('h');			
//			}
//			delay_ms(3000);//检测三秒	
//			Set_PWM(2000,-2000);
//			delay_ms(200);//回来			
//			Set_PWM(0,0);
			lookflag=2;			
		}
		
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==2)//第二个十字路口判断左右
		{

			if(Uart_RxBuffer_2 =='L')
			{
				turn1=0;
				OLED_Printf(0, 48, OLED_8X16, "turn:L");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(2000,-2000);
				delay_ms(750);
				crossflag=3;
			}
			if(Uart_RxBuffer_2 =='R')
			{
				turn1=1;
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(-2000,2000);
				ReadSensors(BIT);
				delay_ms(750);
				crossflag=3;
			}
			HAL_TIM_Base_Start_IT(&htim9);

		}
//		if(lookflag2==1&&lookflag==2)
//		{
//			Set_PWM(0,0);
//			UART1_SendChar('f');
//			delay_ms(500);
//			Set_PWM(2000,-2000);
//			delay_ms(200);//往左小摆头
//			Set_PWM(0,0);
//			for(int i=0;i<10;i++)
//			{
//				UART1_SendChar('f');			
//			}
//			delay_ms(3000);//检测两秒
//			Set_PWM(-2000,2000);
//			delay_ms(200);//转回来
//			Set_PWM(0,0);
//			lookflag2=2;			
//		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==3)//第二个十字路口判断左右
		{
			crossflag=4;

			if(Uart_RxBuffer_2 =='L')
			{
				turn2=0;
				OLED_Printf(0, 48, OLED_8X16, "turn:L");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(2000,-2000);
				delay_ms(750);
			}
			if(Uart_RxBuffer_2 =='R')
			{
				turn2=1;
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(-2000,2000);
				ReadSensors(BIT);
				delay_ms(750);
			}
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 4) //第三题到达终点
		{		
			if(stopflag==0) //药没被取走
			{			
//				Set_PWM(2000,2000);
//				delay_ms(200);
				Set_PWM(0,0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //红灯亮
				OLED_Printf(0, 48, OLED_8X16, "Take your medical");
				if(HAL_GPIO_ReadPin(GPIOB,XL_Pin)==0)
				{
					stopflag=1;
					OLED_Printf(0, 32, OLED_8X16, "NO Medicine");
				}
			}
			else 	//药被取走
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  //红灯灭
				OLED_Printf(0, 48, OLED_8X16, "medical taken");
				Set_PWM(0,0);
				delay_ms(1000);
				if(turn2==1)
				{
					while(Yaw<=-2||Yaw>=2)
					{
							IMU_Proc();
							OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
							OLED_Update();

							Set_PWM(1000,-1000);	
					}					
				}
				else
				{
					while(Yaw<=-182||Yaw>=-178)
					{
							IMU_Proc();
							OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
							OLED_Update();

							Set_PWM(-1000,1000);	
					}						
				}

				crossflag = 5;   //执行完第一题送药，掉头后准备返回
			}
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=5)&&crossflag==5&&stopflag==1) //返回路上转弯
		{
			OLED_Printf(0, 48, OLED_8X16, "Back turn");	

			if(turn2 ==0)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:R"); //去的时候左转，回来就右转
				Set_PWM(2000,2000);
				delay_ms(500);
				Set_PWM(-2000,2000);
				delay_ms(750);
				crossflag = 6;   //回来的时候转弯
			}
			else if(turn2 ==1)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:L");		//去的时候右转，回来就左转
				Set_PWM(2000,2000);
				delay_ms(450);
				Set_PWM(2000,-2000);
				delay_ms(800);
				crossflag = 6;   //回来的时候转弯
			}
			Set_PWM(2000,2000);
			delay_ms(1000);
			ReadSensors(BIT);               
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=5)&&crossflag==6&&stopflag==1) //返回路上转弯
		{
			OLED_Printf(0, 48, OLED_8X16, "Back turn");	
			if(turn1 ==0)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:R"); //去的时候左转，回来就右转
				Set_PWM(2000,2000);
				delay_ms(500);
				Set_PWM(-2000,2000);
				delay_ms(750);
				crossflag = 7;   //回来的时候转弯
			}
			else if(turn1 ==1)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:L");		//去的时候右转，回来就左转
				Set_PWM(2000,2000);
				delay_ms(500);
				Set_PWM(2000,-2000);
				delay_ms(750);
				crossflag = 7;   //回来的时候转弯
			}	
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&(crossflag==7||crossflag==8))//回去遇到第三四个路口直走
		{
			crossflag++;
			Set_PWM(2000,2000);
			delay_ms(100);
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 9&&stopflag==1) //第一题结束
		{
			Set_PWM(0,0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  //绿灯亮
			startflag=0;
		}
}
void problem4(void)
{
		IMU_Proc();
		OLED_Printf(0, 0, OLED_8X16, "crossflag:%d",crossflag);
//		OLED_Printf(0, 16, OLED_8X16, "turn:%d",turn);

		ReadSensors(BIT);               
		int offset = CalculateOffset(BIT);      
		float pid_output = ComputePID(offset);   
		AdjustMotorSpeed(pid_output);
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==0)//第一个十字路口直走
		{
			crossflag=1;
			Set_PWM(2000,2000);
			delay_ms(300);
			HAL_TIM_Base_Start_IT(&htim2);
		}
		if(lookflag==1)
		{
			Set_PWM(0,0);
			UART1_SendChar('f');
			delay_ms(500);
			Set_PWM(2000,-2000);
			delay_ms(200);//往左小摆头
			Set_PWM(0,0);
			for(int i=0;i<10;i++)
			{
				UART1_SendChar('f');			
			}
			delay_ms(3000);//检测两秒
			Set_PWM(-2000,2000);
			delay_ms(200);//转回来
			Set_PWM(0,0);
			lookflag=2;			
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==1)//第二个十字路口判断左右
		{
			
			if(Uart_RxBuffer_2 == 'L')
			{
				turn=0;
				OLED_Printf(0, 48, OLED_8X16, "turn:L");
				Set_PWM(2000,2000);
				delay_ms(300);
				Set_PWM(2000,-2000);
				delay_ms(700);
				crossflag=2;

			}
			if(Uart_RxBuffer_2 =='R')
			{
				turn=1;
				OLED_Printf(0, 48, OLED_8X16, "turn:R");
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(-2000,2000);
				delay_ms(750);
				crossflag=2;
			}
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&&(crossflag == 2)) //第二题到达终点  &&(crossflag == 2)
		{	
			
			Set_PWM(0,0);
			while(stopflag==0) //药没被取走
			{			
//				Set_PWM(2000,2000);
//				delay_ms(200);
				Set_PWM(0,0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  //红灯亮
				OLED_Printf(0, 48, OLED_8X16, "Take your medical");
				if(HAL_GPIO_ReadPin(GPIOB,XL_Pin)==0)
				{
					stopflag=1;
					OLED_Printf(0, 32, OLED_8X16, "NO Medicine");
				}
			}
			//药被取走
			UART6_SendChar('1');//向第二辆车发启动
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  //红灯灭
			OLED_Printf(0, 48, OLED_8X16, "medical taken");
			Set_PWM(0,0);
			delay_ms(1000);
			if(turn==1)
			{
				while(Yaw<=88||Yaw>=92)
				{
						IMU_Proc();
						OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
						OLED_Update();

						Set_PWM(1000,-1000);	
				}					
			}
			else
			{
				while(Yaw<=-92||Yaw>=-88)
				{
						IMU_Proc();
						OLED_Printf(0, 0, OLED_8X16, "Imu:%f",Yaw);
						OLED_Update();

						Set_PWM(-1000,1000);	
				}						
			}

			crossflag = 3;   //执行完第一题送药，掉头后准备返回
			
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==3&&stopflag==1) //返回路上转弯
		{
			OLED_Printf(0, 48, OLED_8X16, "Back turn:%d turn");	
			if(turn ==0)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:R"); //去的时候左转，回来就右转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(-2000,2000);
				delay_ms(500);
				crossflag = 4;   //回来的时候转弯
			}
			else if(turn ==1)
			{
				OLED_Printf(0, 48, OLED_8X16, "back turn:L");		//去的时候右转，回来就左转
				Set_PWM(2000,2000);
				delay_ms(250);
				Set_PWM(2000,-2000);
				delay_ms(750);
				crossflag = 4;   //回来的时候转弯
			}	
			UART6_SendChar('2');//向第二辆车发出发
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]<=2)&&crossflag==4)//回去遇到第一个路口直走
		{
			crossflag=5;
			Set_PWM(2000,2000);
			delay_ms(300);
		}
		if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7]==8)&& crossflag == 5&&stopflag==1) //第一题结束
		{
			Set_PWM(0,0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  //绿灯亮
			startflag=0;
		}
}


