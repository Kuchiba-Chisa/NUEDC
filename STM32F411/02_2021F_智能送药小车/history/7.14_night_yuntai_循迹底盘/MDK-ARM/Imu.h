#ifndef __IMU_H__ 
#define __IMU_H__

#include "main.h"
#include "usart.h"
void IMU_Init(void); 
extern float Yaw;
extern float Gyro;
 //extern float Last Yaw;
void IMU_Proc(void);
#define RXBUFFERSIZE 50
extern uint8_t Usart2_Rx_Buffer[RXBUFFERSIZE];
extern uint8_t Usart2_Rx_Flag;
extern uint8_t Usart2_aRxBuffer[RXBUFFERSIZE];

#endif

