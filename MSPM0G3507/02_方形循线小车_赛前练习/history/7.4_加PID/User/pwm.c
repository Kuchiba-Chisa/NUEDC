#include "pwm.h"
#include "ti_msp_dl_config.h"

void Set_PWM(int motor_left, int motor_right) {
    // 左电机控制
    if (motor_left > 0) {
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000-motor_left,GPIO_PWM_0_C2_IDX);          // 高电平
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_0_C3_IDX);  // PWM 占空比（正转）
    } else {
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_0_C2_IDX);  // PWM 占空比（反转）
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000+motor_left,GPIO_PWM_0_C3_IDX);          // 高电平
    }

    // 右电机控制
    if (motor_right > 0) {
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_0_C0_IDX);  // PWM 占空比（正转）
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000-motor_right,GPIO_PWM_0_C1_IDX);          // 高电平
    } else {
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000+motor_right,GPIO_PWM_0_C0_IDX);          // 高电平
        DL_TimerA_setCaptureCompareValue(PWM_0_INST,10000,GPIO_PWM_0_C1_IDX);  // PWM 占空比（反转）
    }

}