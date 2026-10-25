#include "main.h"
#include "distance.h"
#include "debug.h"
#include "OLED.h"
#include "delay.h"
#include "math.h"
extern TIM_HandleTypeDef htim2;
int s1=0;
int s2=0;
float raw_distance=10.0;
float distance=25.0;
float last_distance=11.0;
float last_raw_distance=0.0;
int false_flag=0;
extern int Flag_ly;
void get_distance(void)
{
		htim2.Instance->CNT=0;
		Trig_L;
		Trig_H;
		Delay_ms(1);
		Trig_L;
		while(HAL_GPIO_ReadPin(Echo_GPIO_Port ,Echo_Pin)==GPIO_PIN_RESET)
		s1=htim2.Instance->CNT;
		while(HAL_GPIO_ReadPin(Echo_GPIO_Port ,Echo_Pin)==GPIO_PIN_SET)
		s2=htim2.Instance->CNT;
		last_distance=distance;
		last_raw_distance=raw_distance;		
		raw_distance=(s2-s1)*0.0034/2;
		if(raw_distance>20&&raw_distance<30&&Flag_ly!=3)
		{
			distance=raw_distance;
		}
		if(raw_distance>10&&raw_distance<25&&Flag_ly==3)
		{
			distance=raw_distance;
		}
		if(distance-last_distance>3||distance-last_distance<-3)
		{
			distance=last_distance;
		}
//		if(raw_distance-last_raw_distance>3||raw_distance-last_raw_distance<-3)
//		{
//			raw_distance=last_raw_distance;
//		}
		false_flag=0;

//		DEBUG_info("distance",":%0.1fcm\r\n",distance);
		OLED_Printf(0, 16, OLED_8X16, "distance:%f", distance);

}

void get_distance2(void)
{
		htim2.Instance->CNT=0;
		Trig_L;
		Trig_H;
		Delay_ms(1);
		Trig_L;
		while(HAL_GPIO_ReadPin(Echo_GPIO_Port ,Echo_Pin)==GPIO_PIN_RESET)
		s1=htim2.Instance->CNT;
		while(HAL_GPIO_ReadPin(Echo_GPIO_Port ,Echo_Pin)==GPIO_PIN_SET)
		s2=htim2.Instance->CNT;
		last_distance=distance;
		distance=(s2-s1)*0.0034/2;

		OLED_Printf(0, 16, OLED_8X16, "distance:%f", distance);

}