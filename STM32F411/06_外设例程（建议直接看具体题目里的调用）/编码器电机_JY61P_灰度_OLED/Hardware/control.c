#include "string.h"
#include "math.h"
#include "control.h"
#include "motor.h"

float Encoder_left, Encoder_right;
int encoder_cnt_flag = 0, ctrl_cnt_flag = 0, sensor_cnt_flag = 0;
float Voltage_temp, Voltage_count, Voltage_all, Voltage;
int Velocity_Pwm_left, Velocity_Pwm_right, Turn_Pwm, Accelerate_Pwm;
float Velocity_Kp = 30, Velocity_Ki = 8; //Velocity_Kp_right = 18, Velocity_Ki_right = 7.5;
float Turn_Kp_a = 130, Turn_Kp_b = 0.25, Turn_Kp_m = 300, Turn_Ki = 0.9, Turn_Kd = 500;
float Velocity_Target;
float Velocity_Target_Max = 500;
float Turn_Target = 0.0, Turn_Target_temp = 0.0;
extern float Velocity_Left,Velocity_Right;
extern int stop_flag, correct_flag;
int left_turn_90_flag = 0, right_turn_90_flag = 0, turn_90_flag = 0;
int sensor_stop = 0;
int time = 0, brake_time = 40, correct_time = 0, error_time = 0, stop_time = 30, turn_time = 0;
int Motor_left, Motor_right;
extern uint8_t sensor_data;
extern uint8_t sensor[8];
int direction;
//char message[2];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim5)
	{
		encoder_cnt_flag++;
		ctrl_cnt_flag++;
		if(encoder_cnt_flag == 5)
		{			
			Encoder_left = Read_Encoder(2);
			Encoder_right = -Read_Encoder(4);
			Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
			self_turn();
//			if(sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1 && sensor[3] == 1 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1 && sensor[7] == 1)
//			{
//				error_time++;
//				if(error_time >= 400)
//				{
//					stop_flag = 0;
//				}
//			}
//			else error_time = 0;
//					if(Turn_Target - Yaw > 0) Turn_Target = 15.0;
//					else if(Turn_Target - Yaw < 0) Turn_Target = -15.0;
//					correct_flag = 1;
			
			encoder_cnt_flag = 0;
		}
		if(ctrl_cnt_flag == 10)
		{
			Voltage_temp = Get_battery_volt();
			Voltage_count++;
			Voltage_all += Voltage_temp;
			if(Voltage_count == 99) Voltage = Voltage_all/100, Voltage_all = 0, Voltage_count = 0;
			
			if(stop_flag == 1)
			{ 
				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
//				STOP_turn90();
				if(turn_90_flag == 0)
				{
					if(Velocity_Target < Velocity_Target_Max)  Velocity_Target += 10.0;
					if(Velocity_Target >= Velocity_Target_Max)  Velocity_Target = Velocity_Target_Max;
					Turn_Target = gw_Sensor();
					Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,stop_flag);
					Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,stop_flag);
//				Set_Pwm(1000,2000);
					Turn_Pwm = Turn_PID(Yaw, Turn_Target,stop_flag);
					Motor_left = Velocity_Pwm_left - Turn_Pwm;
					Motor_right = Velocity_Pwm_right + Turn_Pwm;
					Motor_left = PWM_Limit(Motor_left, 8000, 500);
					Motor_right = PWM_Limit(Motor_right, 8000, 500);
					Set_Pwm(Motor_left,Motor_right);
					if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
					{
						correct_time++;
						Accelerate_Pwm += 50;
						if(correct_time >= 20)
						{
							correct_flag = 1, Turn_Target = 0;
						}
						if(Accelerate_Pwm > 3000) Accelerate_Pwm = 3000;
					}
					else correct_time = 0, Accelerate_Pwm = 0;
					if(Yaw > 160 || Yaw < -160)
					{
						if(fabsf(Turn_Target - Yaw) <= 15)
						{
							Turn_Target = 1.0*(Turn_Target - Yaw);
							correct_flag = 1;
						}
//					if(Turn_Target - Yaw > 0) Turn_Target = 15.0;
//					else if(Turn_Target - Yaw < 0) Turn_Target = -15.0;
//					correct_flag = 1;
					}
											
				}
				else if(turn_90_flag == 1)
				{
					Motor_left = Velocity_Pwm_left - Turn_Pwm;
					Motor_right = Velocity_Pwm_right + Turn_Pwm;
					Motor_left = PWM_Limit(Motor_left, 3000, -3000);
					Motor_right = PWM_Limit(Motor_right, 3000, -3000);
					Set_Pwm(Motor_left,Motor_right);
				}
//				if(Turn_Target > 180)
//				{
//					if(fabsf(Yaw - Turn_Target) < 1)	Turn_Target = -180;	
//					else Turn_Target = 180;
//				}
//				if(Turn_Target > 175 && fabsf(Yaw - Turn_Target) < 10) correct_flag = 1, Turn_Target = 0;
//				if(Turn_Target < -175 && fabsf(Yaw - Turn_Target) < 10) correct_flag = 1, Turn_Target = 0;
				if(Turn_Target > 179) Turn_Target = 179;
				if(Turn_Target < -179) Turn_Target = -179;
//				Motor_left = Velocity_Pwm_left;
//				Motor_right = Velocity_Pwm_right;


//				message[0] = Velocity_Left;
//				message[1] = Velocity_Right;
//				HAL_UART_Transmit(&huart6, (uint8_t*)message, 2, 100);
			}
			else
			{
				HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
				Turn_Pwm = 0;
				Velocity_Pwm_left = 0, Velocity_Pwm_right = 0;
				Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,stop_flag);
				Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,stop_flag);
				Turn_Pwm = Turn_PID(Yaw, Turn_Target,stop_flag);
				Set_Pwm(0,0);
			}
			ctrl_cnt_flag = 0;
		}
