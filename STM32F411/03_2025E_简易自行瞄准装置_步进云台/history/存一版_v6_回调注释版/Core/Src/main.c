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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Stepper_Motor.h"
#include "Delay.h"
#include "yuntai.h"
#include "oled.h"
#include "Bujin.h"
#include "usart_1.h"
#include "serial_it_config.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char RxBuffer[200],Rxcouter; 
uint8_t Res; 
char RxBuffer2[200],Rxcouter_2; 
uint8_t Res_2; 
int problem_flag;
int turn_flag;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//   
//		else if(huart->Instance == USART1)
//		{
//			HAL_UART_Receive_IT(&huart1, (uint8_t *)&Res_2, 1);
//			if(Res_2=='1') //转弯
//			{
//				Motor_Move_A(0,50);
//				delay_ms(800);
//				Motor_Move_A(0,0);
//				Res_2='0';
//				
//			}
//			if(Res_2=='2')//竖线
//			{
//				turn_flag=2;
//			}
//			if(Res_2=='3')//远横线
//			{
//				turn_flag=3;
//			}	
//			if(Res_2=='4')//近横线
//			{
//				turn_flag=4;
//			}	
//			if(Res_2=='5')//近横线
//			{
//				turn_flag=5;
//			}
//			if(Res_2=='0')//近横线
//			{
//				turn_flag=0;
//				Motor_Move_A(0,0);
//			}					
//		}
//    else if(huart->Instance == USART6)
//    {
//        UART3_RxCpltCallback(huart);
//    }
//}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern int current_x;
extern int current_y;
int target_X,target_Y;
int stop_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	extern int Flag;
	int send_flag=0;
	extern float Yaw;
	int reset_flag=0;
	extern int index_ms;
	int move_flag=0;
	int start_flag=0;
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
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
	IMU_Init();
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&Res_2, 1);

//	HAL_TIM_Base_Start_IT(&htim2);
	OLED_Init();
	USART_IT_Config();
	Stepper_Motor_Init();
	Stepper_Control_Init();
	HAL_GPIO_WritePin(GPIOB,light_Pin,1);

	delay_ms(1000);
//	test();

//	Speed_bujin();
//	uint8_t usart_sent2[11];
//	StepperMotor motor3 = {
//			.address = 0x02,
//			.huart = &huart6, // 替换为你的 UART 句柄
//			.mode = ABS_ANGLE_CTRL,
//			.subdivision = 0x20, // 32 细分
//			.speed_rads_x10 = 0x000A, // 10 rad/s (0x0064 = 100)
//	};
//	BuildPositionControlFrame(&motor3, 400, CW, usart_sent2); 
//	USART_SendArray(usart_sent2,sizeof(usart_sent2));	
//	sanjiao();

//	delay_ms(2000);
//	BuildPositionControlFrame(&motor2, 900, CW, usart_sent2); // 100.0° 逆时针
//	USART_SendArray(usart_sent2,sizeof(usart_sent2));
//	BuildPositionControlFrame(&motor1, 900, CW, usart_sent1); // 100.0° 逆时针
//	delay_ms(10);
//	USART_SendArray(usart_sent1,sizeof(usart_sent1));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		OLED_Update();
//		IMU_Proc();
		delay_ms(100);
