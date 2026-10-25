#include "motor.h"
#include "math.h"
float Velocity_Left,Velocity_Right;
extern float Velocity_Kp, Velocity_Ki, Turn_Kp_a, Turn_Kp_b, Turn_Kp_m, Turn_Ki, Turn_Kd;
extern int stop_flag;
extern float Velocity_Target_Max;
int Speed_PWM_Max, Turn_PWM_Max;
/**************************************************************************
Function: Encoder reading is converted to speed (mm/s)
Input   : none
Output  : none
函数功能：编码器读数转换为速度（mm/s）
入口参数：无
返回  值：无
**************************************************************************/
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right)
{ 	
	
	float Rotation_Speed_L,Rotation_Speed_R;						//电机转速  转速=编码器读数（5ms每次）*读取频率/倍频数/减速比/编码器精度
	Rotation_Speed_L = encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = 0.3*Rotation_Speed_L*PI*Diameter_67 + 0.7*Velocity_Left;		//求出编码器速度=转速*周长
	Rotation_Speed_R = encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = 0.3*Rotation_Speed_R*PI*Diameter_67 + 0.7*Velocity_Right;		//求出编码器速度=转速*周长
}


/**************************************************************************
Function: Assign to PWM register
Input   : motor_left：Left wheel PWM；motor_right：Right wheel PWM
Output  : none
函数功能：赋值给PWM寄存器
入口参数：左轮PWM、右轮PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_left,int motor_right)
{
  if(motor_left>0)		
	{
		PWMA_IN1=10000;
		PWMA_IN2=10000-motor_left;//左轮前进
	}		
	else 
	{
		PWMA_IN1=10000+motor_left;
		PWMA_IN2=10000;
	} //左轮后退
  if(motor_right>0)			
	{
		PWMB_IN1=10000-motor_right;
		PWMB_IN2=10000;
	}		//右轮前进
	else 	        			  
	{
		PWMB_IN1=10000;
		PWMB_IN2=10000+motor_right;

	}//右轮后退
	
}
/**************************************************************************
Function: PWM limiting range
Input   : IN：Input  max：Maximum value  min：Minimum value
Output  : Output
函数功能：限制PWM赋值 
入口参数：IN：输入参数  max：限幅最大值  min：限幅最小值
返回  值：限幅后的值
**************************************************************************/
int PWM_Limit(int IN,int max,int min)
{
	int OUT = IN;
	if(OUT>max) OUT = max;
	if(OUT<min) OUT = min;
	return OUT;
}

/**************************************************************************
Function: Speed PI control
Input   : encoder_left：Left wheel encoder reading；encoder_right：Right wheel encoder reading
Output  : Speed control PWM
函数功能：速度控制PWM		
入口参数：encoder_left：左轮编码器读数；encoder_right：右轮编码器读数
返回  值：速度控制PWM
**************************************************************************/
//修改前进后退速度，请修改Target_Velocity，比如，改成60就比较慢了
float Velocity_PI_left(float Velocity,float Target, int flag)
{  
		Speed_PWM_Max = 11*Velocity_Target_Max;
    static float velocity,Encoder_Last_bias,Encoder_bias, Encoder_Integral;
		Encoder_bias = Target-Velocity;
		Encoder_Integral = Velocity_Ki*Encoder_bias;
		velocity += Velocity_Kp*(Encoder_bias-Encoder_Last_bias) + Encoder_Integral;     //速度控制
		Encoder_Last_bias = Encoder_bias;
//		if(Encoder_Integral > 4500) Encoder_Integral = 4500;
//		if(Encoder_Integral < -4500) Encoder_Integral = -4500;
		if(velocity > Speed_PWM_Max) velocity = Speed_PWM_Max;
		if(velocity < -4000) velocity = -4000;
		if(flag == 0) Encoder_Integral=0, velocity = 0;//电机关闭后清除积分
	  return velocity;
}
float Velocity_PI_right(float Velocity,float Target, int flag)
{  
		Speed_PWM_Max = 11*Velocity_Target_Max;
    static float velocity,Encoder_Last_bias,Encoder_bias, Encoder_Integral;
		Encoder_bias = Target-Velocity;
		Encoder_Integral = Velocity_Ki*Encoder_bias;
		velocity += Velocity_Kp*(Encoder_bias-Encoder_Last_bias) + Encoder_Integral;     //速度控制
		Encoder_Last_bias = Encoder_bias;
//		if(Encoder_Integral > 4500) Encoder_Integral = 4500;
//		if(Encoder_Integral < -4500) Encoder_Integral = -4500;
		if(velocity > Speed_PWM_Max) velocity = Speed_PWM_Max;
		if(velocity < -4000) velocity = -4000;
		if(flag == 0) Encoder_Integral=0, velocity = 0;//电机关闭后清除积分
	  return velocity;
}
/**************************************************************************
Function: Turn control
Input   : Z-axis angular velocity
Output  : Turn control PWM
函数功能：转向控制 
入口参数：Z轴陀螺仪
返回  值：转向控制PWM
作    者：轮趣科技（东莞）有限公司
**************************************************************************/
int Turn_PID(float yaw, float Turn_Target, int flag)
{
	 Turn_PWM_Max = Speed_PWM_Max + 2000;
	 static float turn, turn_error, pre_turn_error, turn_Integral;
//	 Turn_Amplitude=54;
	 turn_error = Turn_Target - yaw;
	 if(fabsf(turn_error) < 8)
	 {
		 turn_Integral += Turn_Ki*turn_error;
		 turn = turn_error*Turn_Kp_m + turn_Integral + (turn_error - pre_turn_error)*Turn_Kd;
	 }
//	 else	if(fabsf(turn_error) < 3)
//	 {
//		 turn_Integral += (Turn_Ki+20)*turn_error;
//		 turn = turn_error*(Turn_Kp_m+1500) + turn_Integral + (turn_error - pre_turn_error)*Turn_Kd;
//	 }
	 else 
	 { 
		 turn = turn_error*Turn_Kp_a + turn_error*turn_error*turn_error*Turn_Kp_b + (turn_error - pre_turn_error)*Turn_Kd;
		 turn_Integral = 0;
	 }
	 pre_turn_error = turn_error;
	 if(turn_Integral > 8000) turn_Integral = 8000;
	 if(turn_Integral < -8000) turn_Integral = -8000;
	 if(fabsf(turn_error) < 0.1) turn_Integral=0;
	 if(flag == 0) turn_Integral=0, turn=0;//电机关闭后清除积分
	 if(turn > Turn_PWM_Max) turn = Turn_PWM_Max;
	 if(turn < -Turn_PWM_Max) turn = -Turn_PWM_Max;
	 return turn;								 				 //转向环PWM右转为正，左转为负
}

