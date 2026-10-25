#include "Stepper_Motor.h"
#include "tim.h"
#include "gpio.h"

uint32_t steps_per_rev = 1600; // 每转步数,这个用来控制转速

/* 步进电机初始化 */
void Stepper_Motor_Init(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // 使能电机
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);           // 启动PWM
}

/* 控制步进电机运动 */
void Stepper_Motor_Move(uint8_t direction, uint32_t speed_rpm)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    
    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&htim2, period);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2); // 50%占空比
}

/* 停止步进电机 */
void Stepper_Motor_Stop(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // 禁用电机
}