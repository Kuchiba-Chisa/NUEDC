#include "yuntai.h"
#include <math.h>
#include "Stepper_Motor.h"

#define Servo_MAX_A 2300    //最大位置
#define Servo_MAX_B 2300

#define Servo_MIN_A 700     //最小位置
#define Servo_MIN_B 700

extern int32_t Servo_Kp = 0.5;            // 舵机比例系数
extern int32_t Servo_Ki = 0.8;            // 舵机积分系数
extern int32_t Servo_Kd = 0;            // 舵机微分系数


// 实时位置  
extern uint16_t pwm_A = 1503;
extern uint16_t pwm_B = 1471;

// 上一次控过的位置
uint16_t pwm_A_last = 1503;
uint16_t pwm_B_last = 1471;


extern int8_t Position_error[2];

#define MICROSTEPS         2     // 微步细分
#define STEPS_PER_DEGREE_X 40  // X轴每度步数 (360度/步距角*微步)
#define STEPS_PER_DEGREE_Y 40  // Y轴每度步数

float current_angle_x = 0.0f;  // X轴当前角度
float current_angle_y = 0.0f;  // Y轴当前角度
uint32_t steps_per_rev_x = 1800;  // X轴每转步数
uint32_t steps_per_rev_y = 1800; // Y轴每转步数



/* 计算X轴PID输出 */


/* 计算Y轴PID输出 */




//void Gimbal_Control_Loop(void) {
//    // 计算当前位置与目标的距离
//    float error_x = fabsf(target_pos_x - current_pos_x);
//    float error_y = fabsf(target_pos_y - current_pos_y);
//    
//    // 如果目标未达到或电机仍在运行，则更新位置
//    if (motor_running || error_x > 0.1f || error_y > 0.1f) {
//        Gimbal_Update_Position();
//        motor_running = 1;
//    }
//}
void Gimbal_Move_To_Angle(float target_x, float target_y) 
{
    // 计算X轴需要移动的步数和方向
    float delta_x = target_x - current_angle_x;
    uint8_t dir_x = (delta_x >= 0) ? 1 : 0;
    uint32_t steps_x = (uint32_t)(fabsf(delta_x) * steps_per_rev_x / 360.0f);
    
    // 计算Y轴需要移动的步数和方向
    float delta_y = target_y - current_angle_y;
    uint8_t dir_y = (delta_y >= 0) ? 1 : 0;
    uint32_t steps_y = (uint32_t)(fabsf(delta_y) * steps_per_rev_y / 360.0f);
    
    // 设置固定转速（RPM）
    uint32_t speed_rpm = 60;  // 可调整的固定速度
    
    // 启动电机
    if(steps_x > 0) {
        
    }
    if(steps_y > 0) {
        
    }
    
    // 更新当前角度（实际应在中断中根据步数更新）
    current_angle_x = target_x;
    current_angle_y = target_y;
}
//void Yuntai_Init()
//{
//    SERVO_PWMA_Set(pwm_A);
//    SERVO_PWMB_Set(pwm_B);
//}

//云台控制
void Yuntai_Control(void)
{
//    Stepper_Motor_Move_A(1,pwm_A);
//    Stepper_Motor_Move_B(1,pwm_B);
}

//
//void Yuntai_PID()
//{
//    static int16_t L_x_err=0,L_L_x_err=0,L_y_err=0,L_L_y_err=0;

//    int8_t x_err = Position_error[0];
//    int8_t y_err = Position_error[1];
//    printf("x_err:%d,y_err:%d\r\n",x_err,y_err);

//    // 死区限制，防止抖动
//    if (x_err > -8 && x_err < 8) x_err = 0;
//    if (y_err > -8 && y_err < 8) y_err = 0;


//    // 进行PID计算
//    // -0.2 -0.8 -0.1
//    // 0.2 0.8 0.1
//    pwm_A += -0 * (x_err-L_x_err)+ (-0.7)*x_err+ (40.15) * (x_err - 2 * L_x_err + L_L_x_err);
//    pwm_B += 0.1 * (y_err-L_y_err) + (0.7)*y_err + 0.15* (y_err - 2 * L_y_err + L_L_y_err);
//    
//    //保存误差值
//    L_x_err = x_err;
//    L_L_x_err = L_x_err;
//    
//    L_y_err = y_err;
//    L_L_x_err = L_y_err;

