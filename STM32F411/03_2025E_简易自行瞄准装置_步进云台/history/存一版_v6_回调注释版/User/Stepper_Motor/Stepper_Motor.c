#include "Stepper_Motor.h"
#include "tim.h"
#include "gpio.h"
#include "math.h"
#include "pid.h"
#include "oled.h"
#include "Delay.h"
//#include <stdlib.h> // 包含rand()函数
//#include <time.h>   // 包含time()函数
/* 电机参数配置 */
uint32_t steps_per_rev = 1000;        // 每转步数，用于控制转速
volatile uint32_t steps_count_A = 0;  // 电机A步数计数
volatile uint32_t steps_count_B = 0;  // 电机B步数计数

/* PID控制器实例 */
static PID_Controller pid_x = {0};    // X轴PID控制器
static PID_Controller pid_y = {0};    // Y轴PID控制器
static PID_Controller pid_x_fast = {0};    // X轴PID控制器
static PID_Controller pid_y_fast = {0};    // Y轴PID控制器
static PID_Controller pid_x_turn1 = {0};    // X轴PID控制器
static PID_Controller pid_y_turn1 = {0};    // Y轴PID控制器
static PID_Controller pid_x_turn2 = {0};    // X轴PID控制器
static PID_Controller pid_y_turn2 = {0};    // Y轴PID控制器
static PID_Controller pid_x_turn3 = {0};    // X轴PID控制器
static PID_Controller pid_y_turn3 = {0};    // Y轴PID控制器
static PID_Controller pid_x_turn4 = {0};    // X轴PID控制器
static PID_Controller pid_y_turn4 = {0};    // Y轴PID控制器
/* 目标位置和当前位置 */
static int prev_target_X = 0;
static int prev_target_Y = 0;

int jiguang_x = 198;            // 默认屏幕中心X坐标(640/2)
int jiguang_y = 157; 					// 默认屏幕中心Y坐标(480/2)
extern int target_X;
extern int target_Y;
extern int current_x;                 // 从视觉识别获取的当前X坐标
extern int current_y;                 // 从视觉识别获取的当前Y坐标
extern int stop_flag;
/* 步进电机初始化 */



// 初始化随机种子（在程序开始时调用一次）
//void Init_Random() {
//    srand(HAL_GetTick()); // 使用系统滴答计时器作为种子
//}

//// 生成0或1的随机数
//uint8_t Random_Direction() {
//    return rand() % 2; // 返回0或1
//}

void Stepper_Motor_Init(void)
{
    /* 电机A初始化 */
    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET);  // 使能电机A
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);         // 启动PWM通道1
    
    /* 电机B初始化 */
    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET);  // 使能电机B
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);         // 启动PWM通道2
    
    /* 初始化PID控制器 */
    Stepper_Control_Init();
}

