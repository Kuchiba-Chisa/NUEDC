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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Stepper_Motor.h"
#include "Delay.h"
#include "yuntai.h"
#include "pid.h"
#include "oled.h"
#include "Bujin.h"
#include "usart_1.h"
#include "serial_it_config.h"
#include "duoji.h"
#include "pid_duoji.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    
{
    if(huart->Instance == USART1)
    {
        UART1_RxCpltCallback(huart);
    }
    else if(huart->Instance == USART2)
    {
        UART2_RxCpltCallback(huart);
    }
    else if(huart->Instance == USART6)
    {
        UART3_RxCpltCallback(huart);
    }
}

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	extern int flag;
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
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	USART_IT_Config();
	Stepper_Motor_Init();
//	Gimbal_Set_Target(90.0,90.0);	
	HAL_GPIO_WritePin(GPIOB,light_Pin,1);
	Bujin_Init();
	HAL_GPIO_WritePin(GPIOB,light_Pin,0);
//	Gimbal_Set_Target(320,240);
	send_servo_command(0,1550,1000);
	send_servo_command(1,1750,1000);
	delay_ms(1000);
//	send_servo_command(0,1560,1000);
//	send_servo_command(1,1740,1000);
//	delay_ms(1000);
//	send_servo_command(0,1570,1000);
//	send_servo_command(1,1730,1000);
//	delay_ms(1000);
//	send_servo_command(0,1580,1000);
//	send_servo_command(1,1720,1000);

	set_target_position(320,240);
//	delay_ms(5000);
  init_path_points();

	HAL_TIM_Base_Start_IT(&htim2);

//	delay_ms(500);
//	set_target_position(240,160);
//	delay_ms(500);
//	set_target_position(400,160);
//	delay_ms(500);
//	set_target_position(400,320);
//	delay_ms(500);
//	set_target_position(240,320);

//	Motor_Move_A(0,10);
//	Motor_Move_B(0,10);	
//	delay_ms(1000);
//	Stepper_Motor_Stop_A();
//	Stepper_Motor_Stop_B();
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
//		Gimbal_Update_Position();
			OLED_Update();
			control_loop();
//		OLED_Printf(0,32,OLED_8X16,"ok");
//			ControlSystem_Update(320,240);
//			if(flag==1)
//			{
//				ControlSystem_Update(240,206);
//			}
//			if(flag==2)
//			{
//				ControlSystem_Update(166,106);
//			}
//		HAL_Delay(10);
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