//    //输出限幅
//    if (pwm_A > Servo_MAX_A) pwm_A = Servo_MAX_A;
//    if (pwm_A < Servo_MIN_A) pwm_A = Servo_MIN_A;
//    if (pwm_B > Servo_MAX_B) pwm_B = Servo_MAX_B;
//    if (pwm_B < Servo_MIN_B) pwm_B = Servo_MIN_B;

//    // 进行控制
//    SERVO_PWMA_Set(pwm_A);
//    SERVO_PWMB_Set(pwm_B);
//}

void Yuntai_PID(int16_t x_err, int16_t y_err)
{
    static int16_t L_x_err = 0, L_L_x_err = 0, L_y_err = 0, L_L_y_err = 0;
    static float filtered_x_err = 0, filtered_y_err = 0; // 滤波后的误差
    const float alpha = 0.3; // 滤波系数，可调整

//    printf("x_err:%d, y_err:%d\r\n", x_err, y_err);
//    printf("111:pwm_A:%d, pwm_B:%d\r\n", pwm_A, pwm_B);

//    // 死区限制，防止抖动
//    if (x_err > -1 && x_err < 1) x_err = 0;
//    if (y_err > -1 && y_err < 1) y_err = 0;

//    // 低通滤波
//    filtered_x_err = alpha * x_err + (1 - alpha) * filtered_x_err;
//    filtered_y_err = alpha * y_err + (1 - alpha) * filtered_y_err;

//    // 使用滤波后的误差进行 PID 计算
//    pwm_A += 0.01 * (filtered_x_err - L_x_err) + (-0.3) * filtered_x_err + (-0.3) * (filtered_x_err - 2 * L_x_err + L_L_x_err);
//    pwm_B += 0.01 * (filtered_y_err - L_y_err) + (-0.3) * filtered_y_err + (-0.3) * (filtered_y_err - 2 * L_y_err + L_L_y_err);

	// 进行PID计算
	  pwm_A += 0.005 * (x_err - L_x_err) + (-0.3) * x_err + (-0.3) * (x_err - 2 * L_x_err + L_L_x_err);
    pwm_B += 0.005 * (y_err - L_y_err) + (-0.3) * y_err + (-0.3) * (y_err - 2 * L_y_err + L_L_y_err);
		
//    printf("222:pwm_A:%d, pwm_B:%d\r\n", pwm_A, pwm_B);

    // 保存误差值
    L_x_err = filtered_x_err;
    L_L_x_err = L_x_err;
    L_y_err = filtered_y_err;
    L_L_y_err = L_y_err;

    // 输出限幅
    if (pwm_A > Servo_MAX_A) pwm_A = Servo_MAX_A;
    if (pwm_A < Servo_MIN_A) pwm_A = Servo_MIN_A;
    if (pwm_B > Servo_MAX_B) pwm_B = Servo_MAX_B;
    if (pwm_B < Servo_MIN_B) pwm_B = Servo_MIN_B;

    // 进行控制
//    Stepper_Motor_Move_A(1,pwm_A);
//    Stepper_Motor_Move_B(1,pwm_B);
}


//云台A(左右)丝滑移动,两个参数分别为目标位置和移动延时
void Yuntaiz_A_Move(uint16_t pwm_d,int16_t Flow_Coefficient)
{

    // 计算每次移动的步长
    int16_t step_a = (pwm_d > pwm_A_last) ? 1 : -1;
    while (pwm_A_last != pwm_d)
    {
        pwm_A_last += step_a;
//				Stepper_Motor_Move_A(1,pwm_A_last);
        HAL_Delay(Flow_Coefficient);
    }
		
}


// 云台B(上下)丝滑移动，两个参数分别为目标位置和移动延时
void Yuntaiz_B_Move(uint16_t pwm_d, int16_t Flow_Coefficient)
{

    // 计算每次移动的步长
    int16_t step_b = (pwm_d > pwm_B_last) ? 1 : -1;
    while (pwm_B_last != pwm_d)
    {
        pwm_B_last += step_b;
//				Stepper_Motor_Move_B(1,pwm_B_last);
        HAL_Delay(Flow_Coefficient);
    }
}


