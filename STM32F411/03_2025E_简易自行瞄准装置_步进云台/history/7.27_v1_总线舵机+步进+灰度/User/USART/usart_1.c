#include "usart_1.h"
#include "string.h"
#include "OLED.h"
#include "Bujin.h"
#define RXBUFFERSIZE  256     //最大接收字节数

//定义串口句柄,使用串口1
extern UART_HandleTypeDef huart1;
#define UART_HANDLE huart1

//定义数据缓冲区
uint8_t RxBuffer_1[RXBUFFERSIZE];
uint8_t Uart_RxBuffer_1;      //接收中断缓冲
uint8_t Uart_Rx_Cnt_1 = 0;     //接收缓冲计数

uint16_t data[8]={0};
int current_x,current_y;
uint16_t angle_x,angle_y;
void Debug_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  //使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE,UART_IT_RXNE);
  //开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&UART_HANDLE,&Uart_RxBuffer_1,1);
}

//串口1接收完成回调函数
void UART1_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart_Rx_Cnt_1 >= 255)  //溢出判断
	{
		Uart_Rx_Cnt_1 = 0;
		memset(RxBuffer_1,0x00,sizeof(RxBuffer_1));
		HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"Yi chu", 10,0xFFFF); 	
	}
	else
	{
		RxBuffer_1[Uart_Rx_Cnt_1++] = Uart_RxBuffer_1;
        //单字符判断
//		OLED_Printf(0,48,OLED_8X16,"r:%d",Uart_RxBuffer_1);
//		OLED_Update();

		if((RxBuffer_1[Uart_Rx_Cnt_1-1] == 0x0A)&&(RxBuffer_1[Uart_Rx_Cnt_1-2] == 0x0D)) //判断结束位
		{
      //这里可以写多字节消息的判断
      //单字节消息0

      // 解析k210数据
      sscanf((const char *)RxBuffer_1, "%d,%d\r\n", &data[0], &data[1]);
      current_x =640-data[0];
      current_y =data[1];
			ConvertCoordinatesToAngles(0.32,current_x,current_y,&angle_x,&angle_y);
			OLED_Printf(0,0,OLED_8X16,"x:%d",current_x);
			OLED_Printf(0,16,OLED_8X16,"y:%d",current_y);

      //复位
			Uart_Rx_Cnt_1 = 0;
			memset(RxBuffer_1,0x00,sizeof(RxBuffer_1)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer_1, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
}

//串口1错误回调函数(主要用来清除溢出中断)
void UART1_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(HAL_UART_ERROR_ORE)
  {
     uint32_t temp = huart->Instance->SR;
     temp = huart->Instance->DR;
  }
}

/*****************  发送字符串 **********************/
void Usart1_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UART_HANDLE,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
   
}

void Usart1_SendNumber(int num)
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
void Usart1_SendChar(char c)
{
    HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&c, 1, 1000);  // 发送单个字符
}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
//int fgetc(FILE *f)
//{
//  uint8_t ch = 0;
//  HAL_UART_Receive(&UART_HANDLE, &ch, 1, 0xffff);
//  return ch;
//}
