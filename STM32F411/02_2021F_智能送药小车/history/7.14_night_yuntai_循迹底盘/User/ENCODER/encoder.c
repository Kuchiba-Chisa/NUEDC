#include "encoder.h"
#include "tim.h"
uint16_t Encoder_TIM;
uint16_t TIMX;
float speedA;
float speedB;
volatile float Velocity_Left,Velocity_Right;

float Read_Encoder(uint8_t TIMX)
{       
   switch(TIMX)
	 {
	   case 4:  
				Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim4);  
				__HAL_TIM_SetCounter(&htim4,0);
				return Encoder_TIM;	//读取定时器4对应编码器
		 case 5:  
				Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim5);  
				__HAL_TIM_SetCounter(&htim5,0);
				return Encoder_TIM;	//读取定时器4对应编码器
		 default: Encoder_TIM=0;
	 }
	 return Encoder_TIM;
}

float Read_Encoder_no_zero(uint8_t TIMX)
{       
   switch(TIMX)
	 {
	   case 4:  
				Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim4);  
//				__HAL_TIM_SetCounter(&htim4,0);
				return Encoder_TIM;	//读取定时器4对应编码器
		 case 5:  
				Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim5);  
//				__HAL_TIM_SetCounter(&htim5,0);
				return Encoder_TIM;	//读取定时器4对应编码器
		 default: Encoder_TIM=0;
	 }
	 return Encoder_TIM;
}

void Get_Velocity_Form_Encoder(int Encoder_left,int Encoder_right)
{
	float Rotation_Speed_L,Rotation_Speed_R;						//电机转速  转速=编码器读数（5ms每次）*读取频率/倍频数/减速比/编码器精度
	Rotation_Speed_L = Encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = 0.3*Rotation_Speed_L*PI*Diameter_67 + 0.7*Velocity_Left;		//求出编码器速度=转速*周长
	Rotation_Speed_R = Encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = 0.3*Rotation_Speed_R*PI*Diameter_67 + 0.7*Velocity_Right;		//求出编码器速度=转速*周长
	if(Velocity_Right>2000||Velocity_Right<-2000)
	{
		Velocity_Right=0;
	}
	if(Velocity_Left>2000||Velocity_Left<-2000)
	{
		Velocity_Left=0;
	}
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
