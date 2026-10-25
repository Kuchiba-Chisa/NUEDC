/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "oled.h"
#include "bmp.h"
#include "motor.h"
#include "encoder.h"
#include "control.h"
#include "string.h"
#include "stdio.h"


volatile uint8_t gEchoData[128];
volatile uint8_t count = 0;
int stop_flag = 0, stand_flag = 0, correct_flag = 0, mode = 3, correct_yaw = 0;
extern int Encoder_left;
extern int Encoder_right;
extern float Velocity_Left,Velocity_Right;


int main(void)
{
    SYSCFG_DL_init();

    IRQ_Init();
    uint8_t t='0';
    OLED_Init();		//初始化OLED
    OLED_Clear();
    while(1) 
    {
        
        // sensor_data = gw_gray_serial_read();
		// SEP_ALL_BIT8(sensor_data,
		// 	sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
		// gw_delay(100*GW_GRAY_SERIAL_DELAY_VALUE);
        OLED_ShowNum(10,0,Encoder_left,6,16);
        OLED_ShowFloatNum(10,2,Encoder_right,3,3,16);
        OLED_ShowNum(10,4,Velocity_Left,6,16);
        OLED_ShowSignedNum(10,6,Velocity_Right,6,16);
        //OLED_ShowNum(70,6,count++,3,16);
        
       
    }
}


