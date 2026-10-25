#include "ti_msp_dl_config.h"
#include "hardware_iic.h"
#include "pwm.h"
#include "Imu.h"
#include "debug.h"
#include "oled.h"

#define NUM_SENSORS 8       // 灰度传感器数量
//灰度传感器pid
#define G_KP 90              // PID比例系数
#define G_KI 0                // 积分系数 (可选)
#define G_KD 220					// 微分系数
#define motor_max 8000
#define motor_min 2000

//阿克曼结构pid
float pid_Kp = 1.5f;       // 比例系数
float pid_Ki = 0.01f;      // 积分系数
float pid_Kd = 0.2f;       // 微分系数
float pid_integral = 0.0f; // 积分累积值
float pid_prev_error = 0.0f; // 上一次误差
const float OUTPUT_LIMIT = 90.0f; // 输出限幅值(±30度)
const float INTEGRAL_LIMIT = 10.0f; // 积分限幅

//陀螺仪pid
float A_Kp = 0.8;
float	A_Ki = 0; 
float A_Kd = 2.5;

//速度环pid
#define SPEED_KP 1.7f    // 比例系数
#define SPEED_KI 0.01f   // 积分系数
#define SPEED_KD 0.55f   // 微分系数
#define SPEED_INTEGRAL_LIMIT 100.0f  // 积分限幅
#define SPEED_OUTPUT_LIMIT 500.0f    // 输出限幅
static float speed_integral = 0.0f;
static float speed_prev_error = 0.0f;

// 定义全局变量
float G_error = 0;           // 当前误差
float G_integral = 0;        // 积分项
float G_derivative = 0;      // 微分项
float G_last_error = 0;     // 上一次误差
uint8_t sensor_values[NUM_SENSORS]; // 存储灰度传感器值
int BASE_SPEED = 3000; // 基础速度值，根据实际情况调整
float target_angle = 0; // 设定目标角度为零度（直立状态）
float current_angle = 0;
float offset = 0;
float offset_Yaw = 0;
int motor_right=0;
int motor_left=0;
unsigned char Digtal;
unsigned char rx_buff[256]={0};


void ReadSensors(uint8_t digitBits[8]) {
    Digtal=IIC_Get_Digtal();
	for(int i = 0; i < 8; i++) {
        digitBits[i] = (Digtal >> i) & 0x01;
    }
	// sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);//	GW_analog_Read(values);
	// OLED_ShowString(0, 32, rx_buff, 8,1);
}

void ReadGyroscopeData(void)
{
	Imu_ReceiveData();
}
///**
// * @brief 计算目标位置偏移量
// */
//int CalculateOffset(uint8_t* sensors) {
//    int sum = 0;
//    float weighted_sum = 0;

//    for(int i=0;i<NUM_SENSORS;i++) {
//        if(sensors[i] == 0) {                  // 假设黑线对应低电平(具体阈值需调试)
//            sum += 1 << i;                       // 权重计算
//            weighted_sum += i * (sensors[i]);   // 加权求和
//        }
//    }

//    return (weighted_sum / sum) - (NUM_SENSORS/2);
//}
int turn =0;
int CalculateOffset(uint8_t* sensors) {
//    DEBUG_info("Grey",":%d,%d,%d,%d,%d,%d,%d,%d",(int)sensors[0],(int)sensors[1],(int)sensors[2],(int)sensors[3],(int)sensors[4],(int)sensors[5],(int)sensors[6],(int)sensors[7]);
		offset = 0;
		if(sensors[3] == 0 && sensors[4] == 0) offset = 0;
		if(sensors[2] == 1 ) offset += 3;	//说明略有左偏
		if(sensors[5] == 1 ) offset -= 3;	//说明略有右偏
		if(sensors[1] == 1 ) offset += 6;	//说明较为左偏
		if(sensors[6] == 1 ) offset -= 6;	//说明较为右偏
		if(sensors[0] == 1 ) offset += 9;	//说明非常左偏
		if(sensors[7] == 1 ) offset -= 9;	//说明非常右偏
		if(sensors[7] == 1  && sensors[0] == 0 ) turn = 1;	//说明非常右偏
		if(sensors[7] == 0  && sensors[0] == 1 ) turn = 2;	//说明非常左偏
    return offset;
}

