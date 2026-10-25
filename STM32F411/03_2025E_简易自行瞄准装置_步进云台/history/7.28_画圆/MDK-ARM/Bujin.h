#ifndef __BUJIN_H
#define __BUJIN_H

// 控制模式定义
typedef enum {
    SPEED_CTRL = 0x01,
    POSITION_CTRL = 0x02,
    TORQUE_CTRL = 0x03,
    ABS_ANGLE_CTRL = 0x04
} ControlMode;

// 转向定义
typedef enum {
    CCW = 0x00, // 逆时针
    CW = 0x01   // 顺时针
} RotationDir;



// 电机配置结构体
typedef struct {
    uint8_t address;        // 电机地址（默认 0x01）
    UART_HandleTypeDef *huart; // UART 句柄
    ControlMode mode;       // 控制模式
    uint8_t subdivision;    // 细分值（如 0x20=32 细分）
    uint16_t speed_rads_x10; // 转速（放大 10 倍，如 0x0064=10 rad/s）
} StepperMotor;

 
void UART6_SendHex(uint8_t hex);
void USART_SendArray(uint8_t *data, uint8_t length);
void Bujin_Control(StepperMotor *motor, float correction, uint8_t output[11]);
void Bujin_Init(void);
void ControlSystem_Init(void);
void ControlSystem_Update(uint16_t target_x, uint16_t target_y);

#endif // !__BUJIN_H
