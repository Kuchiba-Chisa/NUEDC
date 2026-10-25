#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#define M_PI = 3.141592653
extern volatile uint32_t steps_count_A; // 电机A剩余步数
extern volatile uint32_t steps_count_B; // 电机B剩余步数
extern volatile uint8_t motor_running;  // 电机运行标志
/* 步进电机初始化 */
void Stepper_Motor_Init(void);

/* 控制步进电机运动 */
void Stepper_Motor_Set_StepsPerRev(uint32_t steps);
void Motor_Move_A(uint8_t direction, uint32_t speed_rpm);
void Motor_Move_B(uint8_t direction, uint32_t speed_rpm);



/* 停止步进电机 */
void Stepper_Motor_Stop_A(void);
void Stepper_Motor_Stop_B(void);

// PID控制函数
void Stepper_Control_Init(void);
void Stepper_Control(int target_x, int target_y);
/* 高精度矩形绘制函数 */
void DrawPrecisionRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
/* 闭环控制直到到达目标位置 */
void Stepper_Control_UntilReached(int target_x_val, int target_y_val);
/* 圆形绘制函数 */
void DrawCircle(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t segments);


#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_MOTOR_H */