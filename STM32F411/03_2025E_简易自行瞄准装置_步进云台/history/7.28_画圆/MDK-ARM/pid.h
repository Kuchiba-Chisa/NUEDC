#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float Kp;           // 比例增益
    float Ki;           // 积分增益
    float Kd;           // 微分增益
    float integral;     // 积分项
    float prev_error;   // 上一次误差
    float output;       // PID输出
    float output_max;   // 输出上限
    float output_min;   // 输出下限
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float output_max, float output_min);
float PID_Update(PID_Controller *pid, float error, float dt);

#ifdef __cplusplus
}
#endif

#endif /* __PID_H */