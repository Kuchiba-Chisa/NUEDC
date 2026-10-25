#include <stdio.h>
#include "main.h"

#ifndef __DEBUG_H
#define __DEBUG_H


#define RXBUFFERSIZE  256     //最大接收字节数
//串口1接收中断初始化
void Debug_Init(void);

//串口1接收完成回调函数
void UART1_RxCpltCallback(UART_HandleTypeDef *huart);

void UART1_ErrorCallback(UART_HandleTypeDef *huart);
void Usart1_SendString(uint8_t *str);
void Usart1_SendNumber(int num);
void Usart1_SendChar(char c);
#endif // !__DEBUG_H






