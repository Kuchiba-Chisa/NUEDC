#ifndef PRINTF_H_
#define PRINTF_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 外部变量声明 */
extern volatile unsigned char uart_data;

/* 函数声明 */
int fputc(int ch, FILE *stream);
int fputs(const char* restrict s, FILE* restrict stream);
int puts(const char* _ptr);
void uart0_send_char(char ch);
void uart0_send_string(char* str);
void UART_0_INST_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* PRINTF_H_ */