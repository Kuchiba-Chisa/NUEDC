#ifndef __UART_H
#define __UART_H

#include "ti_msp_dl_config.h"

void TLY_Init(void);
void MAIX_Init(void);

void jy61p_correct(int flag);
void jy61p_correct_yaw(int flag);
extern float Roll,Pitch,Yaw;
extern int Velocity_Target_Max;

#endif
