#include "Encoder.h"
#include "printf.h"

/* 全局变量定义 */
int32_t EncoderCountA = 0;
int32_t EncoderCountB = 0;
int32_t speedA = 0;
int32_t speedB = 0;

/**
 * @brief 编码器初始化
 */
void Encoder_Init(void)
{
    /* 使能编码器GPIO中断（A/B通道） */
    NVIC_EnableIRQ(GPIO_EncoderA_INT_IRQN);
    NVIC_EnableIRQ(GPIO_EncoderB_INT_IRQN);
}

void GROUP1_IRQHandler(void)
{
    // 获取中断状态
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA, GPIO_EncoderA_PIN_0_PIN | GPIO_EncoderA_PIN_1_PIN);
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB, GPIO_EncoderB_PIN_2_PIN | GPIO_EncoderB_PIN_3_PIN);

    // 编码器A处理
    if((gpioA & GPIO_EncoderA_PIN_0_PIN) == GPIO_EncoderA_PIN_0_PIN)
    {
        if(!DL_GPIO_readPins(GPIOA, GPIO_EncoderA_PIN_1_PIN)) EncoderCountA--;
        else EncoderCountA++;
    }
    else if((gpioA & GPIO_EncoderA_PIN_1_PIN) == GPIO_EncoderA_PIN_1_PIN)
    {
        if(!DL_GPIO_readPins(GPIOA, GPIO_EncoderA_PIN_0_PIN)) EncoderCountA++;
        else EncoderCountA--;
    }
    DL_GPIO_clearInterruptStatus(GPIOA, GPIO_EncoderA_PIN_0_PIN | GPIO_EncoderA_PIN_1_PIN);

    // 编码器B处理
    if((gpioB & GPIO_EncoderB_PIN_2_PIN) == GPIO_EncoderB_PIN_2_PIN)
    {
        if(!DL_GPIO_readPins(GPIOB, GPIO_EncoderB_PIN_3_PIN)) EncoderCountB--;
        else EncoderCountB++;
    }
    else if((gpioB & GPIO_EncoderB_PIN_3_PIN) == GPIO_EncoderB_PIN_3_PIN)
    {
        if(!DL_GPIO_readPins(GPIOB, GPIO_EncoderB_PIN_2_PIN)) EncoderCountB++;
        else EncoderCountB--;
    }

    // 清除中断标志
    DL_GPIO_clearInterruptStatus(GPIOB, GPIO_EncoderB_PIN_2_PIN | GPIO_EncoderB_PIN_3_PIN);
}


void ReadSpeed(void)
{
    speedA = EncoderCountA;
    speedB = EncoderCountB;
    printf("Encoder_A:%d:Encoder_B:%d\r\n", speedA, speedB);
    EncoderCountA = 0;
    EncoderCountB = 0;
}