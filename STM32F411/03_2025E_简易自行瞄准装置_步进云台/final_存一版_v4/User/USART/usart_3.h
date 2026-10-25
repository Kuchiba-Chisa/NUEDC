#include <stdio.h>
#include "main.h"
#include "usart.h"


#ifndef __USART3__H
#define __USART3__H
#define RXBUFFERSIZE_3  256     //最大接收字节数


//串口1接收中断初始化
void  USART3_Init(void);
//串口1接收完成回调函数
void UART3_RxCpltCallback(UART_HandleTypeDef *huart);
void UART3_ErrorCallback(UART_HandleTypeDef *huart);
void Usart3_SendString(uint8_t *str);
void Usart3_SendNumber(int num);
void Usart3_SendChar(char c);
#endif // !__USART3_3_H
