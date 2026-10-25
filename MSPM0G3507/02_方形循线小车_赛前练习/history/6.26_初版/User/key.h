#ifndef KEY_H
#define KEY_H

#include "ti_msp_dl_config.h"


#define KEY1_PIN         DL_GPIO_PIN_0
#define KEY2_PIN         DL_GPIO_PIN_1
#define KEY3_PIN         DL_GPIO_PIN_2
#define KEY4_PIN         DL_GPIO_PIN_3
#define KEY_ALL_PINS     (KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN)
//void Debounce(uint16_t GPIO_Pin);
void GROUP1_IRQHandler(void);
#endif 