/* PID控制器初始化 */
void Stepper_Control_Init(void)
{
    // 初始化X轴PID (Kp, Ki, Kd, 最大输出, 最小输出)
    PID_Init(&pid_x, 0.7f, 0.0f, 0.8f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y, 0.7f, 0.0f, 0.8f, 1000.0f, 0.0f);
    // 初始化X轴PID (Kp, Ki, Kd, 最大输出, 最小输出)
    PID_Init(&pid_x_fast, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y_fast, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
	
	  PID_Init(&pid_x_turn1, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y_turn1, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
	
	  PID_Init(&pid_x_turn2, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y_turn2, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
	
	  PID_Init(&pid_x_turn3, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y_turn3, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
	
	  PID_Init(&pid_x_turn4, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
    // 初始化Y轴PID
    PID_Init(&pid_y_turn4, 0.8f, 0.0f, 1.0f, 1000.0f, 0.0f);
}

void Stepper_Control(target_X,target_Y)
{
    // 更新目标位置

//    OLED_Printf(0,0,OLED_8X16,"%d,%d",target_x,target_y);
    // 计算误差
      int error_x =  240 - target_X;
      int error_y =  198 - target_Y ; //174
	
//	OLED_Printf(0,32,OLED_8X16,"%d",error_x);
//	OLED_Printf(0,48,OLED_8X16,"%d",error_y);
    
    // 更新PID控制器
    float output_x = PID_Update(&pid_x, (float)error_x, 0.01);
    float output_y = PID_Update(&pid_y, (float)error_y, 0.01);
		
		if(output_x>=200)output_x = 200;
		if(output_y>=200)output_y = 200;
		if(output_x<=-200)output_x = -200;
	  if(output_y<=-200)output_y = -200;
	
//		OLED_Printf(0,32,OLED_8X16,"%.2f",output_x);
//		OLED_Printf(0,48,OLED_8X16,"%.2f",output_y);
    
    /* X轴控制 (Motor_A) */
    if (abs(error_x) > 2) { // 死区控制，避免微小抖动
        // 方向判断：error_x > 0 向左(0)，error_x < 0 向右(1)
        uint8_t dir_x = (error_x > 0) ? 1 : 0;
        // 速度控制：PID输出绝对值作为速度
        Motor_Move_A(dir_x, (uint32_t)fabs(output_x));
    } else {
        Stepper_Motor_Stop_A(); // 误差小于死区，停止电机
    }
    
    /* Y轴控制 (Motor_B) */
    if (abs(error_y) > 2) { // 死区控制
        // 方向判断：error_y > 0 向下(1)，error_y < 0 向上(0)
        // 注意：Y轴方向与X轴相反，需要根据实际硬件调整
        uint8_t dir_y = (error_y > 0) ? 1 : 0;
        Motor_Move_B(dir_y, (uint32_t)fabs(output_y));
    } else {
        Stepper_Motor_Stop_B();
    }
		stop_flag = 1;

}

/* 闭环控制直到到达目标位置 */
void Stepper_Control_UntilReached()
{
	  
    int error_x, error_y;
    float output_x, output_y;
//    OLED_Printf(0,16,OLED_8X16,"11111");
//		OLED_Update();
		int target_changed = (target_X != prev_target_X) || (target_Y != prev_target_Y); //没检测到
		int target_changed_large = (target_X - prev_target_X > 50) || (target_Y - prev_target_Y > 40); //检测到脏东西
		if(target_changed==0 || target_changed_large==1 )
		{
//				HAL_GPIO_WritePin(GPIOB,light_Pin,1);//没找到先关灯
			  Stepper_Motor_Stop_A();
        Stepper_Motor_Stop_B();
		}

    do {
        // 更新目标位置

     
        // 计算当前误差
        error_x =  220 - target_X;
        error_y =  187 - target_Y ; //174
        
        // 更新PID控制器
        output_x = PID_Update(&pid_x, (float)error_x, 1);
        output_y = PID_Update(&pid_y, (float)error_y, 1);
			
//				OLED_Printf(0,16,OLED_8X16,"error_x:%d",error_x);
//				OLED_Update();

        // 输出限幅 (±15)
        if(output_x >= 15) output_x = 15;
        if(output_y >= 15) output_y = 15;
        if(output_x <= -15) output_x = -15;
        if(output_y <= -15) output_y = -15;
        
				/* X轴控制 (Motor_A) */
        if (abs(error_x) > 2) {
            uint8_t dir_x = (error_x > 0) ? 1 : 0;
            Motor_Move_A(dir_x, (uint32_t)fabs(output_x));
        } else {
            Stepper_Motor_Stop_A();
        }
				
				/* Y轴控制 (Motor_B) */
        if (abs(error_y) > 2) {
            uint8_t dir_y = (error_y > 0) ? 1 : 0;
            Motor_Move_B(dir_y, (uint32_t)fabs(output_y));
//	          Motor_Move_B(1, 5);

        } else {
            Stepper_Motor_Stop_B();

        }		
       
    } while (abs(error_x) >= 2 && abs(error_y) >= 2); // 当X和Y误差都小于2时退出
    prev_target_X = target_X;
    prev_target_Y = target_Y;	
    // 最终停止电机
		stop_flag = 1;
    Stepper_Motor_Stop_A();
    Stepper_Motor_Stop_B();
}

void Stepper_Control_UntilReached_x()
{
	  
    int error_x;
    float output_x;
//    OLED_Printf(0,16,OLED_8X16,"11111");
//		OLED_Update();
		int target_changed = (target_X != prev_target_X); //没检测到
		int target_changed_large = (target_X - prev_target_X > 50); //检测到脏东西
		if(target_changed==0 || target_changed_large==1 )
		{
//				HAL_GPIO_WritePin(GPIOB,light_Pin,1);//没找到先关灯
			  Stepper_Motor_Stop_A();
		}

    do {
        // 更新目标位置

     
        // 计算当前误差
        error_x =  223 - target_X;
        
        // 更新PID控制器
        output_x = PID_Update(&pid_x, (float)error_x, 0.01);
			
//				OLED_Printf(0,16,OLED_8X16,"error_x:%d",error_x);
//				OLED_Update();

        // 输出限幅 (±15)
        if(output_x >= 30) output_x = 30;
        if(output_x <= -30) output_x = -30;
        
				/* X轴控制 (Motor_A) */
        if (abs(error_x) > 2) {
            uint8_t dir_x = (error_x > 0) ? 1 : 0;
            Motor_Move_A(dir_x, (uint32_t)fabs(output_x));
        } else {
            Stepper_Motor_Stop_A();
        }
					
       
    } while (abs(error_x) >= 2); // 当X和Y误差都小于2时退出
    prev_target_X = target_X;
    // 最终停止电机
		stop_flag = 1;
    Stepper_Motor_Stop_A();
}	

void Stepper_Control_UntilReached_y()
{
    int error_y;
    float output_y;
//    OLED_Printf(0,16,OLED_8X16,"11111");
//		OLED_Update();
		int target_changed = (target_Y != prev_target_Y); //没检测到
		int target_changed_large = (target_Y - prev_target_Y > 40); //检测到脏东西
		if(target_changed==0 || target_changed_large==1 )
		{
//				HAL_GPIO_WritePin(GPIOB,light_Pin,1);//没找到先关灯
        Stepper_Motor_Stop_B();
		}

    do {
        // 更新目标位置

     
        // 计算当前误差
        error_y =  166 - target_Y ; //174
        
        // 更新PID控制器
        output_y = PID_Update(&pid_y, (float)error_y, 0.01);
			
//				OLED_Printf(0,16,OLED_8X16,"error_x:%d",error_x);
//				OLED_Update();

        // 输出限幅 (±15)
        if(output_y >= 30) output_y = 30;
        if(output_y <= -30) output_y = -30;
        
				
				/* Y轴控制 (Motor_B) */
        if (abs(error_y) > 2) {
            uint8_t dir_y = (error_y > 0) ? 1 : 0;
            Motor_Move_B(dir_y, (uint32_t)fabs(output_y));
//	          Motor_Move_B(1, 5);

        } else {
            Stepper_Motor_Stop_B();

        }		
       
    } while (abs(error_y) >= 2); // 当X和Y误差都小于2时退出
    prev_target_Y = target_Y;	
    // 最终停止电机
		stop_flag = 1;
    Stepper_Motor_Stop_B();	
}


void Stepper_Control_UntilReached_fast()
{
	  
    int error_x, error_y;
    float output_x, output_y;
//    OLED_Printf(0,16,OLED_8X16,"11111");
//		OLED_Update();
		int target_changed = (target_X != prev_target_X) || (target_Y != prev_target_Y); //没检测到
		int target_changed_large = (target_X - prev_target_X > 50) || (target_Y - prev_target_Y > 40); //检测到脏东西
		if(target_changed==0 || target_changed_large==1 )
		{
//				HAL_GPIO_WritePin(GPIOB,light_Pin,1);//没找到先关灯
			  Stepper_Motor_Stop_A();
        Stepper_Motor_Stop_B();
		}

    do {
        // 更新目标位置

     
        // 计算当前误差
        error_x =  220 - target_X;
        error_y =  187 - target_Y ; //174
        
        // 更新PID控制器
        output_x = PID_Update(&pid_x_fast, (float)error_x, 0.01);
        output_y = PID_Update(&pid_y_fast, (float)error_y, 0.01);
			
//				OLED_Printf(0,16,OLED_8X16,"error_x:%d",error_x);
//				OLED_Update();

        // 输出限幅 (±15)
        if(output_x >= 50) output_x = 50;
        if(output_y >= 50) output_y = 50;
        if(output_x <= -50) output_x = -50;
        if(output_y <= -50) output_y = -50;
        
				/* X轴控制 (Motor_A) */
        if (abs(error_x) > 2) {
            uint8_t dir_x = (error_x > 0) ? 1 : 0;
            Motor_Move_A(dir_x, (uint32_t)fabs(output_x));
        } else {
            Stepper_Motor_Stop_A();
        }
				
				/* Y轴控制 (Motor_B) */
        if (abs(error_y) > 2) {
            uint8_t dir_y = (error_y > 0) ? 1 : 0;
            Motor_Move_B(dir_y, (uint32_t)fabs(output_y));
//	          Motor_Move_B(1, 5);

        } else {
            Stepper_Motor_Stop_B();

        }		
       
    } while (abs(error_x) >= 2 && abs(error_y) >= 2); // 当X和Y误差都小于2时退出
    prev_target_X = target_X;
    prev_target_Y = target_Y;	
    // 最终停止电机
		stop_flag = 1;
    Stepper_Motor_Stop_A();
    Stepper_Motor_Stop_B();
}

/* 控制电机A运动 */
void Motor_Move_A(uint8_t direction, uint32_t speed_rpm)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR1_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_SET); // 使能电机
    
    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    // 设置PWM参数
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, period / 2); // 50%占空比
}

/* 控制电机B运动 */
void Motor_Move_B(uint8_t direction, uint32_t speed_rpm)
{
    // 设置方向
    HAL_GPIO_WritePin(GPIOB, DIR2_Pin, (direction == 1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_SET); // 使能电机
    
    // 计算PWM参数
    uint32_t pwm_freq = speed_rpm * steps_per_rev / 60;
    uint32_t period = (SystemCoreClock / 100) / pwm_freq - 1;
    
    // 设置PWM参数
    __HAL_TIM_SET_AUTORELOAD(&htim4, period);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, period / 2); // 50%占空比

}

/* 停止电机A */
void Stepper_Motor_Stop_A(void)
{
//    HAL_GPIO_WritePin(GPIOB, EN1_Pin, GPIO_PIN_RESET); // 禁用电机
	Motor_Move_A(1,0);
    OLED_Printf(0,16,OLED_8X16,"11111");
		OLED_Update();
}

/* 停止电机B */
void Stepper_Motor_Stop_B(void)
{
//    HAL_GPIO_WritePin(GPIOB, EN2_Pin, GPIO_PIN_RESET); // 禁用电机
	Motor_Move_B(1,0);
}

/* 设置每转步数 */
void Stepper_Motor_Set_StepsPerRev(uint32_t steps)
{
    steps_per_rev = steps;
}


/* 高精度矩形绘制函数（50细分） */
void DrawPrecisionRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    const uint16_t total_segments = 50;  // 总分段数
    uint16_t segments_per_side = total_segments / 4;  // 每条边分段数
    uint16_t i;
    
    // 计算每条边的步进增量（使用浮点提高精度）
    float x_step = (float)(x2 - x1) / segments_per_side;
    float y_step = (float)(y2 - y1) / segments_per_side;
    
    // 上边（从左到右）
    for(i = 0; i <= segments_per_side; i++) {
        uint16_t x = x1 + (uint16_t)(i * x_step);
        uint16_t y = y1;
        Stepper_Control_UntilReached(x, y);
        delay_ms(10);  // 微延时确保运动平稳
    }
    
    // 右边（从上到下）
    for(i = 0; i <= segments_per_side; i++) {
        uint16_t x = x2;
        uint16_t y = y1 + (uint16_t)(i * y_step);
        Stepper_Control_UntilReached(x, y);
        delay_ms(10);
    }
    
    // 下边（从右到左）
    for(i = 0; i <= segments_per_side; i++) {
        uint16_t x = x2 - (uint16_t)(i * x_step);
        uint16_t y = y2;
        Stepper_Control_UntilReached(x, y);
        delay_ms(10);
    }
    
    // 左边（从下到上）
    for(i = 0; i <= segments_per_side; i++) {
        uint16_t x = x1;
        uint16_t y = y2 - (uint16_t)(i * y_step);
        Stepper_Control_UntilReached(x, y);
        delay_ms(10);
    }
    
    // 回到起点闭合图形
    Stepper_Control_UntilReached(x1, y1);
}

/**
  * @brief  高精度圆形绘制函数
  * @param  center_x: 圆心X坐标
  * @param  center_y: 圆心Y坐标
  * @param  radius: 半径(像素)
  * @param  segments: 细分点数(建议≥36)
  * @retval 无
  */
void DrawCircle(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t segments)
{
    float angle_increment = 2 * 3.1415926 / segments;  // 角度增量
    float current_angle = 0;
    
    for(uint16_t i = 0; i <= segments; i++) {
        // 计算圆上点的坐标
        uint16_t x = center_x + (int16_t)(radius * cosf(current_angle));
        uint16_t y = center_y + (int16_t)(radius * sinf(current_angle));
        
        // 移动到当前点
        Stepper_Control_UntilReached(x, y);
        
        // 更新角度
        current_angle += angle_increment;
    }
    
    // 闭合圆形（回到起点）
    Stepper_Control_UntilReached(center_x + radius, center_y);
}
