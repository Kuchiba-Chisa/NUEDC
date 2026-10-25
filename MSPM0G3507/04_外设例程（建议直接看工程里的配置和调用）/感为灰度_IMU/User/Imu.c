#include "Imu.h"
#include "ti_msp_dl_config.h"
#include "string.h"
float Yaw;
float Last_Yaw;
float Gyro;
float Last_Gyro;
uint8_t Usart2_Rx_Buffer[RXBUFFERSIZE];
uint8_t Usart2_Rx_Flag;
volatile uint16_t Usart2_Rx_Cnt = 0;     // 接收计数器
volatile uint8_t Usart2_Rx_Ready = 0;    // 接收完成标志
char tempBuffer[100] = "";
uint8_t temp2[5] = {0xFF, 0xAA, 0x76, 0x00, 0x00}; // z轴置零
#define UART_Imu_PACKET_SIZE (100)

void IMU_Init(void) 
{
        //清除串口中断标志
    NVIC_ClearPendingIRQ(Imu_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(Imu_INST_INT_IRQN);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&Imu_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) temp2);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, UART_Imu_PACKET_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
}
void IMU_Proc(void) // 读取HWT101陀螺仪的偏航角和角速度
{
    if (Usart2_Rx_Flag == 1)
    {
//        DEBUG_info("IMU_Proc","Usart2_Rx_Flag == 1");
        
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
        else if (Yaw - Last_Yaw < -2000)
        {
            Gyro += 4000;
        }
        Last_Gyro = Gyro;
        memset(Usart2_Rx_Buffer, 0, RXBUFFERSIZE);
        Usart2_Rx_Flag = 0;
    }
}
void Imu_INST_IRQHandler(void)
{
    // 获取中断状态
    switch(DL_UART_getPendingInterrupt(Imu_INST))
    {
        case DL_UART_IIDX_RX:  // 接收中断
            // 检查缓冲区是否已满
            if(Usart2_Rx_Cnt < RXBUFFERSIZE) {
                // 读取接收数据并存储
                Usart2_Rx_Buffer[Usart2_Rx_Cnt++] = DL_UART_Main_receiveData(Imu_INST);
                
                // 可选：根据协议设置接收完成条件
                // 示例1：检测到回车换行结束符
                /*
                if(Usart2_Rx_Cnt >= 2 && 
                   Usart2_Rx_Buffer[Usart2_Rx_Cnt-1] == '\n' &&
                   Usart2_Rx_Buffer[Usart2_Rx_Cnt-2] == '\r') {
                    Usart2_Rx_Ready = 1;
                }
                */
                
                // 示例2：固定长度数据包（如5字节）
                /*
                if(Usart2_Rx_Cnt >= 5) {
                    Usart2_Rx_Ready = 1;
                }
                */
            }
            else {
                // 缓冲区溢出处理
                Usart2_Rx_Cnt = 0;  // 重置计数器
                memset((void*)Usart2_Rx_Buffer, 0, RXBUFFERSIZE);
                // 可添加错误处理代码
            }
            Usart2_Rx_Flag=1;

            break;

        default:  // 发送中断（可选处理）
            break;
    }
}
void uart1_send_byte(uint8_t data)
{
    // 等待串口空闲

    // 发送单字节数据
    DL_UART_Main_transmitData(Imu_INST, data);
}

// 发送二进制数组（新增函数）
void uart1_send_binary(uint8_t *data, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++) {
        uart1_send_byte(data[i]);
    }
}
