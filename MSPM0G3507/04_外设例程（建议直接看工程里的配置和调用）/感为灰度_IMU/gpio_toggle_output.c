#include "ti_msp_dl_config.h"
#include "Time.h"
#include "stdio.h"
#include "Uart.h"
#include "string.h"
#include "hardware_iic.h"
#include "IIC.h"
/*************************************软件IIC测试Demo**********************************************/
/*****************芯片型号 MSPM0G3507 SystemConfig 标准库******************************************/
/*****************引脚 SDA PA1 SCL PA0  ***********************************************************/
/*****************串口 Tx PA10 Rx PA11 ************************************************************/
/***************************************软件IIC测试Demo********************************************/
unsigned char Digtal;
unsigned char Anolog[8]={0};
unsigned char rx_buff[256]={0};
unsigned char Normal[8]={0};
unsigned char tx_buffer[5]={0x55,0x66,0x77,0x88,0x99};
int main(void)
{
    SYSCFG_DL_init();
		sprintf((char *)rx_buff,"hello_world!\r\n");
		uart0_send_string((char *)rx_buff);
		memset(rx_buff,0,256);	
		while(Ping())
		{
			delay_ms(1);
			sprintf((char *)rx_buff,"Ping Faild Try Again!\r\n");
			uart0_send_string((char *)rx_buff);
			memset(rx_buff,0,256);
		}
		sprintf((char *)rx_buff,"Ping Succseful!\r\n");
		uart0_send_string((char *)rx_buff);
		memset(rx_buff,0,256);
		while (1) {
			Digtal=IIC_Get_Digtal();
			sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
			uart0_send_string((char *)rx_buff);
			memset(rx_buff,0,256);
			
			//获取传感器模拟量结果
			if(IIC_Get_Anolog(Anolog,8)){
			sprintf((char *)rx_buff,"Anolog %d-%d-%d-%d-%d-%d-%d-%d\r\n",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
			uart0_send_string((char *)rx_buff);
			memset(rx_buff,0,256);
			}
			
			//获取传感器归一化结果
			if(IIC_Get_Normalize(Normal,8)){
			sprintf((char *)rx_buff,"Normalize %d-%d-%d-%d-%d-%d-%d-%d\r\n",Normal[0],Normal[1],Normal[2],Normal[3],Normal[4],Normal[5],Normal[6],Normal[7]);
			uart0_send_string((char *)rx_buff);
			memset(rx_buff,0,256);
			}
//			
			delay_ms(1);
		}
}