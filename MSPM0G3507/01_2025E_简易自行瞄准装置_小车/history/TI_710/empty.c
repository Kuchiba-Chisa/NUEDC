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
#include "key.h"
#include "pid.h"
//volatile unsigned char uart_data = 0;

//void uart0_send_char(char ch); //串口0发送单个字符
//void uart0_send_string(char* str); //串口0发送字符串
int digital;
unsigned char rx_buff2[256]={0};
char speed_buffR[256]={0};
char speed_buffL[256]={0};

extern unsigned char Digtal;
int key_flag = 0;
float output;
int N=0;
int num = 0;
int turn_flag;
int findflag;
uint8_t BIT[8]={0,0,0,0,0,0,0,0};
int mode=0;
int encoder_cnt=0;
float Encoder_left = 0;
float Encoder_right = 0;
extern int turn;
extern int speedA;
extern int speedB;
extern int32_t gEncoderCount1;
extern int32_t gEncoderCount2;
extern float Rotation_Speed_L,Rotation_Speed_R;
extern int BASE_SPEED;
int main(void)
{
    SYSCFG_DL_init();
    //清除串口中断标志
    NVIC_ClearPendingIRQ(Imu_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(Imu_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(MAIX_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(MAIX_INST_INT_IRQN);

    Encoder_Init();

    IMU_Init();
    MAIX_Init();
    extern float Yaw;
    int i = 0;
    char buffer[32];
    // Set_PWM(-1000, -1000);
    while (1)
    {   
        while(N<num&&key_flag==1)
        {
            ReadSensors(BIT);
            int offset = CalculateOffset(BIT);
            output = ComputePID(offset);
            AdjustMotorSpeed(output);
            if(turn == 1)   //准备左转，先前进一点
            {
                Set_PWM(BASE_SPEED,BASE_SPEED);
                delay_ms(350);
                findflag=1;
                turn =0;
                turn_flag++;
            }
            ReadSensors(BIT);
            if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==1) //左转直到线在中间
            {
                Set_PWM(-3000,3000);
                ReadSensors(BIT);
                while(BIT[3]==1&&BIT[4]==1)
                {
                    ReadSensors(BIT);
                    findflag=0;
                }
            }
            if (turn_flag == 4) // 完成一圈
            {
                N++;
                turn_flag=0;
            }
            // if(N==num)
            // {
            //     Set_PWM(-2500,2500);
            //     ReadSensors(BIT);
            //     while(BIT[3]==1&&BIT[4]==1)
            //     {
            //         ReadSensors(BIT);
            //     }
            //     Set_PWM(0,0);
            // }
        }

    }
}

void TIMER_0_INST_IRQHandler(void)
{
    encoder_cnt++;
    if(encoder_cnt == 20)
    {
        Encoder_left = gEncoderCount1;
        Encoder_right = gEncoderCount2;

        gEncoderCount1 = 0;
        gEncoderCount2 = 0;
        Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
        encoder_cnt = 0;
    }
}