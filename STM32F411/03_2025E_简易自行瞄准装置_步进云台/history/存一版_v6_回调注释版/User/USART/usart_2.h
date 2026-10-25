#include <stdio.h>
#include "main.h"
#include "usart.h"


#ifndef __USART2_2_H
#define __USART2_2_H
#define RXBUFFERSIZE_2  256     //最大接收字节数


//串口1接收中断初始化
void  USART2_Init(void);
//串口1接收完成回调函数
void UART2_RxCpltCallback(UART_HandleTypeDef *huart);
void UART2_ErrorCallback(UART_HandleTypeDef *huart);
void Usart2_SendString(uint8_t *str);
void Usart2_SendNumber(int num);
void Usart2_SendChar(char c);
void UART2_SendHex(uint8_t hex);

#endif // !__USART2_2_H






