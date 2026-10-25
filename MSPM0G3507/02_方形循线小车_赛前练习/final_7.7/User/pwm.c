#include "pwm.h"
#include "ti_msp_dl_config.h"
volatile float Velocity_Left,Velocity_Right;
float Rotation_Speed_L,Rotation_Speed_R;	
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
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right)
{ 							//电机转速  转速=编码器读数（5ms每次）*读取频率/倍频数/减速比/编码器精度
	Rotation_Speed_L = encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = 0.3*Rotation_Speed_L*PI*Diameter_67 + 0.7*Velocity_Left;		//求出编码器速度=转速*周长
	Rotation_Speed_R = encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = 0.3*Rotation_Speed_R*PI*Diameter_67 + 0.7*Velocity_Right;		//求出编码器速度=转速*周长

}