#ifndef __PID_2_H
#define __PID_2_H

typedef struct {
    float Kp;           // 比例增益
    float Ki;           // 积分增益
    float Kd;           // 微分增益
    float integral;     // 积分项累计值
    float prev_error;   // 上一次误差值
    float output_limit; // 输出限幅
} PIDController;


//void PID_Init(PIDController* pid, float Kp, float Ki, float Kd, float output_limit);
//float PID_Update(PIDController* pid, float setpoint, float measurement, float dt);


#endif // !__PID_2_H
