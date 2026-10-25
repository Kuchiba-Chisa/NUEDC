#include "Encoder.h"
#include "oled.h"
/* 全局变量定义 */
int32_t EncoderCountA = 0;
int32_t EncoderCountB = 0;
int32_t speedA = 0;
int32_t speedB = 0;
int key_flag=0;
/**
 * @brief 编码器初始化
 */
void Encoder_Init(void)
{
    /* 使能编码器GPIO中断（A/B通道） */
    NVIC_ClearPendingIRQ(KEY_INT_IRQN);

    NVIC_EnableIRQ(KEY_INT_IRQN);
}

void GROUP1_IRQHandler(void)
{
    // 获取中断状态

    // 清除中断标志
    //按键处理

        //检查是否是KEY的GPIOA端口中断，注意是INT_IIDX，不是PIN_18_IIDX
            //如果按键按下变为高电平
            if( DL_GPIO_readPins(KEY_PORT, KEY_KEY_2_Pin_PIN) == 0 )
            {
                //设置LED引脚状态翻转
                OLED_ShowNum(0,16,3,1,8,1);//6*8 “ABC”
            }
        


}


void ReadSpeed(void)
{
    speedA = EncoderCountA;
    speedB = EncoderCountB;
    // printf("Encoder_A:%d:Encoder_B:%d\r\n", speedA, speedB);
    EncoderCountA = 0;
    EncoderCountB = 0;
}