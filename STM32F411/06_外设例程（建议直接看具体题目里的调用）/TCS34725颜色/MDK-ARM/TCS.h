#include <stdio.h>
#include "main.h"
#ifndef __TCS_H
#define __TCS_H
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

void TCS34725_ENABLE();
uint16_t TCS34725_Get_RawData(I2C_HandleTypeDef *hi2c,uint16_t MemAddress);
void Send_RAWData_To_UART(char RGBC, uint16_t RawData);
#endif 