//		if(sensor_cnt_flag == 20)
//		{			
//			Turn_Target += 0.01*sensor[4] + 0.05*sensor[5] + 0.1*sensor[6] + 0.3*sensor[7];
//			Turn_Target -= 0.01*sensor[3] + 0.05*sensor[2] + 0.1*sensor[1] + 0.3*sensor[0];
//			sensor_cnt_flag = 0;
//		}
	}
}
void STOP_turn90(void)
{
	if(sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0 && sensor[7] == 0 && sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1)
	{
		right_turn_90_flag = -1;
		turn_90_flag = 1;
	}
	else if(sensor[3] == 0 && sensor[2] == 0 && sensor[1] == 0 && sensor[0] == 0 && sensor[7] == 1 && sensor[6] == 1 && sensor[5] == 1)
	{
		left_turn_90_flag = 1;
		turn_90_flag = 1;
	}
//	else if(sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0 && sensor[7] == 0 && sensor[3] == 0 && sensor[2] == 0 && sensor[1] == 0 && sensor[0] == 0)
//	{
//		left_turn_90_flag = 0;
//		right_turn_90_flag = 0;
//		turn_90_flag = 0;
//		time = 0;
//		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
//	}
	
	if(right_turn_90_flag == -1)
	{
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		turn_90_flag = 1;
		time++;
//		Velocity_Target = 150.0;
//		Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,stop_flag);
		if(time > 30)
		{
			if(sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1 && sensor[3] == 1 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1 && sensor[7] == 1)
			{
				right_turn_90_flag = -1;
				turn_90_flag = 1;
			}
			else
			{
				left_turn_90_flag = 0;
				right_turn_90_flag = 0;
				turn_90_flag = 0;
				time = 0;
				HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
			}
		}
	}
	else if(left_turn_90_flag == 1)
	{
		turn_90_flag = 1;
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		time++;
//		Velocity_Target = 150.0;
//		Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,stop_flag);
		if(time > 30)
		{
			if(sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1 && sensor[3] == 1 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1 && sensor[7] == 1)
			{
				left_turn_90_flag = 1;
				turn_90_flag = 1;
			}
			else
			{
				left_turn_90_flag = 0;
				right_turn_90_flag = 0;
				turn_90_flag = 0;
				time = 0;
				HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
			}
		}
	}	
	
	if(time >= brake_time && right_turn_90_flag < 0)
	{
		Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,0);
		Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,0);
		Turn_Target -= 0.8;
		Turn_Pwm = Turn_PID(Yaw, Turn_Target,stop_flag);
		Turn_Pwm = PWM_Limit(Turn_Pwm, 2700, -2700);
		if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
		{
			time++;
			if(time > brake_time + 5)
			{
				if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
				{
					HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
					turn_90_flag = 0;
					right_turn_90_flag = 0, time = 0;
				}
			}
		}
	}	
	else if(time >= brake_time && left_turn_90_flag > 0)
	{
		Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,0);
		Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,0);
		Turn_Target += 0.8;
		Turn_Pwm = Turn_PID(Yaw, Turn_Target,stop_flag);
		Turn_Pwm = PWM_Limit(Turn_Pwm, 2700, -2700);
		if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
		{
			time++;
			if(time > brake_time + 5)
			{
				if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
				{
					HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
					turn_90_flag = 0;
					right_turn_90_flag = 0, time = 0;
				}
			}
		}
	}
}

