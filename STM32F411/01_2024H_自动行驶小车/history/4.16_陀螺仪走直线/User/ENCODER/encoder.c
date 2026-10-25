#include "encoder.h"
uint8_t Encoder_TIM;
uint8_t TIMX;
/**************************************************************************
Function: Read encoder count per unit time
Input   : TIMX：Timer
Output  : none
函数功能：单位时间读取编码器计数
入口参数：TIMX：定时器4/5，两路编码器
返回  值：速度值
**************************************************************************/
int Read_Encoder(uint8_t TIMX)
{       
   switch(TIMX)
	 {
//		 case 4:  Encoder_TIM = 100;  TIM4 -> CNT=0;break;	//读取定时器4对应编码器
	   case 4:  Encoder_TIM = (int)TIM4 -> CNT;  TIM4 -> CNT=0;break;	//读取定时器4对应编码器
		 case 5:  Encoder_TIM = (int)TIM5 -> CNT;  TIM5 -> CNT=0;break;	//读取定时器5对应编码器
		 default: Encoder_TIM=0;
		 
	 }
	  if(Encoder_TIM<0)   Encoder_TIM=-Encoder_TIM;          //将编码器读到的数据变为正数
		return Encoder_TIM;
}

//这个是自己加进去的
int Read_Speed(TIM_HandleTypeDef *htim)
{
	int temp;
	temp=(short)__HAL_TIM_GetCounter(htim);
	__HAL_TIM_SetCounter(htim,0);
	return temp;
}

/**************************************************************************
Function: TIM4 interrupt service function
Input   : none
Output  : none
函数功能：TIM4中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}
/**************************************************************************
Function: TIM2 interrupt service function
Input   : none
Output  : none
函数功能：TIM5中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void TIM5_IRQHandler(void)
{ 		    		  			    
	if(TIM5->SR&0X0001)//溢出中断
	{    				   				     	    	
	}				   
	TIM5->SR&=~(1<<0);//清除中断标志位 	    
}
