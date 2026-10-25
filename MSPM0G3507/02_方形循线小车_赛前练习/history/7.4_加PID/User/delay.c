#include "ti_msp_dl_config.h"
#include "stdio.h"
void delay_us(unsigned long __us) 
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    // ������Ҫ��ʱ���� = �ӳ�΢���� * ÿ΢����ʱ����
    ticks = __us * (32000000 / 1000000);
}
void delay_ms(unsigned long ms) 
{
	delay_us( ms * 1000 );
}
void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }