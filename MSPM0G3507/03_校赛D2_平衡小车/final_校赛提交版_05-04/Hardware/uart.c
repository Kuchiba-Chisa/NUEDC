
#include "uart.h"

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
float Gyro_x, Gyro_y, Gyro_z;
extern float Balance_Kp,Balance_Kd,Velocity_Kp,Velocity_Ki;

unsigned char transmitdata[50];
char tempBuffer[100] = "";

uint8_t YawAngle_to_zero[5] = {0xFF,0xAA,0x01,0x04,0x00};// 航向角置零
uint8_t YawAngle_to_zero_HWT101[5] = {0xFF,0xAA,0x76,0x00,0x00};
uint8_t Angle_set[5] = {0xFF,0xAA,0x01,0x08,0x00};
uint8_t SAVE[5] = {0xFF,0xAA,0x00,0x00,0x00};
uint8_t Restart[5] = {0xFF,0xAA,0x00,0xFF,0x00};
uint8_t Velocity_flag;
uint8_t gData[6];
char tempcenter_point[2] = "";

extern int correct_yaw_flag;

#define UART_TLY_PACKET_SIZE (100)
#define UART_MAIX_PACKET_SIZE (2)

/**
 * @brief       数据包处理函数
 * @param       串口接收的数据RxData
 * @retval      无
 */
void TLY_Init(void)
{
	NVIC_ClearPendingIRQ(TLY_INST_INT_IRQN);
	NVIC_EnableIRQ(TLY_INST_INT_IRQN);
	DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&TLY_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) tempBuffer);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, UART_TLY_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
	
}

void MAIX_Init(void)
{
	NVIC_ClearPendingIRQ(MAIX_INST_INT_IRQN);
	NVIC_EnableIRQ(MAIX_INST_INT_IRQN);
	DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t)(&MAIX_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) tempcenter_point);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, UART_MAIX_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
	


}

// void TLY_ReceiveData(void)
// {

// 	uint8_t i,j=0,sum=0;
//     for(i=0;i<100;i++)
// 	if (tempBuffer[i] == 0x55)	//等待包头
// 	{
//         TLY_RxBuffer[j] = tempBuffer[i];
//         j++;

// 		if (TLY_RxBuffer[1] == 0x53)	//收到包头
// 		{
// 			for(i=0;i<10;i++)
// 			{
// 				sum = sum + TLY_RxBuffer[i];	//计算校验和
// 			}
// 			if(sum == TLY_RxBuffer[10])		//校验成功
// 			{
// 				/*计算数据，根据数据内容选择对应的计算公式*/
// 				Roll = ((int16_t) ((int16_t) TLY_RxBuffer[3] << 8 | (int16_t) TLY_RxBuffer[2])) / 32768.0f * 180.0f;
// 				Pitch = ((int16_t) ((int16_t) TLY_RxBuffer[5] << 8 | (int16_t) TLY_RxBuffer[4])) / 32768.0f * 180.0f;
// 				Yaw = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 180.0f;
// 			}
// 		}
// 		else if(TLY_RxBuffer[1] == 0x52)
// 		{
// 			for(i=0;i<10;i++)
// 			{
// 				sum = sum + TLY_RxBuffer[i];	//计算校验和
// 			}
// 			if(sum == TLY_RxBuffer[10])		//校验成功
// 			{
// 				/*计算数据，根据数据内容选择对应的计算公式*/
// 				Gyro_x = ((int16_t) ((int16_t) TLY_RxBuffer[3] << 8 | (int16_t) TLY_RxBuffer[2])) / 32768.0f * 2000.0f;
// 				Gyro_y = ((int16_t) ((int16_t) TLY_RxBuffer[5] << 8 | (int16_t) TLY_RxBuffer[4])) / 32768.0f * 2000.0f;
// 				Gyro_z = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 2000.0f;
// 			}
// 		}
// 	}
// }