// 走斜线(不考虑斜率)
// 云台B(上下)丝滑移动，三个参数分别为目标位置和移动延时
void Yuntaiz_AB_Move(uint16_t pwm_a,uint16_t pwm_b, int16_t Flow_Coefficient)
{

    // 计算每次移动的步长
    int16_t step_a = (pwm_a > pwm_A_last) ? 1 : -1;
    int16_t step_b = (pwm_b > pwm_B_last) ? 1 : -1;
//    DEBUG_info ("PWM","pwma:%d,pwm_b:%d",(int)pwm_a,(int)pwm_b);
    // 按照步长移动云台A和云台B，直到达到目标位置
    while (pwm_A_last != pwm_a || pwm_B_last != pwm_b)
    {
        if (pwm_A_last != pwm_a)
        {
            pwm_A_last += step_a;
//						Stepper_Motor_Move_A(1,pwm_A_last);
        }
        if (pwm_B_last != pwm_b)
        {
            pwm_B_last += step_b;
//						Stepper_Motor_Move_B(1,pwm_B_last);
        }
        HAL_Delay(Flow_Coefficient);
    }
}




// A4靶纸开环循迹(水平摆放)
void Yuntai_A4_Track(uint16_t pwm_a,uint16_t pwm_b, int16_t Flow_Coefficient)
{
    int16_t left_error = 170; //左右偏
    int16_t up_error = 118; //上下偏

    //坐标换算

    //移动到起始点
    Yuntaiz_AB_Move(pwm_a,pwm_b,Flow_Coefficient);
    //移动到第一个点
    Yuntaiz_AB_Move(pwm_a-left_error,pwm_b,Flow_Coefficient);
    //移动到第二个点
    Yuntaiz_AB_Move(pwm_a-left_error,pwm_b+up_error,Flow_Coefficient);
    //移动到第三个点
    Yuntaiz_AB_Move(pwm_a,pwm_b+up_error,Flow_Coefficient);
    //移动到第四个点
    Yuntaiz_AB_Move(pwm_a,pwm_b,Flow_Coefficient);
}

int abs(int num) {
    return (num < 0) ? -num : num;
}



//// 云台B(上下)丝滑移动，三个参数分别为目标位置和移动延时（考虑斜率）
//void Yuntaiz_AB_Move_2(uint16_t pwm_a, uint16_t pwm_b, int16_t Flow_Coefficient)
//{

//    // 计算云台A和云台B的位置差
//    int16_t diff_a = abs(pwm_a - pwm_A_last);
//    int16_t diff_b = abs(pwm_b - pwm_B_last);

//    // 计算移动次数
//    int16_t num_steps = (diff_a > diff_b)?diff_a:diff_b;
//    

//    // 计算云台A和云台B每次移动的增量
//    float increment_a = (float)(pwm_a - pwm_A_last) / num_steps;
//    float increment_b = (float)(pwm_b - pwm_B_last) / num_steps;
//    
//    DEBUG_info ("yuntai","increment_a:%f,increment_b:%f",increment_a,increment_b);

//    

//    // 插值移动云台A和云台B，直到达到目标位置
//    for (int16_t i = 0; i <= num_steps; i++)
//    {
//        // 计算当前位置
//        uint16_t current_pwm_a = round(pwm_A_last + i * increment_a);
//        uint16_t current_pwm_b = round(pwm_B_last + i * increment_b);

//        // 设置云台A和云台B的位置
//        SERVO_PWMA_Set(current_pwm_a);
//        SERVO_PWMB_Set(current_pwm_b);

//        // 延时一段时间
//        HAL_Delay(Flow_Coefficient);
//    }

//    // 更新云台A和云台B的最新位置
//    pwm_A_last = pwm_a;
//    pwm_B_last = pwm_b;
//}