int CalculateOffset_angle(int* sensors2) {
//    DEBUG_info("Grey",":%d,%d,%d,%d,%d,%d,%d,%d",(int)sensors[0],(int)sensors[1],(int)sensors[2],(int)sensors[3],(int)sensors[4],(int)sensors[5],(int)sensors[6],(int)sensors[7]);
		offset = 0;
		if(sensors2[2] == 0 && sensors2[3] == 0) offset = 0;
		if(sensors2[1] == 1 ) offset += 5;	//说明略有左偏
		if(sensors2[4] == 1 ) offset -= 5;	//说明略有右偏
		if(sensors2[0] == 1 ) offset += 10;	//说明较为左偏
		if(sensors2[6] == 1 ) offset -= 10;	//说明较为右偏
    return offset;
}

//int CalculateOffset_analog(uint8_t* sensors) {
//    DEBUG_info("Grey",":%d,%d,%d,%d,%d,%d,%d,%d", (int)sensors[0], (int)sensors[1], (int)sensors[2], (int)sensors[3], (int)sensors[4], (int)sensors[5], (int)sensors[6], (int)sensors[7]);
//    float offset = 0.0f;
//    const int threshold = 30; // 根据实际情况调整阈值
//    
//    // 如果中间传感器均低于阈值，视为未检测到线，使用两侧传感器计算较大偏移
//    if (sensors[3] < threshold && sensors[4] < threshold) {
//        // 计算各传感器的加权贡献
//        offset += sensors[0] * (200.0f / 255.0f); // 非常左偏
//        offset += sensors[1] * (50.0f / 255.0f);  // 较为左偏
//        offset += sensors[2] * (10.0f / 255.0f);  // 略有左偏
//        offset -= sensors[5] * (10.0f / 255.0f);  // 略有右偏
//        offset -= sensors[6] * (50.0f / 255.0f);  // 较为右偏
//        offset -= sensors[7] * (200.0f / 255.0f); // 非常右偏
//    } else {
//        // 使用所有传感器的加权平均计算连续偏移
//        float positions[8] = {3.5f, 2.5f, 1.5f, 0.5f, -0.5f, -1.5f, -2.5f, -3.5f};
//        float sum = 0.0f, total = 0.0f;
//        for (int i = 0; i < 8; i++) {
//            sum += sensors[i] * positions[i];
//            total += sensors[i];
//        }
//        if (total > 0) {
//            offset = (sum / total) * 57.14f; // 缩放至约±200范围
//        }
//    }
//    
//    return (int)offset;
//}

int IMU_flag = 0;
float Yaw_Init;
int CalculateOffset_IMU(int Yaw_Init) {
//		IMU_flag++;
//		if(IMU_flag == 1) 
//		{
//			Yaw_Init = Yaw+yaw;
//			DEBUG_info("Yaw_Init",":%f",Yaw_Init);
//		}
//		else DEBUG_info("Yaw",":%f",Yaw);
//		
		offset_Yaw = Yaw - Yaw_Init;
//		DEBUG_info("Yaw_Init",":%d",Yaw_Init);
//		DEBUG_info("offset_Yaw",":%f",offset_Yaw);
//		
    return offset_Yaw;
}

/**
 * @brief PID控制器
 */
float ComputePID(float current_offset) {
    G_error = current_offset;
    G_integral += G_error;
    G_derivative = G_error - G_last_error;
    
    float output = G_KP * G_error + G_KI * G_integral + G_KD * G_derivative;
    G_last_error = G_error;
    return output;
}

