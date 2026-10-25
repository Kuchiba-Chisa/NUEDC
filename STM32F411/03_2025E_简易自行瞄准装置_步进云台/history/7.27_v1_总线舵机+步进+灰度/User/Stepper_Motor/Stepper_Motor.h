#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

extern volatile uint32_t steps_count_A; // 电机A剩余步数
extern volatile uint32_t steps_count_B; // 电机B剩余步数
extern volatile uint8_t motor_running;  // 电机运行标志
/* 步进电机初始化 */
void Stepper_Motor_Init(void);

/* 控制步进电机运动 */
void Stepper_Motor_Move_A(uint8_t direction, uint32_t speed_rpm, uint32_t steps);
void Stepper_Motor_Move_B(uint8_t direction, uint32_t speed_rpm, uint32_t steps);
void Stepper_Motor_Set_StepsPerRev(uint32_t steps);
void Stepper_Motor_Move_Angle_A(uint8_t direction, uint32_t speed_rpm, float angle);
void Stepper_Motor_Move_Angle_B(uint8_t direction, uint32_t speed_rpm, float angle);
void Stepper_A_Start(uint8_t dir, uint16_t steps);
void Stepper_B_Start(uint8_t dir, uint16_t steps);
void stepX(void);
void stepY(void);
void Motor_Move_A(uint8_t direction, uint32_t speed_rpm);
void Motor_Move_B(uint8_t direction, uint32_t speed_rpm);

int16_t angle_to_steps_X(float angle_deg);
int16_t angle_to_steps_Y(float angle_deg);
void Stepper_X_Goto_Angle(float target_angle_deg);
void Stepper_Y_Goto_Angle(float target_angle_deg);


/* 停止步进电机 */
void Stepper_Motor_Stop_A(void);
void Stepper_Motor_Stop_B(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_MOTOR_H */