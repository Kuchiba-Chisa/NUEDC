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
#include "Encoder.h"
#include "oled.h"
#include "delay.h"
#include "stdio.h"
#include "key.h"
#include "pid.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"

//volatile unsigned char uart_data = 0;

//void uart0_send_char(char ch); //串口0发送单个字符
//void uart0_send_string(char* str); //串口0发送字符串
//无mcu感为数组
unsigned char rx_buff_gw[256]={0};
unsigned short Anolog[8]={0};
unsigned short white[8]={2500,2500,2500,2500,2500,2500,2500,2500};
unsigned short black[8]={800,800,800,800,800,800,800,800};
unsigned short Normal[8];
extern unsigned char Digtal_2;

int digital;
unsigned char rx_buff2[256]={0};
char speed_buffR[256]={0};
char speed_buffL[256]={0};

extern unsigned char Digtal;
int key_flag = 0;
uint8_t BIT[8]={0,0,0,0,0,0,0,0};
int mode=0;
int encoder_cnt=0;
float Encoder_left = 0;
float Encoder_right = 0;
extern int speedA;
extern int speedB;
extern int32_t gEncoderCount1;
extern int32_t gEncoderCount2;
extern float Velocity_Left,Velocity_Right;
extern int turn;
extern int motor_left;
extern int motor_right;
int Imuflag=0;
int findflag=0;
float output;
extern int BASE_SPEED;
int main(void)
{
    SYSCFG_DL_init();
    //清除串口中断标志


    Encoder_Init();
    OLED_Init();
    No_MCU_Sensor sensor;
    int i = 0;
    char buffer[32];
    int N = 0; //圈数
    int turn_flag=0;
    No_MCU_Ganv_Sensor_Init_Frist(&sensor);
	No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
	Get_Anolog_Value(&sensor,Anolog);

	delay_ms(100);
	memset(rx_buff_gw,0,256);
	//寰楀埌榛戠櫧鏍″噯鍊间箣鍚庯紝鍒濆鍖栦紶鎰熷櫒
	No_MCU_Ganv_Sensor_Init(&sensor,white,black);
    // Set_PWM(2000, 2000);
    while (1)
    {         
        OLED_Refresh();
        No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
		Digtal_2=Get_Digtal_For_User(&sensor);
        for(int i = 0; i < 8; i++) {
            BIT[i] = (Digtal_2 >> i) & 0x01;
        }
		sprintf((char *)rx_buff_gw,"D %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal_2>>0)&0x01,(Digtal_2>>1)&0x01,(Digtal_2>>2)&0x01,(Digtal_2>>3)&0x01,(Digtal_2>>4)&0x01,(Digtal_2>>5)&0x01,(Digtal_2>>6)&0x01,(Digtal_2>>7)&0x01);
	    OLED_ShowString(0, 32, rx_buff_gw, 8,1);
		memset(rx_buff_gw,0,256);
        // ReadSensors(BIT);
        // OLED_ShowNum(0,32,BIT[2],1,8,1);
       
        // if(Get_Anolog_Value(&sensor,Anolog)){
        //     sprintf((char *)rx_buff_gw,"A %d-%d-%d-%d-%d-%d-%d-%d\r\n",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
	    //     OLED_ShowString(0, 16, rx_buff_gw, 8,1);
        //     memset(rx_buff_gw,0,256);
        // }
        // if(Get_Normalize_For_User(&sensor,Normal)){
        //     sprintf((char *)rx_buff_gw,"N %d-%d-%d-%d-%d-%d-%d-%d\r\n",Normal[0],Normal[1],Normal[2],Normal[3],Normal[4],Normal[5],Normal[6],Normal[7]);
        //     OLED_ShowString(0, 48, rx_buff_gw, 8,1);
        //     memset(rx_buff_gw,0,256);
        // }
        // OLED_ShowNum(0,8,gEncoderCount1,8,8,1);
        // OLED_ShowNum(0,16,gEncoderCount2,8,8,1);
        // ReadSensors(BIT);
        int offset = CalculateOffset(BIT);
        output = ComputePID(offset);
        AdjustMotorSpeed(output);
        // SpeedControl(motor_left,motor_right,Velocity_Left*50,Velocity_Right*50,1);
        // if(turn == 1)   //准备左转，先前进一点
        // {
        //     Set_PWM(BASE_SPEED,BASE_SPEED);
		// 	delay_ms(50);
		// 	findflag=1;
		// 	turn =0;
        //     turn_flag++;
        // }
        // if(turn == 2) //准备右转，先前进一点
        // {
        //     Set_PWM(BASE_SPEED,BASE_SPEED);
		// 	delay_ms(50);
		// 	findflag=2;
		// 	turn =0;
        // }
        // if((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==1) //左转直到线在中间
        // {
        //     Set_PWM(-3500,3500);

        //     while(BIT[3]==1&&BIT[4]==1)
        //     {
        //         ReadSensors(BIT);
        //         findflag=0;
        //     }
        // }
        // while((BIT[0]+BIT[1]+BIT[2]+BIT[3]+BIT[4]+BIT[5]+BIT[6]+BIT[7])>6&&findflag==2) //右转直到线在中间
        // {
        //     Set_PWM(3000,-3000);
        //     ReadSensors(BIT);
        //     if(BIT[3]==0||BIT[4]==0)
        //     {
        //         findflag=0;
        //     }
        // }          
        // key_flag=get_key_value();
        // if ((Yaw - last_Yaw > 30) || (Yaw - last_Yaw < -30)) //发生过转弯
        // {
        //     last_Yaw = Yaw;
        //     turn_flag++;
        // }
        if (turn_flag == 4) // 完成一圈
        {
            N++;
        }
        delay_ms(1);
    }
}

void TIMER_0_INST_IRQHandler(void)
{
    encoder_cnt++;
    if(encoder_cnt == 5)
    {
        Encoder_left = -gEncoderCount1;
        Encoder_right = gEncoderCount2;
        // Get_distance(Encoder_left, Encoder_right);
        gEncoderCount1 = 0; 
        gEncoderCount2 = 0;
        Get_Velocity_Form_Encoder(Encoder_left,Encoder_right);
        encoder_cnt = 0;
    }
}