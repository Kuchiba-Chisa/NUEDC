#ifndef DEBUG_H
#define DEBUG_H

#include "ti_msp_dl_config.h"





void Debug_INST_IRQHandler(void);
void Debug_send_char(char ch);
int fputc(int ch, FILE *stream);
int fputs(const char* restrict s, FILE* restrict stream);
int puts(const char* _ptr);

#endif 