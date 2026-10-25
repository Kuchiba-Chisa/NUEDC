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
    // 初始化PID参数 (需要根据实际系统调整)
    PID_Init(&pid_x, 0.2f, 0.0f, 0.8f, 1000.0f);  // X轴PID
    PID_Init(&pid_y, 0.2f, 0.0f, 0.8f, 1000.0f);  // Y轴PID
    
    // 初始化电机
    Bujin_Init();
}
StepperMotor motor1 = {
        .address = 0x01,
        .huart = &huart6, // 替换为你的 UART 句柄
        .mode = ABS_ANGLE_CTRL,
        .subdivision = 0x20, // 32 细分
        .speed_rads_x10 = 0x000A, // 1 rad/s (0x0064 = 100)
    };
StepperMotor motor2 = {
			.address = 0x02,
			.huart = &huart6, // 替换为你的 UART 句柄
			.mode = ABS_ANGLE_CTRL,
			.subdivision = 0x20, // 32 细分
			.speed_rads_x10 = 0x000A, // 1 rad/s (0x0064 = 100)
	};
uint8_t usart_sent1[11];
uint8_t usart_sent2[11];
	
void BuildPositionControlFrame(StepperMotor *motor,uint16_t angle_x10,RotationDir dir,uint8_t output[11]) //构建数组
{
    // 1. 固定帧头、控制 ID
    output[0] = 0x7B; // 帧头
    output[1] = motor->address; // 控制 ID（默认 0x01）

    // 2. 控制模式（位置控制）
    output[2] = motor->mode;

    // 3. 转向
    output[3] = dir;

    // 4. 细分值
    output[4] = motor->subdivision;

    // 5-6. 目标角度（高 8 位 + 低 8 位）
    output[5] = (angle_x10 >> 8) & 0xFF; // 高字节
    output[6] = angle_x10 & 0xFF;        // 低字节

    // 7-8. 转速（高 8 位 + 低 8 位）
    output[7] = (motor->speed_rads_x10 >> 8) & 0xFF; // 高字节
    output[8] = motor->speed_rads_x10 & 0xFF;        // 低字节

    // 9. BCC 校验（前 9 字节异或）
    output[9] = 0;
    for (uint8_t i = 0; i < 9; i++) {
        output[9] ^= output[i];
    }

    // 10. 帧尾
    output[10] = 0x7D;
//		USART_SendArray(output,sizeof(output));
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
	BuildPositionControlFrame(&motor1, 0, CW, usart_sent1); 
	BuildPositionControlFrame(&motor2, 0, CW, usart_sent2); 
	USART_SendArray(usart_sent1,sizeof(usart_sent1));
	delay_ms(10);
	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
}

void Speed_bujin(void)
{
	BuildPositionControlFrame(&motor1, 60, CW, usart_sent1); 
	BuildPositionControlFrame(&motor2, 60, CW, usart_sent2); 
	USART_SendArray(usart_sent1,sizeof(usart_sent1));
//	delay_ms(10);
	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
}

void ControlSystem_Update(uint16_t target_x, uint16_t target_y) 
{
//    static uint32_t last_time = 0;
//    uint32_t current_time = HAL_GetTick();
//    float dt = (current_time - last_time) / 1000.0f; // 转换为秒
//    if (dt <= 0) dt = 0.01f; // 最小时间步长
//    last_time = current_time;
//    
  
    // 计算PID输出
    float correction_x = PID_Update(&pid_x, target_x, current_x, 1);
    float correction_y = PID_Update(&pid_y, target_y, current_y, 1);
    
    // 转换为电机控制量 (需要根据系统特性调整)
    uint16_t motor2_correction = (uint16_t)(correction_x*3);
    uint16_t motor1_correction = (uint16_t)(correction_y*3);
//    RotationDir dir1 = correction_x > 0 ? CW : CCW;
//    RotationDir dir2 = correction_y > 0 ? CW : CCW;
    
    // 应用修正
    BuildPositionControlFrame(&motor1, angle_x+motor1_correction, CW, usart_sent1); 
    BuildPositionControlFrame(&motor2, angle_y+motor2_correction, CW, usart_sent2); 
    
    USART_SendArray(usart_sent1, sizeof(usart_sent1));
//    delay_ms(10);
    USART_SendArray(usart_sent2, sizeof(usart_sent2));
}


// 将像素坐标转换为角度(弧度*10)
void ConvertCoordinatesToAngles(float distance_X_meters, uint16_t pixel_x, uint16_t pixel_y, 
                               uint16_t* angle_motor1_x10, uint16_t* angle_motor2_x10) {
    // 1. 将像素坐标转换为实际距离(米)
    float pos_x_mm = (float)pixel_x / PIXELS_PER_MM_X;
    float pos_y_mm = (float)pixel_y / PIXELS_PER_MM_Y;
    
    // 转换为相对于中心的坐标 (假设激光笔正对A4纸中心)
    float center_x_mm = A4_WIDTH_MM / 2.0f;
    float center_y_mm = A4_HEIGHT_MM / 2.0f;
    
    float offset_x_m = (pos_x_mm - center_x_mm) / 1000.0f; // 转换为米
    float offset_y_m = (center_y_mm - pos_y_mm) / 1000.0f; // Y轴方向相反
    
    // 2. 计算角度 (使用atan2函数)
    float angle_x_rad = atan2f(offset_x_m, distance_X_meters); // 水平角度
    float angle_y_rad = atan2f(offset_y_m, distance_X_meters); // 垂直角度
    
    // 3. 转换为弧度*10 (保持与你现有系统一致)
    *angle_motor1_x10 = (uint16_t)(angle_x_rad * 10.0f * 180.0f / M_PI); // 电机1控制X轴
    *angle_motor2_x10 = (uint16_t)(angle_y_rad * 10.0f * 180.0f / M_PI); // 电机2控制Y轴
}
void test(void)
{
	BuildPositionControlFrame(&motor1, 100, CW, usart_sent1); 
	USART_SendArray(usart_sent1,sizeof(usart_sent1));
	BuildPositionControlFrame(&motor2, 100, CW, usart_sent2); 
	USART_SendArray(usart_sent2,sizeof(usart_sent2));
}

void sanjiao(void)
{
	BuildPositionControlFrame(&motor2, 3400, CW, usart_sent2); 
	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
	delay_ms(2000);
	BuildPositionControlFrame(&motor2, 3500, CW, usart_sent2); 
	BuildPositionControlFrame(&motor1, 100, CW, usart_sent1); 
	USART_SendArray(usart_sent1,sizeof(usart_sent1));
//	delay_ms(10);
	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
	delay_ms(2000);	
	BuildPositionControlFrame(&motor1, 0, CW, usart_sent1); 
	BuildPositionControlFrame(&motor2, 3600, CW, usart_sent2); 
	USART_SendArray(usart_sent1,sizeof(usart_sent1));
//	delay_ms(10);
	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
}
