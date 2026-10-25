#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include "ti_msp_dl_config.h"

void Encoder_Init(void);
void ReadSpeed(void);
void GROUP1_IRQHandler(void);


#endif 