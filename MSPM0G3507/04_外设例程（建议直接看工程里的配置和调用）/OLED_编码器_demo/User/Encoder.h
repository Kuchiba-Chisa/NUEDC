#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include "ti_msp_dl_config.h"

#ifdef __cplusplus
extern "C" {
#endif


void Encoder_Init(void);
void GROUP1_IRQHandler(void);
void ReadSpeed(void);
// void TIMER_Encoder_Read_INST_IRQHandler(void);
// void Encodering(void);
// void Read_Encoder(void);

#ifdef __cplusplus
}
#endif

#endif 