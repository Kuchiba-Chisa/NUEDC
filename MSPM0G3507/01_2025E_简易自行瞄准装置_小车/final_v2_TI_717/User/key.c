#include "key.h"
#include "ti_msp_dl_config.h"

int key_value=0;
int get_key_value(void)
{
    if(DL_GPIO_readPins(KEY_PORT, KEY_KEY_1_Pin_PIN) == 0)
    {
        key_value=1;
    }
    else if(DL_GPIO_readPins(KEY_PORT, KEY_KEY_2_Pin_PIN) == 0)
    {
        key_value=2;
    }
    else if(DL_GPIO_readPins(KEY_PORT, KEY_KEY_3_Pin_PIN) == 0)
    {
        key_value=3;
    }
    // else {
    //     key_value=0;
    // }
    return key_value;
}



