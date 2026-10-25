#ifndef _ENCODER_H
#define _ENCODER_H

#include "main.h"
#include <stdio.h>

int Read_Encoder(uint8_t TIMX);
int Read_Speed(TIM_HandleTypeDef *htim);

#endif
