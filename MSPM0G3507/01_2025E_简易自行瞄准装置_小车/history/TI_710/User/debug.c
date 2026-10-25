#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "debug.h"
volatile unsigned char uart_data = 0;

void Debug_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符

}

void Debug_INST_IRQHandler(void)
{
    //如果产生了串口中断

}
//重定向fputc函数
// int fputc(int ch, FILE *stream)
// {

// }
//重定向fputs函数
// int fputs(const char* restrict s, FILE* restrict stream) {

//     uint16_t char_len=0;
//     while(*s!=0)
//     {
//         while( DL_UART_isBusy(Debug_INST) == false );
//         DL_UART_Main_transmitData(Debug_INST, *s++);
//         char_len++;
//     }
//     return char_len;
// }
// int puts(const char* _ptr)
// {
//  return 0;
// }