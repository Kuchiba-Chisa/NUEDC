#include "control.h"
#include "ti/driverlib/dl_gpio.h"
#include "math.h"
#include "stdlib.h"

int encoder_cnt, ctrl_cnt_flag;
extern int32_t gEncoderCount_L1, gEncoderCount_L2;
int Encoder_left,Encoder_right;
uint8_t sensor[9];
extern char tempcenter_point[2];
extern int stop_flag, stand_flag, correct_flag, mode, correct_yaw;
extern float Velocity_Left,Velocity_Right;
extern float Yaw, Roll, Gyro_x, Gyro_z;
int Motor_left, Motor_right;
int Middle_angle=0;  
float Balance_Kp=27000,Balance_Kd=2600,Velocity_Kp=4300,Velocity_Ki=21,Turn_Kp=3000,Turn_Kd=100, Turn_Ki=10;//PID参数（放大100倍）
float chasu_kp = 130, chasu_kd = 4500;
float Velocity_Target = 0;
int Velocity_Target_Max = 500;
int Balance_Pwm, Velocity_Pwm, Turn_Pwm, Chasu_Pwm;		  					//平衡环PWM变量，速度环PWM变量，转向环PWM变量
float Turn_Target = 0.0;
float Turn_Target_Max = 0.0;
int Inline = 0, last_state = 0;
int cam_point[2];
int cam_middle = 160;
int Min_PWM = 4000;
int tip_time = 0, tip_flag = 0;
int correct_yaw_flag = 0, line_correct = 0, correct_time = 0, correct_wait = 0;
int chasu_error = 0;
int state_num = 0;
int circle_num = 0;
int err_time;

void TIMER_0_INST_IRQHandler(void)
{
    encoder_cnt++;
    ctrl_cnt_flag++;
    if(encoder_cnt == 5)
    {
        Encoder_left = gEncoderCount_L1;
        Encoder_right = gEncoderCount_L2;
        gEncoderCount_L1 = 0;
        gEncoderCount_L2 = 0;
        Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
        // if(Inline == 1)
        // {
        //     Turn_Target = Cam_Sensor();  
        // }
        encoder_cnt = 0;
    }
    if(ctrl_cnt_flag == 10)
    {
        if(stand_flag == 1)
        {
            // DL_GPIO_clearPins(led2_PORT, led2_PIN_7_orange_PIN);
		    Balance_Pwm = Balance(Roll,Gyro_x);    //平衡PID控制 Gyro_Balance平衡角速度极性：前倾为正，后倾为负
            Velocity_Pwm = Velocity(Velocity_Left,Velocity_Right,stand_flag,Velocity_Target);  //速度环PID控制	记住，速度反馈是正反馈，就是小车快的时候要慢下来就需要再跑快一点
            Motor_left = Balance_Pwm + Velocity_Pwm + Chasu_Pwm + Turn_Pwm;       //计算左轮电机最终PWM
		    Motor_right = Balance_Pwm + Velocity_Pwm - Chasu_Pwm - Turn_Pwm;      //计算右轮电机最终PWM
            Motor_left = PWM_Limit(Motor_left,4000,-Min_PWM);
		    Motor_right = PWM_Limit(Motor_right,4000,-Min_PWM);			//PWM限幅
            Set_Pwm(Motor_left,Motor_right);         					//赋值给PWM寄存器 
            if(stop_flag == 1)
		    { 

                DL_GPIO_clearPins(led2_PORT, led2_PIN_2_green_PIN);
                if(mode != 3)
                {
                    Velocity_Target = 30;
                    Middle_angle = -1;
                }
                else if(mode == 3)
                {
                    Velocity_Target = 50;
                    Middle_angle = -1;                    
                }
                if(Yaw < -179) Yaw = -179;
                err_time++;
                if(err_time > 400)  err_time = 400;
                if(Inline == 0 && fabsf(Turn_Target - Yaw) < 12 && line_correct == 1)
				{
                    
                    correct_yaw = 1;
                    correct_wait = 1;
                    line_correct = 0;
                    if(mode == 1 && state_num== 2)
                    {
                        Turn_Target = -8;
                    }
                    else if(mode == 2 && state_num != 0 && (state_num % 4 == 2))
                    {
                        Turn_Target = 47;
                        if(Turn_Target - Yaw > -12 && Turn_Target - Yaw <= -9)
                        {
                            Turn_Target -= 6.6;
                        }
                        else if(Turn_Target - Yaw > -9 && Turn_Target - Yaw <= -6)
                        {
                            Turn_Target -= 4.95;
                        }
                        else if(Turn_Target - Yaw > -6 && Turn_Target - Yaw <= -3)
                        {
                            Turn_Target -= 3.3;
                        }
                        else if(Turn_Target - Yaw > -3 && Turn_Target - Yaw < 0)
                        {
                            Turn_Target -= 1.65;
                        }
                        else if(Turn_Target - Yaw >= 9 && Turn_Target - Yaw < 12)
                        {
                            Turn_Target += 6.6;
                        }
                        else if(Turn_Target - Yaw >= 6 && Turn_Target - Yaw < 9)
                        {
                            Turn_Target += 4.95;
                        }
                        else if(Turn_Target - Yaw >= 3 && Turn_Target - Yaw < 6)
                        {
                            Turn_Target += 3.3;
                        }
                        else if(Turn_Target - Yaw > 0 && Turn_Target - Yaw < 3)
                        {
                            Turn_Target += 1.65;
                        }
                    }
                    else if(mode == 3 && state_num != 0 && (state_num % 4 == 2))     
                    {
                        Turn_Target = 47;
                        if(Turn_Target - Yaw > -12 && Turn_Target - Yaw <= -9)
                        {
                            Turn_Target -= 6.6;
                        }
                        else if(Turn_Target - Yaw > -9 && Turn_Target - Yaw <= -6)
                        {
                            Turn_Target -= 4.95;
                        }
                        else if(Turn_Target - Yaw > -6 && Turn_Target - Yaw <= -3)
                        {
                            Turn_Target -= 3.3;
                        }
                        else if(Turn_Target - Yaw > -3 && Turn_Target - Yaw < 0)
                        {
                            Turn_Target -= 1.65;
                        }
                        else if(Turn_Target - Yaw >= 9 && Turn_Target - Yaw < 12)
                        {
                            Turn_Target += 6.6;
                        }
                        else if(Turn_Target - Yaw >= 6 && Turn_Target - Yaw < 9)
                        {
                            Turn_Target += 4.95;
                        }
                        else if(Turn_Target - Yaw >= 3 && Turn_Target - Yaw < 6)
                        {
                            Turn_Target += 3.3;
                        }
                        else if(Turn_Target - Yaw > 0 && Turn_Target - Yaw < 3)
                        {
                            Turn_Target += 1.65;
                        }
                    }
                    
                    else if(mode == 3 && state_num != 0 && (state_num % 4 == 0))
                    {     
                        Turn_Target = -47;
                        if(Turn_Target - Yaw > -12 && Turn_Target - Yaw <= -9)
                        {
                            Turn_Target += 3.6;
                        }
                        else if(Turn_Target - Yaw > -9 && Turn_Target - Yaw <= -6)
                        {
                            Turn_Target += 2.7;
                        }
                        else if(Turn_Target - Yaw > -6 && Turn_Target - Yaw <= -3)
                        {
                            Turn_Target += 1.8;
                        }
                        else if(Turn_Target - Yaw > -3 && Turn_Target - Yaw < 0)
                        {
                            Turn_Target += 0.9;
                        }
                        else if(Turn_Target - Yaw >= 9 && Turn_Target - Yaw < 12)
                        {
                            Turn_Target -= 3.6;
                        }
                        else if(Turn_Target - Yaw >= 6 && Turn_Target - Yaw < 9)
                        {
                            Turn_Target -= 2.7;
                        }
                        else if(Turn_Target - Yaw >= 3 && Turn_Target - Yaw < 6)
                        {
                            Turn_Target -= 1.8;
                        }
                        else if(Turn_Target - Yaw > 0 && Turn_Target - Yaw < 3)
                        {
                            Turn_Target -= 0.9;
                        }
                    }
				}
                if(correct_wait == 1)
                {
                    correct_time++;
                    if(correct_time > 20 && mode == 1)
                    {
                        correct_wait = 0;
                        correct_time = 0;
                    }
                    else if(correct_time > 10 && (mode == 2 ||mode == 3))
                    {
                        correct_wait = 0;   
                        correct_time = 0;          
                    }
                }
                State();
                if(Inline == 1)
                {
                    Chasu_Pwm = Chasu_PID(chasu_error, 0);
                    Turn_Pwm = 0;                
                    // if(fabsf(Yaw) < 1) Turn_Target = Yaw;
            	} 
                else if(Inline == 0 && correct_wait == 0 && line_correct == 0)
                {
                    if(correct_yaw_flag != 1)  Turn_Pwm = Turn(Yaw, Gyro_z, Turn_Target);
                    Chasu_Pwm = 0;
                }
        	}
			if(stop_flag == 0)
            {
                DL_GPIO_setPins(led2_PORT, led2_PIN_2_green_PIN);
                cam_point[0] = 0, cam_point[1] = 0;
                Chasu_Pwm = 0;
                Turn_Pwm = 0;
                Velocity_Target = 0;
                Middle_angle = 0;
                Inline = 0;
                state_num = 0;
                circle_num = 0;
                if(tip_flag == 1)  
                {
                    tip_time++;                    
                    if(tip_time > 30)
                    {
                        DL_GPIO_clearPins(buzzer_PORT, buzzer_PIN_19_PIN);
                        DL_GPIO_setPins(led_PORT, led_PIN_18_red_PIN);
                        tip_flag = 0;
                        tip_time = 0;
                    }
                }
                if(line_correct == 1)
				{
                    correct_yaw = 1;
                    Turn_Target = 0;
                    line_correct = 0;
				}
                if(correct_yaw_flag != 1)  Turn_Pwm = Turn(Yaw, Gyro_z, Turn_Target);

            } 
		}
        else 
		{
            DL_GPIO_setPins(led2_PORT, led2_PIN_7_orange_PIN);
            Motor_left = 0;
            Motor_right = 0;
            Balance_Pwm = 0;
            Velocity_Pwm = Velocity(0,0,stand_flag,0);
            Chasu_Pwm = 0;
            Turn_Pwm = 0;
            Set_Pwm(0, 0);
//			DL_GPIO_clearPins(buzzer_PORT, buzzer_PIN_19_PIN);
        }
        ctrl_cnt_flag = 0;
   	}
}


