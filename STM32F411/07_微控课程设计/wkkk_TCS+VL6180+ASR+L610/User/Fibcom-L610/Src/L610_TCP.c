/*******************************************************************************
**                       @ Copyright 2014 - 2020                              **
**                           www.diysoon.com                                  **
**                                                                            **
*******************************************************************************/



#include "L610_TCP.h"
#include <stdio.h> 


char *strstr(const char *, const char *); 
extern char *strx,*extstrx,*Readystrx;
extern 	char RxBuffer[200],Rxcouter;


/*****************************************************

	请求运营商分配IP
	对应AT指令：AT+mipcall = 
	运营商不同本条指令有区别

******************************************************/

void IP_allo(void)
{
	printf("AT+mipcall=1,\"ctnet\"\r\n");//请求运营商分配 IP
	HAL_Delay(3000);
	printf("AT+mipcall?\r\n");//检查是否分配IP
	HAL_Delay(3000);
	strx=strstr((const char*)RxBuffer,(const char*)"+MIPCALL: 1");//判断结果有+MIPCALL:1 才能做下一步。

	while(strx==NULL)
	{
		Clear_Buffer();
		printf("AT+mipcall=0\r\n");
		HAL_Delay(2000);
		printf("AT+mipcall=1,\"ctnet\"\r\n");
		HAL_Delay(3000);
		printf("AT+mipcall?\r\n");//检查是否分配IP
		HAL_Delay(3000);
		strx=strstr((const char*)RxBuffer,(const char*)"+MIPCALL: 1");//判断结果有+MIPCALL:1 才能做下一步。
	}
	HAL_Delay(1000);
	Clear_Buffer();
}


/*******************************************************
	
	查询当前可用的socket ID
	socket ID 都可用说明当前没有 TCP 链接。
	对应AT指令：AT+MIPOPEN?

********************************************************/

void Check_socket(void)
{
	
	Clear_Buffer();
	printf("AT+MIPOPEN?\r\n"); 
	HAL_Delay(500);
	strx=strstr((const char*)RxBuffer,(const char*)"+MIPOPEN:");//是否返回
	while(strx==NULL)
	{
		HAL_Delay(1000);
		strx=strstr((const char*)RxBuffer,(const char*)"+MIPOPEN:");//等待返回
	}
	Clear_Buffer();
	
}



/*******************************************************

	通过socket创建TCP连接
	对应AT指令：AT+MIPOPEN=1,,\"47.92.117.163\",30000,0

********************************************************/

void Creat_TCP(void)
{
	Clear_Buffer();
//	uint8_t untildata=0xff;
	
	printf("AT+MIPOPEN=1,,\"120.24.31.181\",3000,0\r\n");//创建TCP连接
	HAL_Delay(3000);
	
	strx=strstr((const char*)RxBuffer,(const char*)"+MIPOPEN: 1,1");//是否成功创建TCP
	while(strx==NULL)
	{
		HAL_Delay(1000);
		strx=strstr((const char*)RxBuffer,(const char*)"+MIPOPEN: 1,1");//等待成功创建TCP
	}
	Clear_Buffer();
}


/**************************************************************

	发送数据：在TCP连接建立后调用才有效，可以向服务器发送数据
	需要传入保存数据的数组
	对应AT指令：AT+MIPSEND

***************************************************************/

void Send_Data(char *bufferdata)
{
	Clear_Buffer();
	HAL_Delay(2000);
	printf("AT+MIPSEND=1,13\r\n");
	printf( "%s",bufferdata);
	HAL_Delay(2000);
	Clear_Buffer();
	
}


/*******************************************************

	关闭TCP连接
	假如数据收发结束或者异常，尝试关闭到服务器的 TCP 连接 
	对应AT指令：AT+MIPCLOSE=1

********************************************************/



void End_TCP(void)
{
	Clear_Buffer();
	HAL_Delay(2000);
	printf("AT+MIPCLOSE=1\r\n");
	
	strx=strstr((const char*)RxBuffer,(const char*)"+MIPCLOSE: 1,0");
	while(strx==NULL)
	{
		HAL_Delay(1000);
		strx=strstr((const char*)RxBuffer,(const char*)"+MIPCLOSE: 1,0");//等待TCP 链接断开
	}
	Clear_Buffer();
	
	
}





/*******************************************************

	释放IP
	尝试释放模块本次激活后获取的 IP 地址  
	对应AT指令：AT+MIPCALL=0

********************************************************/



void IP_release(void)
{
	Clear_Buffer();
	HAL_Delay(2000);
	printf("AT+MIPCALL=0\r\n");
	
	strx=strstr((const char*)RxBuffer,(const char*)"+MIPCALL: 0");
	while(strx==NULL)
	{
		HAL_Delay(1000);
		strx=strstr((const char*)RxBuffer,(const char*)"+MIPCALL: 0");//等待TCP 链接断开
	}
	Clear_Buffer();
	
	
}























