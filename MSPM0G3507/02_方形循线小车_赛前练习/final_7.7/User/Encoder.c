#include "Encoder.h"
#include "oled.h"
/* 全局变量定义 */
int32_t EncoderCountA = 0;
int32_t EncoderCountB = 0;
int32_t speedA = 0;
int32_t speedB = 0;
int key_flag=0;
int num=0;
int32_t gEncoderCount_L1, gEncoderCount_L2;

/**
 * @brief 编码器初始化
 */
void Encoder_Init(void)
{
    /* 使能编码器GPIO中断（A/B通道） */
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_ClearPendingIRQ(L2_Encoder_A_INT_IRQN);
    NVIC_EnableIRQ(L2_Encoder_A_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

void GROUP1_IRQHandler(void)
{
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA,L1_Encoder_A_PIN_15_PIN | KEY_KEY_1_Pin_PIN | KEY_KEY_2_Pin_PIN | KEY_KEY_3_Pin_PIN | KEY_KEY_4_Pin_PIN);
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB,L2_Encoder_A_PIN_8_PIN);
    OLED_ShowNum(0, 24,  1, 1,  8,  1);

    if ((gpioA & L1_Encoder_A_PIN_15_PIN) == L1_Encoder_A_PIN_15_PIN ) //左后轮     A相上升沿
    {
        if(DL_GPIO_readPins(GPIOA, L1_Encoder_B_PIN_16_PIN))                 //B相上升沿
        {
            gEncoderCount_L1--;
        }
        else 
        {
            gEncoderCount_L1++;  
        }                                                                                      //B相下降沿
        DL_GPIO_clearInterruptStatus(GPIOA ,  L1_Encoder_A_PIN_15_PIN);
    }

    if ((gpioB & L2_Encoder_A_PIN_8_PIN ) == L2_Encoder_A_PIN_8_PIN) //右后轮           A相上升沿
    {
        if(DL_GPIO_readPins(GPIOB, L2_Encoder_B_PIN_9_PIN))                  //B相上升沿
        {
            gEncoderCount_L2++;
        }
        else                                                                                                         //B相下降沿
        {
            gEncoderCount_L2--;
        }
        DL_GPIO_clearInterruptStatus(GPIOB , L2_Encoder_A_PIN_8_PIN);
    }

    if ((gpioA & KEY_KEY_1_Pin_PIN) == KEY_KEY_1_Pin_PIN )
    {
        num++;
        DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_KEY_1_Pin_PIN);

    } 
    else if ((gpioA & KEY_KEY_2_Pin_PIN) == KEY_KEY_2_Pin_PIN )
    {  
        key_flag=2;
        DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_KEY_2_Pin_PIN);
    }
    else if ((gpioA & KEY_KEY_3_Pin_PIN) == KEY_KEY_3_Pin_PIN )
    {
        key_flag=3;
        DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_KEY_3_Pin_PIN);
    }
    else if ((gpioA & KEY_KEY_4_Pin_PIN) == KEY_KEY_4_Pin_PIN )
    {
        key_flag=4;
        DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_KEY_4_Pin_PIN);
    }

}


void ReadSpeed(void)
{
    speedA = gEncoderCount_L1;
    speedB = gEncoderCount_L2;
    // printf("Encoder_A:%d:Encoder_B:%d\r\n", speedA, speedB);
}