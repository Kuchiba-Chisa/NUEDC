#include "key.h"
#include "ti_msp_dl_config.h"

int key_flag=0;

void GROUP1_IRQHandler(void) // Group1的中断服务函数
{
    // 读取Group1的中断寄存器并清除中断标志位
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        // 检查是否是KEY的GPIOA端口中断，注意是INT_IIDX，不是PIN_18_IIDX
        case KEY_INT_IIDX:
            // 检查按键1（KEY_1_Pin）
            if (DL_GPIO_readPins(KEY_PORT, KEY1_PIN) > 0)
            {
                // 设置按键1的标志位
                key_flag = 1;
                // 如果按键按下变为高电平
            }
            // 检查按键2（KEY_2_Pin）
            else if (DL_GPIO_readPins(KEY_PORT, KEY2_PIN) > 0)
            {
                // 设置按键2的标志位
                key_flag = 2;
                // 如果按键按下变为高电平
            }
            // 检查按键3（KEY_3_Pin）
            else if (DL_GPIO_readPins(KEY_PORT, KEY3_PIN) > 0)
            {
                // 设置按键3的标志位
                key_flag = 2;
                // 如果按键按下变为高电平
            }
            // 检查按键4（KEY_4_Pin）
            else if (DL_GPIO_readPins(KEY_PORT, KEY4_PIN) > 0)
            {
                // 设置按键4的标志位
                key_flag = 4;
                // 如果按键按下变为高电平
            }
        break;

        // 其他中断处理情况
        // 可以添加其他的中断 case 以处理不同的中断源
    }
}

