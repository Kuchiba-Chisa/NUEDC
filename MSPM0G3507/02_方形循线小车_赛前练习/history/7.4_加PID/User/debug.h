#ifndef DEBUG_H
#define DEBUG_H

#include "ti_msp_dl_config.h"
#include <stdio.h>



void Debug_INST_IRQHandler(void);
void Debug_send_char(char ch);
int fputc(int ch, FILE *stream);


#endif 