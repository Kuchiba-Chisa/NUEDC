#include "main.h"
#include "pid.h"
#include "Stepper_Motor.h"
#include "math.h"
#include "OLED.h"

#define STEPS_PER_DEGREE_X	5.14
#define STEPS_PER_DEGREE_Y	5
#define MAX_ANGLE_X        180     // X轴最大角度(度)
#define MAX_ANGLE_Y         90    // Y轴最大角度(度)
#define BASESPEED			30
// X轴PID参数
float Kp_x = 1.0;       // 比例系数
float Ki_x = 0.01;      // 积分系数
float Kd_x = 0.5;      // 微分系数
float integral_x = 0;   // 积分项
float prev_error_x = 0; // 上一次误差
float setpoint_x = 0;   // 目标位置
float output_x = 0;     // PID输出
float output_max_x = 100; // 输出上限
float output_min_x = -100; // 输出下限
uint32_t rpm_x;
uint32_t rpm_y;
// Y轴PID参数
float Kp_y = 1.0;       // 比例系数
float Ki_y = 0.01;      // 积分系数
float Kd_y = 0.5;      // 微分系数
float integral_y = 0;   // 积分项
float prev_error_y = 0; // 上一次误差
float setpoint_y = 0;   // 目标位置
float output_y = 0;     // PID输出
float output_max_y = 100; // 输出上限
float output_min_y = -100; // 输出下限

// 当前位置和目标位置
float current_pos_x = 0; // X轴当前位置(度)
float current_pos_y = 0; // Y轴当前位置(度)
float target_pos_x = 0;  // X轴目标位置(度)
float target_pos_y = 0;  // Y轴目标位置(度)
extern int current_x,current_y;
	
void Gimbal_Set_Target(float target_x, float target_y) {
    // 限制目标在合理范围内
    if (target_x > MAX_ANGLE_X) target_x = MAX_ANGLE_X;
    if (target_x < -MAX_ANGLE_X) target_x = -MAX_ANGLE_X;
    if (target_y > MAX_ANGLE_Y) target_y = MAX_ANGLE_Y;
    if (target_y < -MAX_ANGLE_Y) target_y = -MAX_ANGLE_Y;
    
    target_pos_x = target_x;
    target_pos_y = target_y;
    
    // 设置PID目标值
    setpoint_x = target_x;
    setpoint_y = target_y;
}


float PID_Calculate_X(float measurement) {
    // 计算误差
    float error = setpoint_x - measurement;
    
    // 比例项
    float proportional = Kp_x * error;
    
    // 积分项 (带抗饱和)
    integral_x += error;
    if (integral_x > output_max_x) integral_x = output_max_x;
    if (integral_x < output_min_x) integral_x = output_min_x;
    float integral = Ki_x * integral_x;
    
    // 微分项
    float derivative = Kd_x * (error - prev_error_x);
    prev_error_x = error;
    
    // 计算输出
    output_x = proportional + integral + derivative;
    
    // 输出限幅
    if (output_x > output_max_x) output_x = output_max_x;
    if (output_x < output_min_x) output_x = output_min_x;
    if(error<1)
		{
			Stepper_Motor_Stop_A();
		}
    return output_x;
}

float PID_Calculate_Y(float measurement) {
    // 计算误差
    float error = setpoint_y - measurement;
    
    // 比例项
    float proportional = Kp_y * error;
    
    // 积分项 (带抗饱和)
    integral_y += error;
    if (integral_y > output_max_y) integral_y = output_max_y;
    if (integral_y < output_min_y) integral_y = output_min_y;
    float integral = Ki_y * integral_y;
    
    // 微分项
    float derivative = Kd_y * (error - prev_error_y);
    prev_error_y = error;
    
    // 计算输出
    output_y = proportional + integral + derivative;
    
    // 输出限幅
    if (output_y > output_max_y) output_y = output_max_y;
    if (output_y < output_min_y) output_y = output_min_y;
    if(error<1)
		{
			Stepper_Motor_Stop_B();
		}
    return output_y;
}
void Move_Gimbal(float speed_x, float speed_y) {
    // 确定方向 (1=正向, 0=反向)
    uint8_t dir_x = (speed_x >= 0) ? 1 : 0;
    uint8_t dir_y = (speed_y >= 0) ? 1 : 0;
    
    // 计算速度 (RPM)
    rpm_x = (uint32_t)fabsf(speed_x);
    rpm_y = (uint32_t)fabsf(speed_y);
    
    // 限制最小速度
    if (rpm_x < 5) rpm_x = 0;
    if (rpm_y < 5) rpm_y = 0;
    
    // 控制电机
    if(fabs(setpoint_x - current_pos_x) > 1.0f) {  // 只有需要移动时才使能
        HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET); // 使能电机
        Motor_Move_A(dir_x, rpm_x);
    } else {
        HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_RESET); // 禁用电机
    }
    
    if(fabs(setpoint_y - current_pos_y) > 1.0f) {
        HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET); // 使能电机
        Motor_Move_B(dir_y, rpm_y);
    } else {
        HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_RESET); // 禁用电机
    }
//		OLED_Printf(0,0,OLED_8X16,"rpm_x:%d",rpm_x);
//		OLED_Printf(0,16,OLED_8X16,"rpm_y:%d",rpm_y);
		
}
void Gimbal_Update_Position(void) {
    // 计算PID输出
    float output_x = PID_Calculate_X(current_x);
    float output_y = PID_Calculate_Y(current_y);
    
    // 控制云台运动
    Move_Gimbal(output_x, output_y);
}
void Stepper_Motor_Update_Position(void) {
    // 在步进电机中断中更新当前位置
        // 根据方向更新位置
			if(steps_count_A>0)
			{
				steps_count_A--;
			}
			if(steps_count_B>0)
			{
				steps_count_B--;
			}
//			  float angle_step = 360.0f / steps_per_rev_x;
//        current_angle_x += (HAL_GPIO_ReadPin(GPIOB, DIR1_Pin) ? -angle_step : angle_step);
			if (HAL_GPIO_ReadPin(GPIOB, DIR1_Pin) == GPIO_PIN_SET) {
					current_pos_x -= 1.0f * rpm_x/60000*360.0;
			} else {
					current_pos_x += 1.0f * rpm_x/60000*360.0;
			}
    
//			  float angle_step = 360.0f / steps_per_rev_y;
//        current_angle_y += (HAL_GPIO_ReadPin(GPIOB, DIR2_Pin) ? -angle_step : angle_step);
        // 根据方向更新位置
			if (HAL_GPIO_ReadPin(GPIOB, DIR2_Pin) == GPIO_PIN_SET) {
					current_pos_y -= 1.0f * rpm_y/60000*360.0;
			} else {
					current_pos_y += 1.0f * rpm_y/60000*360.0;
			}
//			OLED_Printf(0,32,OLED_8X16,"current_x:%f",current_pos_x);
//			OLED_Printf(0,48,OLED_8X16,"current_y:%f",current_pos_y);

}