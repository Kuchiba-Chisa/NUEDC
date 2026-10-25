/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
#include "Imu.h"
#include "key.h"
#include "pid.h"
#include "OLED.h"
#include "Buzzer.h"
#include "usart_2.h"
#include "math.h"
#include "app.h"
#include "pwm.h"
#include "GWKJ.h"
#include "encoder.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float Kp = 0.5;  // 比例增益
float Kd = 0.5;  // 微分增益
char RxBuffer[200],Rxcouter; 
uint8_t Res; 
int Usart1_Rx_Flag=0;
int Flag_ly=0;
int stop=0;
char target;

static uint8_t last_state_BZ2 = 1;  // 假设初始状态为高电平
static uint8_t last_state_BZ3 = 1;
static uint8_t last_state_BZ1 = 1;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    
{
    if(huart->Instance == USART1)
    {
        UART1_RxCpltCallback(huart);
    }
//		else if(huart->Instance == USART6)
//		{
//			RxBuffer[Rxcouter++] = Res; 
//			HAL_UART_Receive_IT(&huart6, (uint8_t *)&Res, 1); 
//			Usart1_Rx_Flag=1;
//			if(Res=='a')
//			{
//				Flag_ly=1;
//				stop=0;
//			}
//			else if(Res=='b')
//			{
//				Flag_ly=2;
//				stop=0;
//			}
//			else if(Res=='c')
//			{
//				Flag_ly=3;
//				stop=0;
//			}
//			else if(Res=='d')
//			{
//				Flag_ly=4;
//				stop=0;
//			}
//			else if(Res=='e')
//			{
//				Flag_ly=5;
//				stop=0;
//			}
//		}

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM2_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_ALL);	
	USART2_Init();
//	HAL_TIM_Base_Start_IT(&htim2);

//	Debug_Init();	//白色是高电平，为1，黑色是低电平，为2
	extern int Flag;

	extern int IMU_flag;
	extern float Yaw;
	extern float Gyro;
	extern int turn;
	int turn1;
	int turn2;
	extern int startflag;
	float Yaw_Init;
	int stopflag=0;
	int findflag=1;
	uint8_t Uart_RxBuffer;
	float L_raw;
	int crossflag=0;
	extern uint16_t find[10];
	extern uint8_t RxBuffer_2[RXBUFFERSIZE_2];
	extern uint8_t Uart_RxBuffer_2;
	extern float encoder_left;
	extern float encoder_right;
	int sendflag=0;
	extern float Velocity_Left,Velocity_Right;
	extern int motor_left,motor_right;
	extern float distance;
//	HAL_TIM_Base_Start_IT(&htim3);
	GW_digital_Init();
	OLED_Init();//OLED初始化
//	GW_analog_Init();
	IMU_Init();
	Yaw_Init=Yaw;
	USART2_Init();
	Set_PWM(0,0);
	uint8_t BIT[8]={0,0,0,0,0,0,0,0};
	HAL_TIM_Base_Start_IT(&htim2);

//	Set_PWM(0,2000);

//	HAL_UART_Receive_IT(&huart6,&Res,1); 


//	uint8_t Problem_Flag = 0;
//	uint8_t Calibration_Flag;
//	DEBUG_info("Main Init",":over");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
//		UART6_SendChar('1');
		OLED_Update();
		ReadSensors(BIT);
		OLED_Printf(0, 0, OLED_8X16, "flag:%d", Flag);
		OLED_Printf(0, 16, OLED_8X16, "right:%f", encoder_right);
		OLED_Printf(0, 32, OLED_8X16, "distance:%f", distance);
//		UART1_SendChar('0');
//		UART1_SendChar('.');
//		UART1_SendChar('1');
//		UART1_SendChar('2');
//		UART1_SendString("\r\n");

//		OLED_Printf(0, 0, OLED_8X16, "gw:%d,%d,%d,%d,%d,%d,%d,%d",BIT[0],BIT[1],BIT[2],BIT[3],BIT[4],BIT[5],BIT[6],BIT[7]);
		
		while(Flag==1)//基础1
		{
			OLED_Update();

			problem1();

		}
		
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
float prev_error_left = 0.0;
float prev_error_right = 0.0;
void PD_update(int target_left, int target_right, 
               float actual_left, float actual_right, 
               float dt, 
               int *output_left, int *output_right) {
    
    // 计算左右电机的误差
    float error_left = target_left - actual_left;
    float error_right = target_right - actual_right;
    
    // 计算微分项
    float derivative_left = (error_left - prev_error_left) / dt;
    float derivative_right = (error_right - prev_error_right) / dt;
    
    // 计算 PD 控制输出
    *output_left = Kp * error_left + Kd * derivative_left;
    *output_right = Kp * error_right + Kd * derivative_right;
    
    // 保存当前误差，供下次计算微分项使用
    prev_error_left = error_left;
    prev_error_right = error_right;
}
//float Velocity_Left,Velocity_Right;
float Velocity_Kp = 18, Velocity_Ki = 7.5; //Velocity_Kp_right = 18, Velocity_Ki_right = 7.5;
float Velocity_Target_Max = 500;
int Speed_PWM_Max, Turn_PWM_Max;
float Velocity_PI_left(float Velocity,float Target, int flag)
{  
		Speed_PWM_Max = 11*Velocity_Target_Max;
    static float velocity,Encoder_Last_bias,Encoder_bias, Encoder_Integral;
		Encoder_bias = Target-Velocity;
		Encoder_Integral = Velocity_Ki*Encoder_bias;
		velocity += Velocity_Kp*(Encoder_bias-Encoder_Last_bias) + Encoder_Integral;     //速度控制
		Encoder_Last_bias = Encoder_bias;
//		if(Encoder_Integral > 4500) Encoder_Integral = 4500;
//		if(Encoder_Integral < -4500) Encoder_Integral = -4500;
		if(velocity > Speed_PWM_Max) velocity = Speed_PWM_Max;
		if(velocity < -4000) velocity = -4000;
		if(flag == 0) Encoder_Integral=0, velocity = 0;//电机关闭后清除积分
	  return velocity;
}
float Velocity_PI_right(float Velocity,float Target, int flag)
{  
		Speed_PWM_Max = 11*Velocity_Target_Max;
    static float velocity,Encoder_Last_bias,Encoder_bias, Encoder_Integral;
		Encoder_bias = Target-Velocity;
		Encoder_Integral = Velocity_Ki*Encoder_bias;
		velocity += Velocity_Kp*(Encoder_bias-Encoder_Last_bias) + Encoder_Integral;     //速度控制
		Encoder_Last_bias = Encoder_bias;
//		if(Encoder_Integral > 4500) Encoder_Integral = 4500;
//		if(Encoder_Integral < -4500) Encoder_Integral = -4500;
		if(velocity > Speed_PWM_Max) velocity = Speed_PWM_Max;
		if(velocity < -4000) velocity = -4000;
		if(flag == 0) Encoder_Integral=0, velocity = 0;//电机关闭后清除积分
	  return velocity;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