float gw_Sensor(void)
{
	float Kp_in, Kp_out1, Kp_out2;
	Kp_in = 0.2, Kp_out1 = 1, Kp_out2 = 3;
	if(turn_90_flag == 0 && stop_flag == 1)
	{
//		if((sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0 && sensor[7] == 0) || (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0)) 
//		{
//			Turn_Target = Turn_Target;
//		}
//		else sensor_stop = 0;
		if(sensor_stop == 0)
		{
//			Turn_Target = -(Kp_in*sensor[5] + Kp_out1*sensor[6] + Kp_out2*sensor[7]);
//			Turn_Target = (Kp_in*sensor[2] + Kp_out1*sensor[1] + Kp_out2*sensor[0]);
	
//			Turn_Target += (0.05*sensor[4] + 0.1*sensor[5] + 1.5*sensor[6] + 3.2*sensor[7]);
//			Turn_Target -= (0.05*sensor[3] + 0.1*sensor[2] + 1.5*sensor[1] + 3.2*sensor[0]);
			Turn_Target += (0.1*sensor[4] + 0.8*sensor[5] + 2.5*sensor[6] + 6.5*sensor[7]);
			Turn_Target -= (0.1*sensor[3] + 0.8*sensor[2] + 2.5*sensor[1] + 6.5*sensor[0]);
		}
		if(sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1 && sensor[7] == 1 && sensor[3] == 0 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1)
//			if(sensor[4] == 0 && sensor[3] == 0)
		{
			Turn_Target = Yaw;
		}
//				Turn_Target = 0.3*Turn_Target_temp + 0.7*Turn_Target;
//	}
		if(Turn_Target > 179) Turn_Target = 179;
		if(Turn_Target < -179) Turn_Target = -179;
//	if(Turn_Target > 180)
//	{
//		if(fabsf(Yaw - Turn_Target) < 1)	Turn_Target = -180;	
//		else Turn_Target = 180;
//	}
	}
	return Turn_Target;
}

void self_turn(void)
{
	if(sensor[0] == 0)  direction = 1;
	if(sensor[7] == 0)  direction = -1;
	if(sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1 && sensor[3] == 1 && sensor[2] == 1 && sensor[1] == 1 && sensor[0] == 1 && sensor[7] == 1)
	{
			time++;
//			if(time >= 1 && time < stop_time)
//			{
//				Turn_Target_temp = Turn_Target;
//				if(Turn_Target_temp - Yaw > 1)  direction += 1;
//				else if(Turn_Target_temp - Yaw < -1)  direction -= 1;
//				if(Turn_Target_temp - Yaw > 2)  direction = 1;
//				else if(Turn_Target_temp - Yaw < -2)  direction = -1;
//				else if(Turn_Target_temp - Yaw > -2 && Turn_Target_temp - Yaw < 2)
//				{
//					HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
//					turn_90_flag = 0;
//					time = 0;
//					Turn_Target_temp = 0;
//				}
//				if(Velocity_Left > 350 && Velocity_Left > 350)  stop_time = 40;
//				else  stop_time = 80;
//			}
	}
	if(time >= stop_time)
	{
		turn_90_flag = 1;
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		turn_time++;
//		if(direction > 0)  direction = 1;
//		else if(direction < 0) direction = -1;
//		if(time == 40)
//		{
//			Turn_Target_temp = Turn_Target;
//			if(Turn_Target_temp - Yaw > 0)  direction = 1;
//			else if(Turn_Target_temp - Yaw < 0)  direction = -1;
//		}
		Velocity_Pwm_left = Velocity_PI_left(Velocity_Left,Velocity_Target,0);
		Velocity_Pwm_right = Velocity_PI_right(Velocity_Right,Velocity_Target,0);
		Turn_Target += direction*0.8;
//		if(turn_time > 160)
//		{
//			if(direction == 1)  direction = -1;
//			else if(direction == -1)  direction = 1;
//			turn_time = 0;
//		}
		Turn_Pwm = Turn_PID(Yaw, Turn_Target,stop_flag);
		Turn_Pwm = PWM_Limit(Turn_Pwm, 2700, -2700);
		if(sensor[4] == 0 || sensor[3] == 0)
		{
			time++;
			if(time > stop_time + 10)
			{
				HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
				turn_90_flag = 0;
				time = 0;
				turn_time = 0;
				Turn_Target_temp = 0;
			}
		}
	}
	else if(time < stop_time && (sensor[0] == 0 || sensor[7] == 0))
	{
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
		turn_90_flag = 0;
		time = 0;
//		direction = 0;
		Turn_Target_temp = 0;
	}
}
		
			