/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-13 17-52-31
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-04 06-57-42
 * @FilePath: \Project\RedServo\STM32\User\USART\usart_2.c
 * @Description: 串口2的驱动代码(用于调试PID)
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */
#include "main.h"
#include "usart_3.h"
#include "usart_1.h"
#include "string.h"
#include <stdlib.h>
#include <string.h>
#include "oled.h"
//定义串口句柄,使用串口3
extern UART_HandleTypeDef huart6;

#define UART_HANDLE huart6

//定义数据缓冲区
uint8_t RxBuffer_3[RXBUFFERSIZE_3];
uint8_t Uart_RxBuffer_3;      //接收中断缓冲
uint8_t Uart_Rx_Cnt_3 = 0;     //接收缓冲计数
extern int target_X,target_Y;
extern uint16_t data[8];
//extern int8_t Position_error[2]={0};

//--------------------------K210参考系---------------------
//消息接收标志位
//extern uint8_t K210_Flag;
//接收缓冲区
//extern uint16_t K210_data[8]={0};
void USART3_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  //使能 USART6 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE,UART_IT_RXNE);
  //开启 USART6 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&UART_HANDLE,&Uart_RxBuffer_3,1);
}

//串口2接收完成回调函数
void UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
    
    /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
//	OLED_Printf(0, 0, OLED_8X16, "recieve:%d,%d,%d", RxBuffer_2[0],RxBuffer_2[1],RxBuffer_2[2]);
//	OLED_Printf(0, 16, OLED_8X16, "recieve:%d", Uart_RxBuffer_2);

	if(Uart_Rx_Cnt_3 >= 255)  //溢出判断
	{
		Uart_Rx_Cnt_3 = 0;
		memset(RxBuffer_3,0x00,sizeof(RxBuffer_3));
//		HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"数据溢出", 10,0xFFFF); 
	}
	else
	{
//		HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"1", 10,0xFFFF); 	

		RxBuffer_3[Uart_Rx_Cnt_3++] = Uart_RxBuffer_3;
	
		
		if((RxBuffer_3[Uart_Rx_Cnt_3-1] == 0x0A)&&(RxBuffer_3[Uart_Rx_Cnt_3-2] == 0x0D)) //判断结束位
		{
       
        // 这里可以写多字节消息的判断
      
      //printf("%d,%d",Position_error[0],Position_error[1]);
      sscanf((const char *)RxBuffer_3, "%d,%d\r\n", &data[0], &data[1]);
      target_X =data[0];
      target_Y =data[1];
			// 新增：发送坐标到串口屏
			char screen_buf[64];
			sprintf(screen_buf, "feedback.t6.txt=\"%d\"", target_X); // X坐标发送到t6文本
			Usart1_SendString((uint8_t *)screen_buf);
			Usart1_SendChar(0xff); // 结束符
			Usart1_SendChar(0xff); // 结束符
			Usart1_SendChar(0xff); // 结束符

			sprintf(screen_buf, "feedback.t7.txt=\"%d\"", target_Y); // Y坐标发送到t7文本
			Usart1_SendString((uint8_t *)screen_buf);
			Usart1_SendChar(0xff); // 结束符
			Usart1_SendChar(0xff); // 结束符
			Usart1_SendChar(0xff); // 结束符

//			OLED_Printf(0,32,OLED_8X16,"x:%d",target_X);
//			OLED_Printf(0,48,OLED_8X16,"y:%d",target_Y);
      //复位
			Uart_Rx_Cnt_3 = 0;
			memset(RxBuffer_3,0x00,sizeof(RxBuffer_3)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer_3, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用


    
}

//串口1错误回调函数(主要用来清除溢出中断)
void UART3_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(HAL_UART_ERROR_ORE)
  {
     uint32_t temp = huart->Instance->SR;
     temp = huart->Instance->DR;
  }
}

/*****************  发送字符串 **********************/
void Usart3_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UART_HANDLE,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
   
}

void Usart3_SendNumber(int num)
{
    char str[12];  // 用于存储整数的字符串表示，12个字符足够表示一个32位整数
    int len = sprintf(str, "%d", num);  // 将整数转换为字符串
    unsigned int k = 0;
    do 
    {
        HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)(str + k), 1, 1000);  // 逐个字符发送
        k++;
    } while(k < len);  // 直到发送完所有字符
}
void Usart3_SendChar(char c)
{
    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&c, 1, 1000);  // 发送单个字符
}