// 云台B(上下)丝滑移动，三个参数分别为目标位置和移动延时（考虑斜率）
void Yuntaiz_AB_Move_2(uint16_t pwm_a, uint16_t pwm_b, int16_t Flow_Coefficient)
{
    // 计算云台A和云台B的位置差
    int16_t diff_a = abs(pwm_a - pwm_A_last);
    int16_t diff_b = abs(pwm_b - pwm_B_last);

    // 计算移动次数
    int16_t num_steps  = (diff_a > diff_b) ? diff_a : diff_b;
    num_steps = num_steps* 3;

    if (num_steps == 0)
        return;  // 无需移动，直接返回

    // 计算云台A和云台B每次移动的增量
    float increment_a = (float)(pwm_a - pwm_A_last) / num_steps;
    float increment_b = (float)(pwm_b - pwm_B_last) / num_steps;

//    DEBUG_info("yuntai", "increment_a:%f,increment_b:%f", increment_a, increment_b);

    // 计算延时时间
    uint32_t delay_time = Flow_Coefficient / num_steps;

    // 插值移动云台A和云台B，直到达到目标位置
    for (int16_t i = 0; i <= num_steps; i++)
    {
        // 计算当前位置
        uint16_t current_pwm_a = round(pwm_A_last + i * increment_a);
        uint16_t current_pwm_b = round(pwm_B_last + i * increment_b);

        // 设置云台A和云台B的位置
//        SERVO_PWMA_Set(current_pwm_a);
//        SERVO_PWMB_Set(current_pwm_b);

        // 延时一段时间
        HAL_Delay(delay_time);
    }

    // 更新云台A和云台B的最新位置
    pwm_A_last = pwm_a;
    pwm_B_last = pwm_b;
}





void Yuntaiz_AB_Move_3(uint16_t pwm_a, uint16_t pwm_b, int16_t Flow_Coefficient)
{
    // 定义云台A和云台B的最新位置变量
    uint16_t pwm_A_last = 0;
    uint16_t pwm_B_last = 0;
    // 计算云台A和云台B的位置差
    int16_t diff_a = abs(pwm_a - pwm_A_last); //水平方向
    int16_t diff_b = abs(pwm_b - pwm_B_last); //竖直方向

    // 计算移动次数
    int16_t num_steps = (diff_a > diff_b) ? diff_a : diff_b;

    // 计算加速度和减速度所需的步数
    int16_t accel_steps = num_steps / 2;
    int16_t decel_steps = num_steps - accel_steps;

    // 计算加速度和减速度阶段的增量
    float accel_increment_a = (float)(pwm_a - pwm_A_last) / accel_steps;
    float accel_increment_b = (float)(pwm_b - pwm_B_last) / accel_steps;
    
    float decel_increment_a = (float)(pwm_a - pwm_A_last - accel_increment_a * accel_steps) / decel_steps;
    float decel_increment_b = (float)(pwm_b - pwm_B_last - accel_increment_b * accel_steps) / decel_steps;

    // 插值移动云台A和云台B，直到达到目标位置
    for (int16_t i = 0; i <= num_steps; i++)
    {
        // 计算当前位置
        uint16_t current_pwm_a;
        uint16_t current_pwm_b;

        if (i < accel_steps)
        {
            // 加速阶段
            current_pwm_a = round(pwm_A_last + i * accel_increment_a);
            current_pwm_b = round(pwm_B_last + i * accel_increment_b);
        }
        else if (i >= num_steps - decel_steps)
        {
            // 减速阶段
            int16_t decel_index = i - (num_steps - decel_steps);
            current_pwm_a = round(pwm_A_last + accel_steps * accel_increment_a + decel_index * decel_increment_a);
            current_pwm_b = round(pwm_B_last + accel_steps * accel_increment_b + decel_index * decel_increment_b);
        }
        else
        {
            // 匀速阶段
            current_pwm_a = round(pwm_A_last + accel_steps * accel_increment_a + (i - accel_steps) * accel_increment_a * decel_steps);
            current_pwm_b = round(pwm_B_last + accel_steps * accel_increment_b + (i - accel_steps) * accel_increment_b * decel_steps);
        }

        // 设置云台A和云台B的位置
//        SERVO_PWMA_Set(current_pwm_a);
//        SERVO_PWMB_Set(current_pwm_b);

        // 延时一段时间
        HAL_Delay(Flow_Coefficient);
    }

    // 更新云台A和云台B的最新位置
    pwm_A_last = pwm_a;
    pwm_B_last = pwm_b;
}



