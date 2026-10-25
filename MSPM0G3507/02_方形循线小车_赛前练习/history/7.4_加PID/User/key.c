#include "key.h"
#include "ti_msp_dl_config.h"

int get_key_value(void)
{
    if(DL_GPIO_readPins(KEY_PORT, KEY_KEY_2_Pin_PIN) == 0)
    {
        OLED_ShowNum(0,8,1,1,8,1);//6*8 “ABC”
    }    
}
