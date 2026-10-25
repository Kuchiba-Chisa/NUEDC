#ifndef __DUOJI_H
#define __DUOJI_H


void send_servo_command(int servo_id, int pulse_width, int time_ms);
void send_string(UART_HandleTypeDef *huart, const char *str) ;


#endif // !__DUOJI_H
