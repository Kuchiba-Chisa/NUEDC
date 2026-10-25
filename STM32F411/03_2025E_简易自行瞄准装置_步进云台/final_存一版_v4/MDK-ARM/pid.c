#include "pid.h"
#include "math.h"
#include "oled.h"
#include "Stepper_Motor.h"

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float output_max, float output_min) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
    pid->output_max = output_max;
    pid->output_min = output_min;
}

float PID_Update(PID_Controller *pid, float error, float dt) {
    // 比例项
    float proportional = pid->Kp * error;
    
    // 积分项 (带抗饱和)
    pid->integral += error * dt;
    float integral = pid->Ki * pid->integral;
    
    // 微分项
    float derivative = pid->Kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;
    
    // 计算输出
    pid->output = proportional + integral + derivative;
		
//		OLED_Printf(0,0,OLED_8X16,"%.2f",proportional);
//		OLED_Printf(0,16,OLED_8X16,"%.2f",integral);
//		OLED_Printf(0,32,OLED_8X16,"%.2f",derivative);
//		OLED_Printf(0,48,OLED_8X16,"%.2f",pid->output);
	
//    // 输出限幅
//    if (pid->output > pid->output_max) {
//        pid->output = pid->output_max;
//        pid->integral -= error * dt; // 抗饱和
//    } else if (pid->output < pid->output_min) {
//        pid->output = pid->output_min;
//        pid->integral -= error * dt; // 抗饱和
//    }
    
    return pid->output;
}