void TLY_ReceiveData(void)
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
                   for(l=0;l<10;l++)
			    {
				    sum = sum + TLY_RxBuffer[l];	//计算校验和
			    }
			    if(sum == TLY_RxBuffer[10])		//校验成功
			    {
				/*计算数据，根据数据内容选择对应的计算公式*/
			    	Gyro_x = ((int16_t) ((int16_t) TLY_RxBuffer[3] << 8 | (int16_t) TLY_RxBuffer[2])) / 32768.0f * 2000.0f;
			    	Gyro_y = ((int16_t) ((int16_t) TLY_RxBuffer[5] << 8 | (int16_t) TLY_RxBuffer[4])) / 32768.0f * 2000.0f;
			    	Gyro_z = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 2000.0f;
                }
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
                   for(l=0;l<10;l++)
			        {
				      sum = sum + TLY_RxBuffer[l];	//计算校验和
			        }
			        if(sum == TLY_RxBuffer[10])		//校验成功
			     	{
				    /*计算数据，根据数据内容选择对应的计算公式*/
				    	Roll = ((int16_t) ((int16_t) TLY_RxBuffer[3] << 8 | (int16_t) TLY_RxBuffer[2])) / 32768.0f * 180.0f;
				    	Pitch = ((int16_t) ((int16_t) TLY_RxBuffer[5] << 8 | (int16_t) TLY_RxBuffer[4])) / 32768.0f * 180.0f;
						Yaw = ((int16_t) ((int16_t) TLY_RxBuffer[7] << 8 | (int16_t) TLY_RxBuffer[6])) / 32768.0f * 180.0f;
                    	sum = 0;
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

void BLE_ReceiveData(uint8_t RxData)
{
	uint8_t i,sum=0;
	if (BLE_RxState == 0)	//等待包头
	{
		if (RxData == 0xFD)	//收到包头
		{
			BLE_RxBuffer[BLE_RxIndex] = RxData;
			BLE_RxState = 1;
			BLE_RxIndex = 1;	//进入下一状态
		}
	}
	else if (BLE_RxState == 1)
	{
		if (RxData == 0xFC)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			BLE_RxBuffer[BLE_RxIndex] = RxData;
			BLE_RxState = 2;
			BLE_RxIndex = 2;	//进入下一状态
		}
	}
	else if (BLE_RxState == 2)	//接收数据
	{
		BLE_RxBuffer[BLE_RxIndex++] = RxData;
		if(BLE_RxIndex == 8)	//接收完成
		{
			if(BLE_RxBuffer[7] == 0xEF)
			{
				if(BLE_RxBuffer[5] == BLE_RxBuffer[4] + BLE_RxBuffer[3] +BLE_RxBuffer[2])
				{
					if (BLE_RxBuffer[6] == 0xAA) 
					{
						Balance_Kp =100*(100*BLE_RxBuffer[2] + 10*BLE_RxBuffer[3] + BLE_RxBuffer[4]);
					}
					 else if (BLE_RxBuffer[6] == 0xBB) 
					{
						Velocity_Kp = 100*(100*BLE_RxBuffer[2] + 10*BLE_RxBuffer[3] + BLE_RxBuffer[4]);
						Velocity_Ki = Velocity_Kp/200;
					}
					else if (BLE_RxBuffer[6] == 0xCC) 
					{
						 Balance_Kd = 100*BLE_RxBuffer[2] + 10*BLE_RxBuffer[3] + BLE_RxBuffer[4];
					}
                    else if (BLE_RxBuffer[6] == 0xDD) 
					{
						// Cam_Kp = (100*BLE_RxBuffer[2] + 10*BLE_RxBuffer[3] + BLE_RxBuffer[4])/1000;
					}
				}
			}
			BLE_RxState = 0;
			BLE_RxIndex = 0;	//读取完成，回到最初状态，等待包头
		}
	}
}

void TLY_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(TLY_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
			TLY_ReceiveData();
            break;
        default:
            break;
    }
}

void MAIX_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(MAIX_INST)) {
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            break;
        default:
            break;
    }
}

void BLE_INST_IRQHandler(void)
{   
	switch (DL_UART_Main_getPendingInterrupt(BLE_INST)) {
        case DL_UART_MAIN_IIDX_RX:
		    BLE_ReceiveData(DL_UART_Main_receiveData(BLE_INST));
		    break;
        default:
            break;
	}
}

void jy61p_correct(int flag)
{
	if(flag == 1)
	{
//		HAL_UART_Transmit_IT(&huart1,unlock,5);
//		HAL_Delay(50);
//		HAL_UART_Transmit_IT(&huart1,Angle_set,5);
//		HAL_Delay(50);
//		HAL_UART_Transmit_IT(&huart1,YawAngle_to_zero,5);
//		HAL_Delay(50);
//		HAL_UART_Transmit_IT(&huart1,SAVE,5);
//		HAL_Delay(200);
//		HAL_UART_Transmit_IT(&huart1,unlock,5);
//		HAL_Delay(200);
		

//		HAL_UART_Transmit_IT(&huart1,SAVE,5);
//	  HAL_Delay(100);
//		HAL_UART_Transmit_IT(&huart1,Restart,5);
//		HAL_Delay(500);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X69);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X88);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XB5);
	delay_ms(10);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X01);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X04);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
	delay_ms(10);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X01);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X08);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
	delay_ms(10);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
	DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
    delay_ms(50);
	}
}	

void jy61p_correct_yaw(int flag)
{
	if(flag == 1)
	{
		correct_yaw_flag = 1;
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X69);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X88);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XB5);
		delay_ms(2);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X01);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X04);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
		delay_ms(2);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XFF);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0XAA);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
		DL_UART_Main_transmitDataBlocking(TLY_INST,0X00);
		delay_ms(5);
		correct_yaw_flag = 0;
	}
}

