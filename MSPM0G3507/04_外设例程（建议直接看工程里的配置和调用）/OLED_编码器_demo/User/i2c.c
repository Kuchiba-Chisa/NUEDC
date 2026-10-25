/*
 * i2c.c
 *
 *  Created on: 2024年1月11日
 *      Author: junying
 */

#include "i2c.h"

// 寄存器地址变量
uint8_t gTxRegAddr;

// 延时宏定义（约1ms）
#define DELAY (32000)
void delay_ms(uint16_t ms)
{
    while(ms--)
        delay_cycles(DELAY);
}

/**
 * @brief 通过I2C写入多个字节数据
 * @param DevAddr 设备地址
 * @param RegAddr 寄存器地址
 * @param buf 数据缓冲区指针
 * @param nBytes 要写入的字节数
 * @return 实际写入的字节数
 */
uint8_t I2C_Write_Bytes(uint8_t DevAddr, uint8_t RegAddr, uint8_t *buf, uint8_t nBytes)
{
    uint8_t n;
    uint32_t Byte4Fill;
    gTxRegAddr = RegAddr; // 保存寄存器地址

    // 预填充TX FIFO
    DL_I2C_fillControllerTXFIFO(I2C_INST, &buf[0], nBytes);

    /* 等待I2C控制器空闲 */
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_flushControllerTXFIFO(I2C_INST); // 清空FIFO
    DL_I2C_fillControllerTXFIFO(I2C_INST, &gTxRegAddr, 1); // 写入寄存器地址

    /* 启动控制器传输（自动发送Start+Stop信号）
     * 参数说明：
     * DevAddr - 从设备地址
     * DL_I2C_CONTROLLER_DIRECTION_TX - 传输方向（写）
     * (nBytes+1) - 总传输字节数（寄存器地址+数据）
     */
    DL_I2C_startControllerTransfer(I2C_INST, DevAddr,
        DL_I2C_CONTROLLER_DIRECTION_TX, (nBytes+1));
    
    // 动态填充剩余数据
    n = 0;
    do {
        Byte4Fill = DL_I2C_getControllerTXFIFOCounter(I2C_INST); // 获取FIFO剩余空间
        if(Byte4Fill > 1)
        {
            DL_I2C_fillControllerTXFIFO(I2C_INST, &buf[n], 1); // 填充1字节数据
            n++;
        }
    } while(n < nBytes);

    /* 等待总线传输完成 */
    while (DL_I2C_getControllerStatus(I2C_INST) &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    /* 错误处理 */
    if (DL_I2C_getControllerStatus(I2C_INST) &
        DL_I2C_CONTROLLER_STATUS_ERROR) {
        /* 如果出错触发断点 */
        __BKPT(0);
    }

    /* 传输间隔延时 */
    delay_cycles(1000);

    return nBytes;
}