#include <stdint.h>
#include "main.h"
#include "Bujin.h"
#include "delay.h"
#include "pid_2.h"
#include "math.h"
#include "OLED.h"


extern UART_HandleTypeDef huart6;
PIDController pid_x, pid_y;
extern int current_x,current_y;
extern uint16_t angle_x,angle_y;
void ControlSystem_Init(void)
{
    PID_Init(&pid_x, 0.3f, 0.0f, 0.5f, 1000.0f);  // X轴PID
    PID_Init(&pid_y, 0.3f, 0.0f, 0.5f, 1000.0f);  // Y轴PID
    
    // 初始化电机
    Bujin_Init();
}
StepperMotor motor1 = {
        .address = 0x01,
        .huart = &huart6, // 替换为你的 UART 句柄
        .mode = SPEED_CTRL,
        .subdivision = 0x20, // 32 细分
        .speed_rads_x10 = 0x000A, // 1 rad/s (0x0064 = 100)
    };
StepperMotor motor2 = {
			.address = 0x02,
			.huart = &huart6, // 替换为你的 UART 句柄
			.mode = SPEED_CTRL,
			.subdivision = 0x20, // 32 细分
			.speed_rads_x10 = 0x000A, // 1 rad/s (0x0064 = 100)
	};
uint8_t usart_sent1[11];
uint8_t usart_sent2[11];
	
/**
 ? @brief 速度控制模式指令生成

 ? @param motor 电机结构体指针

 ? @param correction 速度修正量（带方向）

 ? @param output 指令缓冲区

 ? @note 自动转向+动态速度，完全遵循手册3.1节协议

 */
void Bujin_Control(StepperMotor *motor, float correction, uint8_t output[11])
{
    /* 1. 参数安全检查 */
    if(!motor || !output) return;
    
    /* 2. 动态速度计算（手册1.2节限制） */
    float speed = fabsf(correction);
    speed = (speed < 1.0f) ? 1.0f : (speed > 42.0f) ? 42.0f : speed;
    uint16_t speed_10x = (uint16_t)(speed * 0.1);  // 放大10倍

    /* 3. 构建协议帧（手册表3-1） */
    output[0] = 0x7B;                    // 帧头
    output[1] = motor->address;         // 设备地址
    output[2] = SPEED_CTRL;              // 速度控制模式(0x01)
    output[3] = (correction >= 0) ? CW : CCW; // 自动转向
    output[4] = motor->subdivision;      // 细分值
    
    /* 4. 位置数据（速度模式下固定为0） */
    output[5] = 0x00;  // POS_H
    output[6] = 0x00;  // POS_L
    
    /* 5. 速度设置 */
    output[7] = (speed_10x >> 8) & 0xFF; // SPEED_H
    output[8] = speed_10x & 0xFF;       // SPEED_L
    
    /* 6. BCC校验（前9字节异或） */
    output[9] = 0;
    for(uint8_t i=0; i<9; i++) output[9] ^= output[i];
    
    /* 7. 帧尾 */
    output[10] = 0x7D;
}

uint8_t usart_sent1[11] = {0x7b, 0x01, 0x02, 0x01, 0x20, 0x03, 0x84, 0x00, 0x64, 0xBA, 0x7d};//检验能否用
uint8_t usart_receive[11] = {0x7b, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7a, 0x7d};//接收数据

void UART6_SendHex(uint8_t hex)
{
  HAL_UART_Transmit(&huart6, &hex, 1, 100);
}

void USART_SendArray(uint8_t *data, uint8_t length) 
{
    for (uint8_t i = 0; i < length; i++) 
		{
        UART6_SendHex(data[i]);
        // 如果需要延迟（例如防止发送过快），可以加：
        // HAL_Delay(1);
    }
}


void Bujin_Init(void)
{  
    /* 电机1归零指令（绝对角度模式） */
    uint8_t init_motor1[11] = {
        0x7B,                   // 帧头
        0x01,                   // 地址（电机1）
        0x04,                   // 绝对角度模式（0x04）
        0x01,                   // 顺时针方向（CW）
        0x20,                   // 32细分（0x20）
        0x00, 0x00,             // 目标角度0度（0x0000）
        0x00, 0x64,             // 转速10rad/s（0x0064）
        0x3B,                   // BCC校验（计算得出）
        0x7D                    // 帧尾
    };
    
    /* 电机2归零指令（绝对角度模式） */
    uint8_t init_motor2[11] = {
        0x7B,                   // 帧头
        0x02,                   // 地址（电机2）
        0x04,                   // 绝对角度模式（0x04）
        0x01,                   // 顺时针方向（CW）
        0x20,                   // 32细分（0x20）
        0x00, 0x00,             // 目标角度0度（0x0000）
        0x00, 0x64,             // 转速10rad/s（0x0064）
        0x38,                   // BCC校验（计算得出）
        0x7D                    // 帧尾
    };

    /* 发送归零指令 */
    USART_SendArray(init_motor1, sizeof(init_motor1));
    USART_SendArray(init_motor2, sizeof(init_motor2));
    
    /* 等待运动完成（基于运动时间预估） */
    delay_ms(500);              // 0→0度理论上瞬时完成，保留安全余量
}

void ControlSystem_Update(uint16_t target_x, uint16_t target_y) 
{
    // 计算PID输出
    float correction_x = PID_Update(&pid_x, target_x, current_x, 1);
    float correction_y = PID_Update(&pid_y, target_y, current_y, 1);
    
    // 2. 生成控制指令
    Bujin_Control(&motor2, correction_x, usart_sent2); // 电机2控制X轴
    Bujin_Control(&motor1, correction_y, usart_sent1); // 电机1控制Y轴
    
    // 3. 发送指令
    USART_SendArray(usart_sent1, sizeof(usart_sent1));
    USART_SendArray(usart_sent2, sizeof(usart_sent2));
}