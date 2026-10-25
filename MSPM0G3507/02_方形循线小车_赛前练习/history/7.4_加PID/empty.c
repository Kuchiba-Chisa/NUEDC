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
#include "pwm.h"
#include "stdio.h"
#include "debug.h"
#include "Encoder.h"
#include "oled.h"
#include "delay.h"
#include "Imu.h"
#include "stdio.h"
#include "hardware_iic.h"
#include "IIC.h"
//volatile unsigned char uart_data = 0;

//void uart0_send_char(char ch); //串口0发送单个字符
//void uart0_send_string(char* str); //串口0发送字符串
extern unsigned char Digtal;
extern int key_flag;
uint8_t BIT[8]={0,0,0,0,0,0,0,0};

int main(void)
{
    SYSCFG_DL_init();
    //清除串口中断标志
    NVIC_ClearPendingIRQ(Imu_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(Imu_INST_INT_IRQN);

    Encoder_Init();
    OLED_Init();    //初始化OLED
    OLED_Clear();
    IMU_Init();
    extern float Yaw;
    int motor_test=1000;
    //uart0_send_string("uart0 start work\r\n");
    int i = 0;
    // SYSCFG_DL_init();
    // Set_PWM(0, 0);
    // Set_PWM(3000, 3000);
    // Set_PWM(-3000, 3000);
    // Set_PWM(-3000, -3000);
    // Set_PWM(3000, -3000);
    // Set_PWM(0, 0);
    char buffer[32];
    // delay_cycles(32000000);
    while (1)
    {
        if(key_flag==1)
        {
            float offset_Yaw=Yaw-0;
			// float angle_pid=angle_PID_Controller(offset_Yaw,1);
			// AdjustMotorSpeed(angle_pid);
            // ReadSensors(BIT);               
			// int offset = CalculateOffset(BIT);
            // float pid_output = ComputePID(offset);   
			// AdjustMotorSpeed(pid_output);           
        }
        snprintf(buffer, sizeof(buffer), "%f", Yaw);
        OLED_ShowString(0,0,(uint8_t *)buffer,8,1);//6*8 “ABC”
        OLED_Refresh();
        if(DL_GPIO_readPins(KEY_PORT, KEY_KEY_2_Pin_PIN) == 0)
        {
            OLED_ShowNum(0,8,1,1,8,1);//6*8 “ABC”
        }

    }
}

 //void uart0_send_char(char ch)
 //{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
     //while( DL_UART_isBusy(UART_0_INST) == true );
    //发送单个字符
     //DL_UART_Main_transmitData(UART_0_INST, ch);
 //}
//串口发送字符串
 //void uart0_send_string(char* str)
 //{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
     //while(*str!=0&&str!=0)
    // {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        //  uart0_send_char(*str++);
    // }
 //}
