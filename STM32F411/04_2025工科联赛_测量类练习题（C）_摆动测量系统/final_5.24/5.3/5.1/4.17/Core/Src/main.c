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
#include "distance.h"
#include "usart_2.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float Kp = 0.7;  // 比例增益
float Kd = 0.8;  // 微分增益
char RxBuffer[200],Rxcouter; 
uint8_t Res; 
int Usart1_Rx_Flag=0;
int Flag_ly=0;
int stop=0;
int index_1ms=0;
static uint8_t last_state_BZ2 = 1;  // 假设初始状态为高电平
static uint8_t last_state_BZ3 = 1;
static uint8_t last_state_BZ1 = 1;
void PD_update(int target_left, int target_right, 
               float actual_left, float actual_right, 
               float dt, 
               float *output_left, float *output_right) ;
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
		else if(huart->Instance == USART6)
		{
			RxBuffer[Rxcouter++] = Res; 
			HAL_UART_Receive_IT(&huart6, (uint8_t *)&Res, 1); 
			Usart1_Rx_Flag=1;
			if(Res=='a')
			{
				Flag_ly=1;
				stop=0;
			}
			else if(Res=='b')
			{
				Flag_ly=2;
				stop=0;
			}
			else if(Res=='c')
			{
				Flag_ly=3;
				stop=0;
			}
			else if(Res=='d')
			{
				Flag_ly=4;
				stop=0;
			}
			else if(Res=='e')
			{
				Flag_ly=5;
				stop=0;
			}
		}

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
  MX_TIM2_Init();
  MX_USART6_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

	USART2_Init();
//	Debug_Init();	//白色是高电平，为1，黑色是低电平，为2

	extern int IMU_flag;
	extern float Yaw;
	extern float Gyro;

	float Yaw_Init;
	float min_distance=70;
	float L=0.0;
	float T=0.0;
	float h=0.0;
	float a=0.0;
	int min_flag=0;
	int zhouqi_flag=0;
	int jishu_flag=0;
	int _2flag=0;
	int _1flag=0;
	int _3flag=0;
	int _4flag=0;
	int _5flag=0;
	int i=0;
	float L_raw;
	float a_sum=0.0;
	float a_ave=12.215470;
	float Gyro_max=0.0;
	extern uint8_t RxBuffer_2[RXBUFFERSIZE_2];
	extern uint8_t Uart_RxBuffer_2;
	HAL_TIM_Base_Start_IT(&htim3);

	OLED_Init();//OLED初始化
//	GW_analog_Init();
	IMU_Init();
	IMU_Proc();
	Yaw_Init=Yaw;

	uint16_t K210_data[10]={0};
	HAL_UART_Receive_IT(&huart6,&Res,1); 
	HAL_TIM_Base_Start_IT(&htim2);

	extern float distance;
	extern float last_distance;
	extern float raw_distance;
