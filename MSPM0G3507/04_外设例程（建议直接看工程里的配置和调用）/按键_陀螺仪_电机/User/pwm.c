#include "pwm.h"
#include "ti_msp_dl_config.h"

void Set_PWM(int motor_left, int motor_right) {
    // 左电机控制
    if (motor_left > 0) {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_0_C0_IDX);          // 高电平
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,10000-motor_left,GPIO_PWM_1_C0_IDX);  // PWM 占空比（正转）
    } else {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,10000+motor_left,GPIO_PWM_0_C0_IDX);  // PWM 占空比（反转）
        DL_TimerG_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_1_C0_IDX);          // 高电平
    }

    // 右电机控制
    if (motor_right > 0) {
        DL_TimerG_setCaptureCompareValue(PWM_1_INST,10000-motor_right,GPIO_PWM_0_C0_IDX);  // PWM 占空比（正转）
        DL_TimerG_setCaptureCompareValue(PWM_1_INST,10000,GPIO_PWM_1_C0_IDX);          // 高电平
    } else {
        DL_TimerG_setCaptureCompareValue(PWM_1_INST,10000,GPIO_PWM_0_C0_IDX);          // 高电平
        DL_TimerG_setCaptureCompareValue(PWM_1_INST,10000+motor_right,GPIO_PWM_1_C0_IDX);  // PWM 占空比（反转）
    }

}