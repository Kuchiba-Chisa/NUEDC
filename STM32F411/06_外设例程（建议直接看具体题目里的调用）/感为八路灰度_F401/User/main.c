#include <stm32f4xx.h>
#include "OLED.h"
#include "Delay.h"
#include "GWKJ.h"

uint8_t digital;

int main(void)
{
	uint8_t BIT[8];
	GW_digital_Init();
	OLED_Init();
	while (1)
	{
		digital = GW_digital_Read();
        OLED_ShowBinNum(0, 0, digital, 8, OLED_8X16);
        OLED_Update();
	}
}
