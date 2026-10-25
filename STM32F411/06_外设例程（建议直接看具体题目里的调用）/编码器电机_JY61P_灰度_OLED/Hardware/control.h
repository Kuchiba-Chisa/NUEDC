#include "tim.h"
#include "main.h"
#include "encoder.h"
#include "jy61p.h"
#include "adc.h"
#include "stm32f4xx.h"  
#include "gw_grayscale_sensor.h"

#ifndef __CONTROL_H 
#define __CONTROL_H 


void STOP_turn90(void);
void Big_Turn(void);
float gw_Sensor(void);
void self_turn(void);
#endif