//	uint8_t Problem_Flag = 0;
//	uint8_t Calibration_Flag;
//	DEBUG_info("Main Init",":over");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	UART6_SendString("start\r\n");//第一题
  while (1)
  {
		
		OLED_Update();
    OLED_Printf(0,0,OLED_8X16,"Imu:%f",Yaw);
		
		if(Flag_ly==1)//第二题
		{
			if(_1flag==0)
			{
				UART6_SendString("1_start\r\n");
				_1flag++;
			}

			get_distance();
			
			IMU_Proc();
//			if(Gyro>Gyro_max&&min_flag<5)
//			{
//				Gyro_max=Gyro;
//			}
			if(-2<Yaw&&Yaw<2&&min_flag<5)
			{
				min_distance=distance;
				min_flag++;
			}
      OLED_Printf(0,48,OLED_8X16,"min:%f",min_distance);

			if(-5.5<Yaw&&Yaw<1.5)
			{
				HAL_GPIO_WritePin(GPIOB,LED_Pin,GPIO_PIN_SET);
				delay_ms(10);
				HAL_GPIO_WritePin(GPIOB,LED_Pin,GPIO_PIN_RESET);

				L=L_raw+(67-min_distance-12.2)/100;
				char tempStr[10];  
				sprintf(tempStr, "%f", L); 
				UART6_SendString(tempStr);
				UART6_SendString("\r\n");
			}
		}
			if(Flag_ly==2)//第三题
			{
				if(_2flag==0)
				{
					UART6_SendString("2_start\r\n");
					_2flag++;
				}
				
				IMU_Proc();
				OLED_Printf(0,48,OLED_8X16,"Gyro:%f",Gyro);

				if(Gyro<10&&Gyro>-10)
				{
					if(zhouqi_flag %2 ==0)
					{
						index_1ms=0;
					}
					if(zhouqi_flag %2 ==1)
					{
						float raw_T=(float)index_1ms/1000*2;
						if(raw_T>1)
						{
							T=raw_T;
						}
//						T = 1.00;
					}
					zhouqi_flag++;
					HAL_GPIO_WritePin(GPIOB,LED_Pin,GPIO_PIN_SET);
					delay_ms(10);
					HAL_GPIO_WritePin(GPIOB,LED_Pin,GPIO_PIN_RESET);
					char tempStr2[10];  
					sprintf(tempStr2, "%f", T); 
					UART6_SendString(tempStr2);
					UART6_SendString("\r\n");					
				}
			}
			if(Flag_ly==3)//第四题
			{
				if(_3flag==0)
				{
					UART6_SendString("3_start\r\n");
					_3flag++;
				}
				IMU_Proc();
				if(-5.5<Yaw&&Yaw<1.5)
				{
					get_distance2();
					h=68-L-12.2-distance;
			    OLED_Printf(0,32,OLED_8X16,"Gyro_max:%f",Gyro_max);
					
					a=atan(h/50.0)*180.00/3.14+2;
					if(a>8&&a<20)
					{
						a_sum+=a;
						i++;
						a_ave=a_sum/i;
					}
					char tempStr6[10];  
					sprintf(tempStr6, "%f", distance);
					UART6_SendString("distance");
					UART6_SendString(tempStr6);
					UART6_SendString("\r\n");
					
					char tempStr3[10];  
					sprintf(tempStr3, "%f", a);
					UART6_SendString("angle");
					UART6_SendString(tempStr3);
					UART6_SendString("\r\n");
					
					char tempStr5[10];  
					sprintf(tempStr5, "%f", a_ave);
					UART6_SendString("ave_angle");
					UART6_SendString(tempStr5);
					UART6_SendString("\r\n");					
				}
			}
			if(Flag_ly==4)//第五题
			{
				if(_4flag==0)
				{
					UART6_SendString("4_start\r\n");
					_4flag++;
				}
//					get_distance();
			if ((HAL_GPIO_ReadPin(BZ2_GPIO_Port, BZ2_Pin) == 0 && last_state_BZ2 == 1) ||
					(HAL_GPIO_ReadPin(GPIOB, BZ3_Pin) == 0 && last_state_BZ3 == 1) ||
					(HAL_GPIO_ReadPin(GPIOB, BZ1_Pin) == 0 && last_state_BZ1 == 1)) 				
				{
					jishu_flag++;
					delay_ms(500);
				}
			last_state_BZ2 = HAL_GPIO_ReadPin(BZ2_GPIO_Port, BZ2_Pin);
			last_state_BZ3 = HAL_GPIO_ReadPin(GPIOB, BZ3_Pin);
			last_state_BZ1 = HAL_GPIO_ReadPin(GPIOB, BZ1_Pin);
				char tempStr4[10];  
				sprintf(tempStr4, "%d", jishu_flag); 
				UART6_SendString(tempStr4);
				UART6_SendString("\r\n");		
			}
			if(Flag_ly==5)
			{
				if(_5flag==0)
				{
					UART6_SendString("IMU_init\r\n");
					_5flag++;
				}
				a_sum=0;
				a_ave=0;
				i=0;
				IMU_Init();
				min_distance=0;
				min_flag=0;
				_1flag=0;
				_2flag=0;
				_3flag=0;
				_4flag=0;
				jishu_flag=0;
				get_distance();
				L_raw=67-distance-12.2;

			}
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */


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
               float *output_left, float *output_right) {
    
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
