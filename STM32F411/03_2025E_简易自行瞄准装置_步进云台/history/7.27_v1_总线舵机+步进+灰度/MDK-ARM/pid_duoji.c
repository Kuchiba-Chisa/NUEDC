#include "main.h"
#include "pid_duoji.h"
#include "duoji.h"
#include "math.h"
#include "OLED.h"



// PID控制器参数
float pid_kp_x = 0.17f;   // X轴比例系数
float pid_ki_x = 0.05f;  // X轴积分系数
float pid_kd_x = 0.35f;   // X轴微分系数

float pid_kp_y = 0.17f;   // Y轴比例系数
float pid_ki_y = 0.05f;  // Y轴积分系数
float pid_kd_y = 0.35f;   // Y轴微分系数

// PID变量
float error_x = 0.0f, last_error_x = 0.0f, duoji_integral_x = 0.0f;
float error_y = 0.0f, last_error_y = 0.0f, duoji_integral_y = 0.0f;

// 舵机ID定义
#define SERVO_X_ID 0     // X轴舵机ID
#define SERVO_Y_ID 1     // Y轴舵机ID

// 舵机参数
#define SERVO_MIN_PULSE 500    // 最小脉冲宽度(us)
#define SERVO_MAX_PULSE 2500   // 最大脉冲宽度(us)
#define SERVO_MID_PULSE 1500   // 中间位置脉冲宽度(us)

// 目标位置和当前位置
float target_x = 0.0f, target_y = 0.0f;
extern int current_x,current_y;
// 控制周期(ms)
#define CONTROL_PERIOD 20

// 限制函数
float constrain_float(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// PID计算函数 - X轴
float compute_pid_x(int target, int current) {
    float output;
    
    // 计算误差
    error_x = target - current;
    
    // 积分项
    duoji_integral_x += error_x;
//    duoji_integral_x = constrain_float(duoji_integral_x, -500.0f, 500.0f); // 抗积分饱和
    
    // 微分项
    float derivative = error_x - last_error_x;
    
    // PID输出
    output = pid_kp_x * error_x + pid_ki_x * duoji_integral_x + pid_kd_x * derivative;
    
    // 保存当前误差
    last_error_x = error_x;
    
    return output;
}

// PID计算函数 - Y轴
float compute_pid_y(int target, int current) {
    float output;
    
    // 计算误差
    error_y = target - current;
    
    // 积分项
    duoji_integral_y += error_y;
//    duoji_integral_y = constrain_float(duoji_integral_y, -500.0f, 500.0f); // 抗积分饱和
    
    // 微分项
    float derivative = error_y - last_error_y;
    
    // PID输出
    output = pid_kp_y * error_y + pid_ki_y * duoji_integral_y + pid_kd_y * derivative;
    
    // 保存当前误差
    last_error_y = error_y;
    
    return output;
}

// 更新摄像头反馈的当前位置

// 设置目标位置
void set_target_position(int x, int y) {
    target_x = x;
    target_y = y;
}

// 控制循环函数，需要在定时中断或主循环中定期调用
void control_loop(void) {
//    static uint32_t last_time = 0;
//    uint32_t current_time = HAL_GetTick();
//    
//    // 控制周期检查
//    if (current_time - last_time < CONTROL_PERIOD) {
//        return;
//    }
//    last_time = current_time;
    
    // 计算PID输出
    float output_x = compute_pid_x(target_x, current_x);
    float output_y = compute_pid_y(target_y, current_y);
    
    // 转换为舵机脉冲宽度
    int pulse_x = 1550 + (int)output_x;
    int pulse_y = 1750 - (int)output_y;
    
    // 限制脉冲宽度在有效范围内
    pulse_x = (int)constrain_float(pulse_x, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    pulse_y = (int)constrain_float(pulse_y, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    
    // 发送舵机控制命令
    send_servo_command(SERVO_X_ID, pulse_x, CONTROL_PERIOD);
    send_servo_command(SERVO_Y_ID, pulse_y, CONTROL_PERIOD);
}

// 初始化PID控制器
void pid_init(void) {
    error_x = 0.0f;
    last_error_x = 0.0f;
    duoji_integral_x = 0.0f;
    
    error_y = 0.0f;
    last_error_y = 0.0f;
    duoji_integral_y = 0.0f;
    
    target_x = 0.0f;
    target_y = 0.0f;
    current_x = 0.0f;
    current_y = 0.0f;
}

// 设置PID参数
void set_pid_params(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y) {
    pid_kp_x = kp_x;
    pid_ki_x = ki_x;
    pid_kd_x = kd_x;
    
    pid_kp_y = kp_y;
    pid_ki_y = ki_y;
    pid_kd_y = kd_y;
}

typedef struct {
    int x;
    int y;
} Point;

// 矩形四个角点（假设摄像头分辨率480x320）
static const Point corners[] = {
    {320, 160},   // 顶点（上）
    {400, 240},   // 右顶点
    {320, 320},   // 底点（下）
    {240, 240}    // 左顶点
};

// 预计算的路径点数组
static Point path_points[TOTAL_PATH_POINTS];

// 初始化路径点（在主函数初始化时调用一次）
void init_path_points() 
{
    int index = 0;
    
    // 插值生成路径点
    for (int edge = 0; edge < 4; edge++) {
        Point start = corners[edge];
        Point end = corners[(edge + 1) % 4];
        
        for (int i = 0; i < POINTS_PER_EDGE - 1; i++) {
            float t = (float)i / (POINTS_PER_EDGE - 1);
            path_points[index].x = start.x + t * (end.x - start.x);
            path_points[index].y = start.y + t * (end.y - start.y);
            index++;
        }
    }
}
static uint16_t current_point_index = 0;
static uint32_t last_update_time = 0;

// 在1ms定时器中断中调用
void update_path_in_timer_irq() {
    static uint32_t tick = 0;
    tick++;
    
    // 按固定间隔更新路径点
    if (tick - last_update_time >= PATH_UPDATE_INTERVAL) {
        last_update_time = tick;
        
        // 设置当前目标点
        Point target = path_points[current_point_index];
        set_target_position(target.x, target.y);
        
        // 循环遍历路径点
        current_point_index = (current_point_index + 1) % TOTAL_PATH_POINTS;
    }
}