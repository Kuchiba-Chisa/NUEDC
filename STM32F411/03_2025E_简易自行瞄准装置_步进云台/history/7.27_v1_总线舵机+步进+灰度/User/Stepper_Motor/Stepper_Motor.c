#include "Stepper_Motor.h"
#include "tim.h"
#include "gpio.h"
#include "math.h"
uint32_t steps_per_rev = 1800; // 每转步数,这个用来控制转速
volatile uint32_t steps_count_A = 0;
volatile uint32_t steps_count_B = 0;
volatile uint8_t motor_running = 0;
int16_t stepX_target=0, stepX_count = 0, stepX_pin_state = 0;
int16_t stepY_target=0, stepY_count = 0, stepY_pin_state = 0;
float current_angle_X = 0.0f;
float current_angle_Y = 0.0f;
/* 步进电机初始化 */
void Stepper_Motor_Init(void)
{
    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET); // 使能电机
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);           // 启动PWM
    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET); // 使能电机
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);           // 启动PWM
}

//void Stepper_X_Start(uint8_t dir, uint16_t steps)
//{
//    HAL_GPIO_WritePin(GPIOB, DIR1_Pin, (dir == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);

//	  // 设置方向

//    stepX_target = steps;   // 设置要走的步数
//    stepX_count = 0;
//    stepX_pin_state = 0;

//		HAL_TIM_Base_Start_IT(&htim2);
//}

//void Stepper_Y_Start(uint8_t dir, uint16_t steps)
//{
//    HAL_GPIO_WritePin(GPIOB, DIR1_Pin, (dir == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);

//		stepY_target = steps;   // 设置要走的步数
//    stepY_count = 0;
//    stepY_pin_state = 0;

//		HAL_TIM_Base_Start_IT(&htim4);
//}

//void stepX(void)
//{
//    if (stepX_pin_state == 0)
//    {
//        HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET);
//        stepX_pin_state = 1;
//    }
//    else
//    {
//        HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_RESET);
//        stepX_pin_state = 0;
//        
//        if (++stepX_count >= stepX_target)
//        {
//            HAL_TIM_Base_Stop_IT(&htim2);  // 停止定时器
//            stepX_count = 0;
//        }
//    }
//}

///* Y轴步进脉冲生成 - 在定时器中断中调用 */
//void stepY(void)
//{
//    if (stepY_pin_state == 0)
//    {
//        HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET);
//        stepY_pin_state = 1;
//    }
//    else
//    {
//        HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_RESET);
//        stepY_pin_state = 0;
//        
//        if (++stepY_count >= stepY_target)
//        {
//            HAL_TIM_Base_Stop_IT(&htim4);  // 停止定时器
//            stepY_count = 0;
//        }
//    }
//}

///* 角度转换函数 */
//int16_t angle_to_steps_X(float angle_deg)
//{
//    return (int16_t)(angle_deg * 400.0f / 360.0f);  // X轴 400步/圈
//}

//int16_t angle_to_steps_Y(float angle_deg)
//{
//    return (int16_t)(angle_deg * 2170.0f / 360.0f);  // Y轴 2170步/圈
//}

///* 角度控制函数 */
//void Stepper_X_Goto_Angle(float target_angle_deg)
//{
//    float delta_angle = target_angle_deg - current_angle_X;
//    uint8_t dir = (delta_angle >= 0) ? 1 : 0;
//    int16_t steps = angle_to_steps_X(fabsf(delta_angle));

//    current_angle_X = target_angle_deg;  // 更新当前角度
//    Stepper_X_Start(dir, steps);
//}

//void Stepper_Y_Goto_Angle(float target_angle_deg)
//{
//    float delta_angle = target_angle_deg - current_angle_Y;
//    uint8_t dir = (delta_angle >= 0) ? 1 : 0;
//    int16_t steps = angle_to_steps_Y(fabsf(delta_angle));

//    current_angle_Y = target_angle_deg;  // 更新当前角度
//    Stepper_Y_Start(dir, steps);
//}

/* 控制步进电机运动 */
void Motor_Move_A(uint8_t direction, uint32_t speed_rpm)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR1_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET); // 使能电机
    
    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, period / 2); // 50%占空比
}
void Motor_Move_B(uint8_t direction, uint32_t speed_rpm)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR2_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET); // 使能电机
   
    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2); // 50%占空比
}

void Stepper_Motor_Move_A(uint8_t direction, uint32_t speed_rpm, uint32_t steps)//5ms一步
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR1_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET); // 使能电机

    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, period / 2); // 50%占空比
	  steps_count_A = steps;
    motor_running = 1;
}

void Stepper_Motor_Move_B(uint8_t direction, uint32_t speed_rpm, uint32_t steps)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR2_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET); // 使能电机

    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2); // 50%占空比
	  steps_count_B = steps;
    motor_running = 1;
}

/* 停止步进电机 */
void Stepper_Motor_Stop_A(void)
{
    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_RESET); // 禁用电机
}
void Stepper_Motor_Stop_B(void)
{
    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_RESET); // 禁用电机
}