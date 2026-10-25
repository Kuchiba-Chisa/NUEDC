#ifndef IMU_H
#define IMU_H

#include "ti_msp_dl_config.h"

void IMU_Init(void); 
void Imu_INST_IRQHandler(void);
void uart1_send_byte(uint8_t data);
void uart1_send_binary(uint8_t *data, uint16_t length);
void Imu_ReceiveData(void);
extern float Yaw;
extern float Gyro;
 //extern float Last Yaw;
#define RXBUFFERSIZE 50
extern uint8_t Usart2_Rx_Buffer[RXBUFFERSIZE];
extern uint8_t Usart2_Rx_Flag;
extern uint8_t Usart2_aRxBuffer[RXBUFFERSIZE];

#endif