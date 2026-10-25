#include "Imu.h"
#include "ti_msp_dl_config.h"
#include "string.h"
#include "oled.h"
#include "stdio.h"
float Yaw;
float Last_Yaw;
float Last_Gyro;
int xunji[8];
int panduan[6];
char buffer[32];
static uint8_t TLY_RxBuffer[11];/*接收数据数组*/
static volatile uint8_t TLY_RxState = 0;/*接收状态标志位*/
static uint8_t TLY_RxIndex = 0;/*接受数组索引*/
static uint8_t BLE_RxBuffer[11];/*接收数据数组*/
static volatile uint8_t BLE_RxState = 0;/*接收状态标志位*/
static uint8_t BLE_RxIndex = 0;/*接受数组索引*/
static uint8_t MAIX_RxBuffer[11];/*接收数据数组*/
static volatile uint8_t MAIX_RxState = 0;/*接收状态标志位*/
static uint8_t MAIX_RxIndex = 0;/*接受数组索引*/
float Roll,Pitch,Yaw;/*角度信息，如果只需要整数可以改为整数类型*/
unsigned char transmitdata[50];
char tempBuffer[100] = "";
char tempMAIX[6] = "";
uint8_t RxBuffer_2[RXBUFFERSIZE];
uint8_t Uart_RxBuffer_2;
volatile uint16_t Uart_Rx_Cnt_2 = 0;     // 接收计数器
volatile uint8_t Usart2_Rx_Ready = 0;    // 接收完成标志
uint8_t temp2[5] = {0xFF, 0xAA, 0x76, 0x00, 0x00}; // z轴置零
float Roll,Pitch,Yaw;/*角度信息，如果只需要整数可以改为整数类型*/
float Gyro_x, Gyro_y, Gyro_z;
#define UART_Imu_PACKET_SIZE (100)
#define UART_MAIX_PACKET_SIZE (6)

void MAIX_Init(void)
{
	NVIC_ClearPendingIRQ(MAIX_INST_INT_IRQN);
	NVIC_EnableIRQ(MAIX_INST_INT_IRQN);
	DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)(&MAIX_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) tempMAIX);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, UART_MAIX_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
}

void IMU_Init(void) 
{
        //清除串口中断标志
    NVIC_ClearPendingIRQ(Imu_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(Imu_INST_INT_IRQN);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&Imu_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) tempBuffer);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, UART_Imu_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    uart1_send_binary(temp2, sizeof(temp2));

}

void Imu_Zero(void)
{
    uart1_send_binary(temp2, sizeof(temp2));
}

void Imu_ReceiveData(void) // 读取HWT101陀螺仪的偏航角和角速度
{
    uint8_t i=0,j=0,k=0,l=0;
    for(i=0;i<100;i++)
    {
       if(tempBuffer[i] == 0x55)
        {
            if(tempBuffer[i+1] == 0x52)
            {	uint8_t sum = 0;
                TLY_RxBuffer[0] = 0x55;
                TLY_RxBuffer[1] = 0x52;
                for(k=0;k<9;k++)
                {
                    TLY_RxBuffer[2+k] = tempBuffer[i+2+k];
                } 
			
				/*计算数据，根据数据内容选择对应的计算公式*/
			    Gyro_y = ((int16_t) ((int16_t) TLY_RxBuffer[5] << 8 | (int16_t) TLY_RxBuffer[4])) / 32768.0f * 2000.0f;
			    Gyro_z = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 2000.0f;
            
            }
        }
        if(tempBuffer[i] == 0x55)
        {
            if(tempBuffer[i+1] == 0x53)
            {
				uint8_t sum = 0;
                TLY_RxBuffer[0] = 0x55;
                TLY_RxBuffer[1] = 0x53;
                   for(k=0;k<9;k++)
                   {
                        TLY_RxBuffer[2+k] = tempBuffer[i+2+k];
                   }

				    /*计算数据，根据数据内容选择对应的计算公式*/
                    Last_Yaw=Yaw;
					Yaw = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 180.0f;
                    if((Yaw>60)||(Yaw<0.1&&Yaw>-1)||(Yaw<-60))
                    {
                        Yaw=Last_Yaw;
                    }

             }
            //  else 
            //  {
            //     k = 0;
            //     l = 0;
            //  }
        }
    }
}
void MAIX_ReceiveData(void)
{
    int lentemp=strlen(tempMAIX);
    if((tempMAIX[lentemp-1] == 0x0A)&&(tempMAIX[lentemp-2] == 0x0D)) //判断结束位
    {
    
    // 这里可以写多字节消息的判断
    
    //Usart2_SendString(RxBuffer_2);
    // 解析k210数据
        sscanf((const char *)tempMAIX, "[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\r\n", &xunji[0], &xunji[1],&xunji[2],&xunji[3],&xunji[4], &xunji[5],&xunji[6],&xunji[7],&panduan[0],&panduan[1],&panduan[2],&panduan[3],&panduan[4],&panduan[5]);
        Usart2_Rx_Ready ++;
    //DEBUG_info("K210","K210_Flag:%d\r\n",K210_Flag);
    
    //printf("%d,%d",Position_error[0],Position_error[1]);

    //复位

    }
	
}
void Imu_INST_IRQHandler(void)
{
    // 获取中断状态
    switch(DL_UART_getPendingInterrupt(Imu_INST))
    {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:  // 接收中断
        	Imu_ReceiveData();
            break;
        default:
            break;
    }
}

void MAIX_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(MAIX_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            // MAIX_ReceiveData();
            break;
        default:
            break;
    }
}

void uart1_send_byte(uint8_t data)
{
    // 等待串口空闲

    // 发送单字节数据
    DL_UART_Main_transmitDataBlocking(Imu_INST, data);
}

// 发送二进制数组（新增函数）
void uart1_send_binary(uint8_t *data, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++) {
        uart1_send_byte(data[i]);
    }
}
