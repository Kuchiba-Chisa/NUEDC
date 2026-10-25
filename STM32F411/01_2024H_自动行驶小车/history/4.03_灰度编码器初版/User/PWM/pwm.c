#include "pwm.h"



/*************************************************************
函数功能：PWM赋值
入口参数：motor_left:左电机PWM值，motor_right:右电机PWM值
返回  值：无
注意： 这里411设的ARR值为9999，意味着PWM取值0~10000
*************************************************************/
void Set_PWM(int motor_left,int motor_right)
{
   if(motor_left>0)   
   {
	   PWMA_IN1=10000;
	   PWMA_IN2=10000-motor_left;
   }
   else
   {
	   PWMA_IN1=10000+motor_left;
	   PWMA_IN2=10000;
   }
   if(motor_right>0)
   {
	   PWMB_IN1=10000;
	   PWMB_IN2=10000-motor_right;
   }
   else
   {
	   PWMB_IN1=10000+motor_right;
	   PWMB_IN2=10000;
   }
	
	
}



