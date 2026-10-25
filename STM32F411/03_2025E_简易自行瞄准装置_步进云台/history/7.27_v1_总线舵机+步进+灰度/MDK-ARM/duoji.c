#include "main.h"
#include "duoji.h"
#include "math.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
extern UART_HandleTypeDef huart6;

void send_servo_command(int servo_id, int pulse_width, int time_ms) {
    char command[20]; // 足够大的缓冲区
    
    // 格式化指令
    snprintf(command, sizeof(command), "#%03dP%04dT%04d!", servo_id, pulse_width, time_ms);
    
    // 这里替换为你的实际发送函数，例如：
		send_string(&huart6,command); 
    // 如果是嵌入式环境，可能需要通过串口发送：
    // HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
}

void send_string(UART_HandleTypeDef *huart, const char *str) 
{
    HAL_UART_Transmit(huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}