//		Stepper_Control(320,240);
//		DrawRectangle(460, 150, 220, 300, 200);
//		DrawPrecisionRectangle(200, 150, 400, 350);
//		DrawCircle(320, 240, 100, 500);
//		OLED_Printf(0,0,OLED_8X16,"ok");
//		OLED_Printf(0,16,OLED_8X16,"turn_flag:%d",turn_flag);

		if(problem_flag==1) 
		{
			Stepper_Control(target_X,target_Y);

			if(stop_flag==1)
			{
				HAL_TIM_Base_Start_IT(&htim2);
//				move_flag=1;
//				start_flag=1;
//				delay_ms(1000);
			}
			if(index_ms>=1500)
			{
					HAL_GPIO_WritePin(GPIOB,light_Pin,0);
			}
		}
		
		if(problem_flag==2) //逆时针
		{
			if(send_flag==0)
			{
				UART1_SendChar('1');
				send_flag++;
			}
			if(reset_flag==0)
			{

				while(target_X==0)
				{
					Motor_Move_A(1,80);	
					Motor_Move_B(1,5);			
					
				}
				HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); // 停止电机			
				reset_flag++;				
			}
			Stepper_Control_UntilReached(target_X,target_Y);	

			if(stop_flag==1)
			{
				stop_flag=0;
				HAL_TIM_Base_Start_IT(&htim2);
			}
			if(index_ms>=1000)
			{
				HAL_GPIO_WritePin(GPIOB,light_Pin,0);
			}
			if(index_ms>=2000)
			{
				HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				problem_flag=0;
			}
		}
		if(problem_flag==3)//顺时针
		{
			if(send_flag==0)
			{
				UART1_SendChar('1');
				send_flag++;
			}
			if(reset_flag==0)
			{

				while(target_X==0)
				{
					Motor_Move_A(0,80);
					Motor_Move_B(1,5);
				
				}
				HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); // 停止电机				
				reset_flag++;				

			}

			Stepper_Control_UntilReached(target_X,target_Y);	
			if(stop_flag==1)
			{
				stop_flag=0;
				HAL_TIM_Base_Start_IT(&htim2);
			}
			if(index_ms>=1000)
			{
				HAL_GPIO_WritePin(GPIOB,light_Pin,0);
			}
			if(index_ms>=2000)
			{
				HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				problem_flag=0;
			}
		}
		if(problem_flag==4)
		{
			Stepper_Control(target_X,target_Y);

			if(stop_flag==1)
			{
				HAL_TIM_Base_Start_IT(&htim2);
//				move_flag=1;
//				start_flag=1;
//				delay_ms(1000);
			}
			if(index_ms>=1500)
			{
					HAL_GPIO_WritePin(GPIOB,light_Pin,0);
			}
//			Stepper_Control_UntilReached_fast(target_X,target_Y);

//			while(start_flag==1)
//			{
//				if(turn_flag==1)
//				{
//					Motor_Move_A(0,100);
//					delay_ms(700);
//				}
//				while(turn_flag==2)
//				{
//					Stepper_Control_UntilReached(target_X,target_Y);	
//				}
//				while(turn_flag==3)
//				{
//					Motor_Move_A(0,60);
//					Stepper_Control_UntilReached_y();
//				}
//				while(turn_flag==4)
//				{
//					Motor_Move_A(0,80);
//					Stepper_Control_UntilReached_y();
//				}
//				while(turn_flag==5)
//				{
//					Stepper_Control_UntilReached(target_X,target_Y);						
//				}
//			}
//			if(turn_flag==1)
//			{
//				delay_ms(300);
//				Motor_Move_A(0,90);
//				delay_ms(500);
//				turn_flag=0;
//			}
//			if(turn_flag==2)
//			{
//				Motor_Move_A(1,90);
//				delay_ms(500);
//				turn_flag=0;
//			}			
		}
		while(problem_flag==5)
		{

//			while(turn_flag==0)
//			{
//				Stepper_Control(target_X,target_Y);
//			}
			while(turn_flag==1)
			{
				Motor_Move_A(0,80);
				UART1_SendByte(1);
				
			}
//			while(turn_flag==2||turn_flag==3)
//			{
//				Motor_Move_A(0,55);
//			}			
		}
//			if(turn_flag==1)
//			{
//				Motor_Move_A(0,80);
//				delay_ms(500);
//				turn_flag=0;
//			}
		if(problem_flag==6)
		{
			HAL_TIM_Base_Stop(&htim2);
			index_ms=0;
			send_flag=0;
			reset_flag=0;
			Stepper_Motor_Init();
		}
		
//		Stepper_Control_UntilReached(200,100);
//		Stepper_Control_UntilReached(400,100);
//		Stepper_Control_UntilReached(400,200);
//		Stepper_Control_UntilReached(200,200);
//		
			
//		    Motor_Move_B(0,1);	 //B是竖直
//				Motor_Move_A(0,15);  //A是水平
//	
//				Motor_Move_A(0,2);
//				Motor_Move_B(1,15);
				
//				Motor_Move_A(0,5);
//				Motor_Move_B(0,2);
//				
//				Motor_Move_B(0,2);
//				Motor_Move_A(0,5);
//				
//				Motor_Move_B(0,5);
//				Motor_Move_A(0,2);
		
				
  }
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
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if (htim == &htim2)  // X�ᶨʱ��
//    {
//        stepX();
//    }
//    else if (htim == &htim4)  // Y�ᶨʱ��
//    {
//        stepY();
//    }
//}
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
