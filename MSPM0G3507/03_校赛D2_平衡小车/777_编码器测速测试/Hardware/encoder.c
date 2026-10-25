#include "encoder.h"

int32_t gEncoderCount_L1, gEncoderCount_L2;
extern int stop_flag, stand_flag, correct_flag, mode, correct_yaw;

void IRQ_Init(void)
{

    NVIC_ClearPendingIRQ(BLE_INST_INT_IRQN);
    NVIC_EnableIRQ(BLE_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);

    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

}

void GROUP1_IRQHandler(void)            //编码器的中断函数
{   
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA,
        bottoms_bottom_12_PIN | bottoms_bottom_23_PIN | bottoms_bottom_14_PIN |bottoms_bottom_13_PIN |L1_Encoder_A_PIN_15_PIN);
     uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB ,L2_Encoder_A_PIN_8_PIN);
    if ((gpioA & bottoms_bottom_12_PIN) ==bottoms_bottom_12_PIN)
    {
            


         DL_GPIO_clearInterruptStatus(bottoms_PORT, bottoms_bottom_12_PIN);
    }
    else if ((gpioA & bottoms_bottom_23_PIN) ==
        bottoms_bottom_23_PIN) {
             


        DL_GPIO_clearInterruptStatus(bottoms_PORT, bottoms_bottom_23_PIN);
    }
     else if ((gpioA & bottoms_bottom_13_PIN) ==
         bottoms_bottom_13_PIN) {
            


        DL_GPIO_clearInterruptStatus(bottoms_PORT, bottoms_bottom_13_PIN);
    }
    else if ((gpioA & bottoms_bottom_14_PIN) ==
         bottoms_bottom_14_PIN) {
            

            
        DL_GPIO_clearInterruptStatus(bottoms_PORT, bottoms_bottom_14_PIN);
    }
    else if ((gpioA & L1_Encoder_A_PIN_15_PIN) != 0) //左后轮     A相上升沿
    {
        if(DL_GPIO_readPins(GPIOA, L1_Encoder_B_PIN_16_PIN))                 //B相上升沿
           gEncoderCount_L1--;
        else 
          gEncoderCount_L1++;                                                                                        //B相下降沿
        DL_GPIO_clearInterruptStatus(GPIOA ,  L1_Encoder_A_PIN_15_PIN);
    }



    if ((gpioB & L2_Encoder_A_PIN_8_PIN )!=0) //右后轮           A相上升沿
    {
        if(DL_GPIO_readPins(GPIOB, L2_Encoder_B_PIN_9_PIN     ))                  //B相上升沿
            gEncoderCount_L2++;
        else                                                                                                         //B相下降沿
            gEncoderCount_L2--;
        DL_GPIO_clearInterruptStatus(GPIOB , L2_Encoder_A_PIN_8_PIN);
    }

}