float angle_PID_Controller(float offset_Yaw, float dt) {
    static float A_prev_error = 0;
    static float A_integral = 0;
    float A_error = offset_Yaw;
    A_integral += A_error * dt;
    float A_derivative = (A_error - A_prev_error) / dt;
    A_prev_error = A_error;
		
    // PID 输出信号
    float output = A_Kp * A_error + A_Ki * A_integral + A_Kd * A_derivative;
//		DEBUG_info("P",":%f",A_Kp * A_error);
//	DEBUG_info("I",":%f",A_Ki * A_integral);
//	DEBUG_info("D",":%f",A_Kd * A_derivative);
//		DEBUG_info("output_Yaw",":%f",output);
    return output*0.5;
}

float SpeedPID_Controller(int target_speed, float current_speed, float dt) {
    // 计算误差
    float error = target_speed - current_speed;
    
    // 积分项计算(带限幅)
    speed_integral += error * dt;
    // if (speed_integral > SPEED_INTEGRAL_LIMIT) speed_integral = SPEED_INTEGRAL_LIMIT;
    // if (speed_integral < -SPEED_INTEGRAL_LIMIT) speed_integral = -SPEED_INTEGRAL_LIMIT;
    
    // 微分项计算
    float derivative = (error - speed_prev_error) / dt;
    speed_prev_error = error;
    
    // PID输出计算
    float output = SPEED_KP * error + SPEED_KI * speed_integral + SPEED_KD * derivative;
    
    // 输出限幅
    if (output > SPEED_OUTPUT_LIMIT) output = SPEED_OUTPUT_LIMIT;
    if (output < -SPEED_OUTPUT_LIMIT) output = -SPEED_OUTPUT_LIMIT;
    
    return output;
}

// 重置PID状态
void reset_pid() {
    pid_integral = 0.0f;
    pid_prev_error = 0.0f;
}

// 计算PID输出
float compute_pid(float setpoint, float input, float dt) {
    // 计算误差
    float error = setpoint - input;
    
    // 积分项计算(带限幅)
    pid_integral += error * dt;
    if (pid_integral > INTEGRAL_LIMIT) pid_integral = INTEGRAL_LIMIT;
    if (pid_integral < -INTEGRAL_LIMIT) pid_integral = -INTEGRAL_LIMIT;
    
    // 微分项计算
    float derivative = (error - pid_prev_error) / dt;
    pid_prev_error = error;
    
    // PID输出计算
    float output = pid_Kp * error + pid_Ki * pid_integral + pid_Kd * derivative;
    
    // 输出限幅
    if (output > OUTPUT_LIMIT) output = OUTPUT_LIMIT;
    if (output < -OUTPUT_LIMIT) output = -OUTPUT_LIMIT;
    
    return output;
}

void SpeedControl(int target_left, int target_right, 
                 float current_left, float current_right, float dt) {
    // 计算左右轮速度PID输出
    float left_correction = SpeedPID_Controller(target_left, current_left, dt);
    float right_correction = SpeedPID_Controller(target_right, current_right, dt);
    
    // 应用修正值到电机
    motor_left += (int)left_correction;
    motor_right += (int)right_correction;
    
    // 电机输出限幅
    if(motor_left > motor_max) motor_left = motor_max;
    if(motor_left < motor_min) motor_left = motor_min;
    if(motor_right > motor_max) motor_right = motor_max;
    if(motor_right < motor_min) motor_right = motor_min;
    
    Set_PWM(motor_left, motor_right);
}

/**
 * @brief 调节电机速度
 */
//陀螺仪pid和灰度传感器pid求得误差后，都用这个函数调整电机
void AdjustMotorSpeed(float correction_value) {
	motor_left = BASE_SPEED + 2*(int)correction_value;
    motor_right = BASE_SPEED - 2*(int)correction_value;
		if(motor_left>motor_max)
		{
			motor_left=motor_max;
		}
		if(motor_left<motor_min)
		{
			motor_left=motor_min;
		}
		if(motor_right>motor_max)
		{
			motor_right=motor_max;
		}
		if(motor_right<motor_min)
		{
			motor_right=motor_min;
		}
    Set_PWM(motor_left, motor_right);
//		DEBUG_info("motor_left",":%d",motor_left);
//		DEBUG_info("motor_right",":%d",motor_right);
}


