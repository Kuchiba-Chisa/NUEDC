#include "IMU.h"
#include <string.h> 
#include "Delay.h" 
#include "oled.h"
float Yaw;
float Last_Yaw;
float Gyro;
float Last_Gyro;
uint8_t Usart2_Rx_Buffer[RXBUFFERSIZE];
uint8_t Usart2_Rx_Flag;

void IMU_Init(void) 
{
	HAL_UART_Receive_DMA(&huart2,(uint8_t *)Usart2_Rx_Buffer,RXBUFFERSIZE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	uint8_t temp2[5] = {0xFF,0xAA,0x76,0x00,0x00}; //z轴置零
	HAL_UART_Transmit(&huart2,temp2,5,0xffff);
}

//void IMU_Proc(void)//把偏向角都变成正的23 
//{
//	
//	if (Usart2_Rx_Flag == 1)
//	{
//		DEBUG_info("IMU_Proc","Usart2_Rx_Flag == 1");
//		Yaw = (float)(Usart2_Rx_Buffer[18] << 8 | Usart2_Rx_Buffer[17]) / 32768 * 180;
//		DEBUG_info("IMU_Proc:Yaw","=%f",Yaw);
//		if (Yaw - Last_Yaw> 180)//处理跳变
//		{
//			Yaw -= 360;
//		}
//		else if (Yaw - Last_Yaw<-180)
//		{
//				Yaw += 360;
//		}
//		Last_Yaw = Yaw;
//		memset(Usart2_Rx_Buffer, 0, RXBUFFERSIZE);
//		Usart2_Rx_Flag = 0;
//	}
//}
void IMU_Proc(void) // 读取HWT101陀螺仪的偏航角和角速度
{
    if (Usart2_Rx_Flag == 1)
    {
//        DEBUG_info("IMU_Proc","Usart2_Rx_Flag == 1");
//       OLED_Printf(0, 32, OLED_8X16, "Usart2_Rx_Flag:%d",Usart2_Rx_Flag);
 
        // 读取偏航角
			if(Usart2_Rx_Buffer[12]==83)
			{
					Yaw = (float)(Usart2_Rx_Buffer[18] << 8 | Usart2_Rx_Buffer[17]) / 32768 * 180;
//        DEBUG_info("IMU_Proc:Yaw","=%f",Yaw);
			}

			if(Usart2_Rx_Buffer[1]==82)
			{
        Gyro = (float)(Usart2_Rx_Buffer[7] << 8 | Usart2_Rx_Buffer[6]) / 32768 * 2000; // 2000dps量程
//        DEBUG_info("IMU_Proc:Gyro","=%f",Gyro);
			}        
        // 读取角速度（Z轴角速度）

//        DEBUG_info("Buffer","=%d",Usart2_Rx_Buffer[1]);

        // 处理角度跳变
        if (Yaw - Last_Yaw > 180)
        {
            Yaw -= 360;
        }
        else if (Yaw - Last_Yaw < -180)
        {
            Yaw += 360;
        }
        Last_Yaw = Yaw;
				//处理角速度跳变
        if (Gyro - Last_Gyro > 2000)
        {
            Gyro -= 4000;
        }
        else if (Gyro - Last_Gyro < -2000)
        {
            Gyro += 4000;
        }
        Last_Gyro = Gyro;
        memset(Usart2_Rx_Buffer, 0, RXBUFFERSIZE);
        Usart2_Rx_Flag = 0;
    }
}
