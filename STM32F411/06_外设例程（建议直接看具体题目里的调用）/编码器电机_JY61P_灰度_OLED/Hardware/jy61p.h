#ifndef __JY61P_H
#define __JY61P_H

#include "main.h" 
#include "usart.h"
#include "gw_grayscale_sensor.h"
void jy61p_ReceiveData(uint8_t RxData);
void jy61p_correct(int flag);
extern float Roll,Pitch,Yaw;

#endif
