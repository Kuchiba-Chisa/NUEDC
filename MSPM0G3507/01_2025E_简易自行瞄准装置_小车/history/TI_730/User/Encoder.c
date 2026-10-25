#include "Encoder.h"
#include "oled.h"
/* 全局变量定义 */
float speedA = 0;
float speedB = 0;
int32_t gpioA;
int32_t gpioB;
int32_t gEncoderCount1 = 0;
int32_t gEncoderCount2 = 0;
int32_t test=0;
extern int key_flag;
int num;
/**
 * @brief 编码器初始化
 */
void Encoder_Init(void)
{
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(GPIO_EncoderB_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

void GROUP1_IRQHandler(void)
{
    //获取中断信号
    gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA,GPIO_EncoderA_PIN_15_PIN|KEY_KEY_1_Pin_PIN|KEY_KEY_2_Pin_PIN|KEY_KEY_3_Pin_PIN|KEY_KEY_4_Pin_PIN);
    gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB,GPIO_EncoderB_PIN_8_PIN);
 
    if((gpioA & GPIO_EncoderA_PIN_15_PIN) != 0)
    {
        test = 1;
        if(!DL_GPIO_readPins(GPIOA,GPIO_EncoderA_PIN_16_PIN))//P1为低电平
        {
            gEncoderCount1--;
            test = 2;
        }
        else
        {
            gEncoderCount1++;
            test = 3;
        }
    }
    DL_GPIO_clearInterruptStatus(GPIOA, GPIO_EncoderA_PIN_15_PIN);

    if((gpioB & GPIO_EncoderB_PIN_8_PIN) != 0)
    {
        test = 4;
        if(!DL_GPIO_readPins(GPIOB,GPIO_EncoderB_PIN_9_PIN))//P1为低电平
        {
            gEncoderCount2--;
            test = 5;
        }
        else
        {
            gEncoderCount2++;
            test = 6;
        }
    }
    DL_GPIO_clearInterruptStatus(GPIOB, GPIO_EncoderB_PIN_8_PIN);

    if((gpioA & KEY_KEY_1_Pin_PIN) != 0)
    {
        key_flag=1;
    }
    DL_GPIO_clearInterruptStatus(GPIOA, KEY_KEY_1_Pin_PIN);
    if((gpioA & KEY_KEY_2_Pin_PIN) != 0)
    {
        key_flag=2;
    }
    DL_GPIO_clearInterruptStatus(GPIOA, KEY_KEY_2_Pin_PIN);
    if((gpioA & KEY_KEY_3_Pin_PIN) != 0)
    {
        key_flag=3;
    }
    DL_GPIO_clearInterruptStatus(GPIOA, KEY_KEY_3_Pin_PIN);
    if((gpioA & KEY_KEY_4_Pin_PIN) != 0)
    {
        num++;
    }
    DL_GPIO_clearInterruptStatus(GPIOA, KEY_KEY_4_Pin_PIN);

}


void ReadSpeed(void)
{
    speedA = gEncoderCount1;
    speedB = gEncoderCount2;
}