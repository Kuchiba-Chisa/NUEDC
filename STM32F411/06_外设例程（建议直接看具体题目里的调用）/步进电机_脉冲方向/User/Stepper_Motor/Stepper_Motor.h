#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


/* 步进电机初始化 */
void Stepper_Motor_Init(void);

/* 控制步进电机运动 */
void Stepper_Motor_Move(uint8_t direction, uint32_t speed_rpm);

/* 停止步进电机 */
void Stepper_Motor_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEPPER_MOTOR_H */