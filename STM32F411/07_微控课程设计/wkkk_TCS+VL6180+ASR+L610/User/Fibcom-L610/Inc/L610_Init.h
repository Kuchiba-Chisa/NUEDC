/*******************************************************************************
**                       @ Copyright 2014 - 2019                              **
**                           www.diysoon.com                                  **
**                                                                            **
*******************************************************************************/


#ifndef L610_INIT_H
#define L610_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"


/* function declaration -----------------------------------------------------*/


extern void OPEN_MOUDLE(void);
extern void WAIT_OPEN(void);
extern void Clear_Buffer(void);
extern void Fibcom_L610_init(void);
extern void AT_check(void);
extern void CSQ_check(void);
extern void ATI_check(void);
extern void CPIN_check(void);
extern void GPRS_check(void);
extern void EPS_check(void);
extern void Hex_set(void);

extern void Fibcom_L610_init_pre(void);
extern void Deactive_PDP(void);





#ifdef __cplusplus
}
#endif
#endif /*L610_Init_H */
