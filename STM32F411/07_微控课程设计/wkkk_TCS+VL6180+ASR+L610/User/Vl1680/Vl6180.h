#include <stdio.h>
#include "main.h"
#ifndef __VL6180_H
#define __VL6180_H

extern I2C_HandleTypeDef hi2c2;
void VL6180X_Init(void);
HAL_StatusTypeDef VL6180X_ReadByte(uint16_t regAddress, uint8_t* data);
HAL_StatusTypeDef VL6180X_WriteByte(uint16_t regAddress, uint8_t data);
uint8_t VL6180X_GetID(void);
uint8_t VL6180X_GetRange(void);
#endif 