/**************************************************************************
Function: Vertical PD control
Input   : Angle:angle；Gyro：angular velocity
Output  : balance：Vertical control PWM
函数功能：直立PD控制		
入口参数：Angle:角度；Gyro：角速度
返回  值：balance：直立控制PWM
**************************************************************************/	
int Balance(float Angle,float Gyro)
{  
    float Angle_bias,Gyro_bias;
	int balance;
	Angle_bias=Middle_angle-Angle;                       				//求出平衡的角度中值 和机械相关
	Gyro_bias=0-Gyro; 
	balance=Balance_Kp/100*Angle_bias + Gyro_bias*Balance_Kd/100; //计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	return balance;
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
int Velocity(int encoder_left,int encoder_right,int flag, float Target)
{  
    static float velocity,Encoder_Least,Encoder_bias,Movement;
	static float Encoder_Integral;
    Movement = Target; //周长/编码器读取频率*频数*减速比*精度*2;		
		
   //================速度PI控制器=====================//	
		Encoder_Least =0-(encoder_left+encoder_right)/2;                    //获取最新速度偏差=目标速度（此处为零）-测量速度（左右编码器之和） 
		Encoder_bias *= 0.7;		                                          //一阶低通滤波器       
		Encoder_bias += Encoder_Least*0.3;	                              //一阶低通滤波器，减缓速度变化 
		Encoder_Integral += Encoder_bias;                                  //积分出位移 积分时间：10ms
		Encoder_Integral=Encoder_Integral+Movement;                       //接收遥控器数据，控制前进后退
		if(Encoder_Integral>4000)  	Encoder_Integral=4000;            //积分限幅
		if(Encoder_Integral<-4000)	  Encoder_Integral=-4000;         //积分限幅	
		velocity = -Encoder_bias*Velocity_Kp/100 - Encoder_Integral*Velocity_Ki/100;     //速度控制
		if(flag==0) Encoder_Integral=0;//电机关闭后清除积分
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
int Turn(float Yaw, float gyro, float Target)
{
	float turn,Turn_Amplitude=54;
	float Kp=Turn_Kp, Kd0=Turn_Kd, Kd1 = Turn_Kd;			//修改转向速度，请修改Turn_Amplitude即可
    float turn_error, pre_turn_error, turn_Integral;
	turn_error = Target - Yaw;
	//  if(1==Flag_front||1==Flag_back)  Kd=Turn_Kd;        
	//  else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
  //===================转向PD控制器=================//
    // if(fabsf(turn_error) < 5)
    // {
    //     turn_Integral += Turn_Ki/100*turn_error;
    //     turn = turn_error*Turn_Kp + turn_Integral + (turn_error - pre_turn_error)*Kd1;
    // }
	// else 
    turn = -turn_error*Kp/100+gyro*Kd0/100;//结合Z轴陀螺仪进行PD控制
    pre_turn_error = turn_error;
	return turn;								 				 //转向环PWM右转为正，左转为负
}

// float Cam_Sensor(void)
// {
// 	float Kp_in, Kp_out1, Kp_out2;
    
// 	if(Inline == 1)
// 	{
//         Turn_Target += (0.1*sensor[3] + 0.4*sensor[2] + 0.6*sensor[1] + 0.8*sensor[0]);
//         Turn_Target -= (0.1*sensor[5] + 0.4*sensor[6] + 0.6*sensor[7] + 0.8*sensor[8]);
//         if(sensor[4] == 1)
//         {
//             Turn_Target = Yaw;
//         }
//         if(Turn_Target > 179) Turn_Target = 179;
//         if(Turn_Target < -179) Turn_Target = -179;
// 	}
	
// 	return Turn_Target;
// }
void State(void)
{
    cam_point[0] = tempcenter_point[0]*320/255;
	cam_point[1] = tempcenter_point[1]*320/255;
	if(stop_flag == 1)
	{
        if(cam_point[1] > 0 && cam_point[1] < 38)  sensor[0] = 1, chasu_error = -7;
        else sensor[0] = 0;
        if(cam_point[1] >= 38 && cam_point[1] < 76)  sensor[1] = 1, chasu_error = -5;
        else sensor[1] = 0;
        if(cam_point[1] >= 76 && cam_point[1] < 114)  sensor[2] = 1, chasu_error = -3;
        else sensor[2] = 0;
        if(cam_point[1] >= 114 && cam_point[1] < 152)  sensor[3] = 1, chasu_error = -1;
        else sensor[3] = 0;
        if(cam_point[1] >= 145 && cam_point[1] < 175)  sensor[4] = 1, chasu_error = 0;
        else sensor[4] = 0;
        if(cam_point[1] >= 168 && cam_point[1] < 206)  sensor[5] = 1, chasu_error = 1;
        else sensor[5] = 0;
        if(cam_point[1] >= 206 && cam_point[1] < 244)  sensor[6] = 1, chasu_error = 3;
        else sensor[6] = 0;
        if(cam_point[1] >= 244 && cam_point[1] < 282)  sensor[7] = 1, chasu_error = 5;
        else sensor[7] = 0;
        if(cam_point[1] >= 282 && cam_point[1] < 320)  sensor[8] = 1, chasu_error = 7;
        else sensor[8] = 0;  
    }  
    if(sensor[0] == 1 || sensor[1] == 1 || sensor[2] == 1 || sensor[3] == 1 || sensor[4] == 1 || sensor[5] == 1 || sensor[6] == 1 || sensor[7] == 1 || sensor[8] == 1)
    {
        
        Inline = 1;
        
    }
    else if(sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0 && sensor[7] == 0 || sensor[8] == 0) 
    {

        Inline = 0;
    }
    if(last_state != Inline)
    {
        if(err_time >200)
        {
            err_time = 0;
            DL_GPIO_setPins(buzzer_PORT, buzzer_PIN_19_PIN);
            DL_GPIO_clearPins(led_PORT, led_PIN_18_red_PIN);
            if(mode == 0)
            {
                stop_flag = 0;
            }
            if(mode == 1)
            {
                state_num++;
                if(state_num == 2)
                {
                    line_correct = 1;
                    Turn_Target = -179;
                }
                if(state_num == 4)
                {
                    line_correct = 1;
                    Turn_Target = -179;
                    circle_num ++;
                }
                if(circle_num == 1)
                {
                    line_correct = 1;
                    Turn_Target = -179;
                    stop_flag = 0;
                }
            }
            if(mode == 2)
            {
                state_num++;
                if(state_num == 2)
                {
                    line_correct = 1;
                    Turn_Target = -150;
            }
                if(state_num == 4)
                {
                    line_correct = 1;
                    Turn_Target = 150;
                    circle_num ++;
                }
                if(circle_num == 1)
                {
                    line_correct = 1;
                    Turn_Target = -179;
                    stop_flag = 0;
                }
            }
            if(mode == 3)
            {
                state_num++;
                if(state_num != 0 && state_num == 2)
                {
                    line_correct = 1;
                    Turn_Target = -150;
                }
                else if(state_num != 0 && state_num > 2 && state_num % 4 == 2)
                {
                    line_correct = 1;
                    Turn_Target = -150;
                }
                else if(state_num != 0 && (state_num % 4 == 0))
                {
                    line_correct = 1;
                    Turn_Target = 150;
                    circle_num ++;
                }
        
                if(circle_num == 4)
                {
                    line_correct = 1;
                    Turn_Target = -179;
                    stop_flag = 0;
                }
            }
        }
        else 
        {
            Inline = last_state;
        }
        tip_flag = 1;
    }
    if(tip_flag == 1)
    {
        tip_time++;
        if(tip_time > 30)
        {
            DL_GPIO_clearPins(buzzer_PORT, buzzer_PIN_19_PIN);
            DL_GPIO_setPins(led_PORT, led_PIN_18_red_PIN);
            tip_flag = 0;
            tip_time = 0;
        }
        else if(tip_time == 1 && (state_num % 2 == 1) && (state_num > 1))
        {
            correct_yaw = 1;
            Turn_Target = 0;
        }
    }
    last_state = Inline;
    
}
// void State(char data0, char data1)
// {
//     if((data0 == 0 && data1 > 0) || (data0 > 0 && data1 == 0))
//     {
//         if(Inline == 0) Inline = 1;
//         else if(Inline == 1) Inline = 0;
//     }
// }
int Chasu_PID(int error,int target)   //差速PID控制器//这里Encoder=error，Target=0;
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=error-target;                                  //计算偏差
	//  Integral_bias+=Bias;	                                 //求出偏差的积分
	 Pwm=chasu_kp*Bias + chasu_kd*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                       //保存上一次偏差 
	 return Pwm;                                           //返回PWM值
}