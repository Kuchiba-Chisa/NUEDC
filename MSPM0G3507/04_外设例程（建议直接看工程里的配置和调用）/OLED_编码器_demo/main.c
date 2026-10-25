#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "printf.h"
#include "Encoder.h"
#include "oled.h"

extern int32_t speedA;
extern int32_t speedB;

int main(void)
{
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    printf("Printf start work\r\n");
    Encoder_Init();
    printf("Encoder start work\r\n");
    OLED_Init();
    OLED_Clear(); 
    while (1)
    {
        ReadSpeed();
        delay_cycles(32000000/2);
        // OLED_ShowString(0, 0, (u8 *)"Hello World!", 16, 1); 
        // OLED_Refresh();
    }
}


