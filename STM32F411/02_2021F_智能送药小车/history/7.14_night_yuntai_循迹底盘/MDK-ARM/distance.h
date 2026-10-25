#ifndef __distance_H
#define __distance_H

#include "main.h"

#define Trig_H	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_SET);
#define Trig_L	HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_RESET);

void get_distance(void);
void get_distance2(void);

#endif // 

