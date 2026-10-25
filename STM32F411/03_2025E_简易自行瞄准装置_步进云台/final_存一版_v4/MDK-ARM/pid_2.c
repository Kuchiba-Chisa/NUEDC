#include "main.h"
#include "pid_2.h"
#include "Bujin.h"
#include "math.h"
#include "OLED.h"

int flag=0;

//void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float output_limit) {
//    pid->Kp = Kp;
//    pid->Ki = Ki;
//    pid->Kd = Kd;
//    pid->integral = 0.0f;
//    pid->prev_error = 0.0f;
//    pid->output_limit = output_limit;
//}

//float PID_Update(PIDController* pid, float setpoint, float measurement, float dt) {
//    // 计算误差
//    float error = setpoint - measurement;
//		
//    
//    // 比例项
//    float proportional = pid->Kp * error;

//    // 积分项 (带抗饱和)
//    pid->integral += error * dt;
//    // 积分限幅
//    if (pid->integral > pid->output_limit) pid->integral = pid->output_limit;
//    if (pid->integral < -pid->output_limit) pid->integral = -pid->output_limit;
//    float integral = pid->Ki * pid->integral;
//    
//    // 微分项
//    float derivative = pid->Kd * (error - pid->prev_error) / dt;
//    pid->prev_error = error;
//    
//    // 计算总输出
//    float output = proportional + integral + derivative;
////    OLED_Printf(0,32,OLED_8X16,"%.2f,%.2f,%.2f",proportional,integral,derivative);
////		OLED_Printf(0,48,OLED_8X16,"%.2f",error);
//    if(fabs(error)<=5)output = 0;
//		

//    return output;
//}