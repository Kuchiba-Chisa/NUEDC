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
extern int turn;
int findflag;
int turn_flag;
extern unsigned char Digtal;
extern int key_flag;
extern int BASE_SPEED_left;
extern int BASE_SPEED_right;
int N = 0;
extern int num;
uint8_t BIT[8]={0,0,0,0,0,0,0,0};
int mode=0;
int encoder_cnt=0;
int delay_flag=0;
extern int speedA;
extern int speedB;
extern int recieve;
int Encoder_left,Encoder_right;
extern int32_t gEncoderCount_L1, gEncoderCount_L2;
extern float Rotation_Speed_L,Rotation_Speed_R;
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
    int send_flag=0;
    char buffer[32];
    // Set_PWM(4000, 3500);
    // Set_PWM(-2000, 2000);
    // Set_PWM(-2000, -2000);
    // Set_PWM(2000, -2000);
    while (1)
    {
        // uart0_send_char('1');

        // OLED_Refresh();
        // ReadSensors_No_mcu(BIT);
        if(key_flag==2)
        {
            ReadSensors_No_mcu(BIT);
            int offset = CalculateOffset(BIT);
            float pid_output = ComputePID(offset);   
            AdjustMotorSpeed(pid_output); 
            if(turn == 1)   
            {
                Set_PWM(BASE_SPEED_left,BASE_SPEED_right);
                delay_ms(90);
                findflag=1;
                turn =0;
                turn_flag++;
            }
            if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==1) //宸﹁浆鐩村埌绾垮湪涓棿
            {
                Set_PWM(-3000,3000);
                ReadSensors_No_mcu(BIT);
                while(BIT[3]==1&&BIT[4]==1)
                {
                    ReadSensors_No_mcu(BIT);
                    findflag=0;
                }
            }
            if(turn_flag==4)
            {
                N++;
                if(N==num)
                {
                    Set_PWM(-3000,3000);
                    ReadSensors_No_mcu(BIT);
                    while(BIT[3]==1&&BIT[4]==1)
                    {
                        ReadSensors_No_mcu(BIT);
                        findflag=0;
                    }
                    Set_PWM(0, 0);
                    key_flag=0;
                }
                turn_flag=0;
            }
            
        }
        if(key_flag==3) //转弯发1，竖线发2，远横线发3，近横线发4
        {
            // if(recieve==1)
            // {
            //     uart0_send_char('0');

            // }
            // uart0_send_char('0');
            ReadSensors_No_mcu(BIT);
            if(delay_flag==0)
            {

                Set_PWM(2000, 2000);
                delay_ms(250);
                delay_flag++;
            }
            int offset = CalculateOffset(BIT);
            float pid_output = ComputePID(offset);   
            AdjustMotorSpeed(pid_output); 
            ReadSensors_No_mcu(BIT);
            if(turn == 1)   
            {
                uart0_send_char('1');
                Set_PWM(2000,2000);
                delay_ms(300);

                findflag=1;
                turn =0;
                turn_flag++;

            }
            ReadSensors_No_mcu(BIT);

            if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==1) //宸﹁浆鐩村埌绾垮湪涓棿
            {
                Set_PWM(-2200,2200);
                ReadSensors_No_mcu(BIT);
                while(BIT[3]==1&&BIT[4]==1)
                {
                    ReadSensors_No_mcu(BIT);
                    findflag=0;
                }
                
                uart0_send_char('0');
                uart0_send_char('0');
                uart0_send_char('0');
            
                
            }
            if(turn_flag==4)
            {
                N++;
                // uart0_send_char('4');

                if(N==num)
                {
                    uart0_send_char('1');
                    Set_PWM(-2200,2200);
                    // uart0_send_char('2');

                    ReadSensors_No_mcu(BIT);
                    while(BIT[3]==1&&BIT[4]==1)
                    {
                        ReadSensors_No_mcu(BIT);
                        findflag=0;
                    }
                    while (turn==0) 
                    {
                        ReadSensors_No_mcu(BIT);
                        int offset = CalculateOffset(BIT);
                        float pid_output = ComputePID(offset);   
                        AdjustMotorSpeed(pid_output); 
                    }
                    Set_PWM(0, 0);
                    // uart0_send_char('5');
                    key_flag=0;
                }
                turn_flag=0;
            }
            // if(turn_flag==1&&send_flag==0)
            // {
            //     uart0_send_char('1');
            //     send_flag++;
            // }
            // if(turn_flag==2&&send_flag==1)
            // {
            //     uart0_send_char('2');
            //     send_flag++;
            // }
            // if(turn_flag==3&&send_flag==2)
            // {
            //     uart0_send_char('3');     
            //     send_flag++;
            // }
        } 
        if(key_flag==4)
        {
            ReadSensors(BIT);
            int offset = CalculateOffset(BIT);
            float pid_output = ComputePID(offset);   
            AdjustMotorSpeed(pid_output); 
            if(turn == 1)   
            {
                Set_PWM(BASE_SPEED_left,BASE_SPEED_right);
                delay_ms(150);
                findflag=1;
                turn =0;
                turn_flag++;
            }
            if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==1) //宸﹁浆鐩村埌绾垮湪涓棿
            {
                Set_PWM(-3000,3000);
                ReadSensors(BIT);
                while(BIT[3]==1&&BIT[4]==1)
                {
                    ReadSensors(BIT);
                    findflag=0;
                }
            }
            if(turn_flag==4)
            {
                N++;
                if(N==4)
                {
                    Set_PWM(-3000,3000);
                    ReadSensors(BIT);
                    while(BIT[3]==1&&BIT[4]==1)
                    {
                        ReadSensors(BIT);
                        findflag=0;
                    }
                    Set_PWM(0, 0);
                    key_flag=0;
                }
                turn_flag=0;
            }
            
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


void TIMER_0_INST_IRQHandler(void)
{
    encoder_cnt++;
    if(encoder_cnt == 20)
    {
        Encoder_left = gEncoderCount_L1;
        Encoder_right = gEncoderCount_L2;

        gEncoderCount_L1 = 0;
        gEncoderCount_L2 = 0;
        Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
        // if(Inline == 1)
        // {
        //     Turn_Target = Cam_Sensor();  
        // }
        encoder_cnt = 0;
    }
}