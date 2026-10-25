#include "control.h"
#include "ti/driverlib/dl_gpio.h"
#include "math.h"
#include "stdlib.h"

int encoder_cnt, ctrl_cnt_flag;
extern int32_t gEncoderCount_L1, gEncoderCount_L2;
int Encoder_left,Encoder_right;
extern float Velocity_Left,Velocity_Right;

void TIMER_0_INST_IRQHandler(void)
{
    encoder_cnt++;
    ctrl_cnt_flag++;
    if(encoder_cnt == 5)
    {
        Encoder_left = gEncoderCount_L1;
        Encoder_right = gEncoderCount_L2;
        gEncoderCount_L1 = 0;
        gEncoderCount_L2 = 0;
        Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
    
        encoder_cnt = 0;
    }
    
}

