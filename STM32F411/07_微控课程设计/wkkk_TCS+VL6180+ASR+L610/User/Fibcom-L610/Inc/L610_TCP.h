/*******************************************************************************
**                       @ Copyright 2014 - 2020                              **
**                           www.diysoon.com                                  **
**                                                                            **
*******************************************************************************/


#ifndef L610_TCP_H
#define L610_TCP_H
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "L610_Init.h"




/* function declaration -----------------------------------------------------*/

extern void IP_allo(void);
extern void Send_Data(char *bufferdata);
extern void Check_socket(void);

extern void Creat_TCP(void);
extern void End_TCP(void);
extern void IP_release(void);
	 
	 
	 
	 
	 
#endif


