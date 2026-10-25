#include "pwm.h"
#include "OLED.h"


/*************************************************************
函数功能：PWM赋值
入口参数：motor_left:左电机PWM值，motor_right:右电机PWM值
返回  值：无
注意： 这里411设的ARR值为9999，意味着PWM取值0~10000
*************************************************************/
void Set_PWM(int motor_left, int motor_right) {
    // 左电机控制
    if (motor_left > 0) {
        PWMA_IN1 = 10000;          // 高电平
        PWMA_IN2 = 10000 - motor_left;  // PWM 占空比（正转）
    } else {
        PWMA_IN1 = 10000 + motor_left;  // PWM 占空比（反转）
        PWMA_IN2 = 10000;          // 高电平
    }

    // 右电机控制
    if (motor_right > 0) {
        PWMB_IN1 = 10000 - motor_right;  // PWM 占空比（正转）
        PWMB_IN2 = 10000;          // 高电平
    } else {
        PWMB_IN1 = 10000;          // 高电平
        PWMB_IN2 = 10000 + motor_right;  // PWM 占空比（反转）
    }

    // OLED 显示
    OLED_Printf(0, 32, OLED_8X16, "L:%d", motor_left);
    OLED_Printf(0, 48, OLED_8X16, "R:%d", motor_right);
}



