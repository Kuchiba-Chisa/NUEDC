#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "debug.h"
volatile unsigned char uart_data = 0;

void Debug_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(Debug_INST) == false );
    //发送单个字符
    DL_UART_Main_transmitData(Debug_INST, ch);
}

void Debug_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(Debug_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data = DL_UART_Main_receiveData(Debug_INST);

            Debug_send_char(uart_data);

            break;

        default://其他的串口中断
            break;
    }
}
//重定向fputc函数
int fputc(int ch, FILE *stream)
{
    while( DL_UART_isBusy(Debug_INST) == false );
    DL_UART_Main_transmitData(Debug_INST, ch);
    return ch;
}

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