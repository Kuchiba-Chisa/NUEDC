#include "main.h"
#include "distance.h"
#include "debug.h"
#include "OLED.h"
#include "delay.h"
extern TIM_HandleTypeDef htim2;
int s1=0;
int s2=0;
float distance=0.0;
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
		distance=(s2-s1)*0.0034/2;
//		DEBUG_info("distance",":%0.1fcm\r\n",distance);
		OLED_Printf(0, 16, OLED_8X16, "distance:%f", distance);

}
