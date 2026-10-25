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
#include "string.h"
#include "Delay.h"
#include "usart_1.h"
#include "usart_2.h"
#include "usart_3.h"
#include "serial_it_config.h"
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int index_1ms=0;
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
//{	
//	if (htim->Instance == htim2.Instance)	
//	{		
//		index_1ms++;	    
//	}
//}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//uint8_t rx_buf1[256];  // USART1���ջ�����
//uint8_t rx_buf2[256];  // USART2���ջ�����
//uint8_t rx_buf6[256];  // USART6���ջ�����
//uint8_t Uart_RxBuffer_1;      //�����жϻ���
//uint8_t Uart_RxBuffer_2;      //�����жϻ���
//uint8_t Uart_RxBuffer_6;      //�����жϻ���
//uint8_t Uart_Rx_Cnt_1 = 0;     //���ջ������
//uint8_t Uart_Rx_Cnt_2 = 0;     //���ջ������
//uint8_t Uart_Rx_Cnt_6 = 0;     //���ջ������
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
//	HAL_UART_Receive_IT(&huart1, rx_buf1, sizeof(rx_buf1));
//  HAL_UART_Receive_IT(&huart2, rx_buf2, sizeof(rx_buf2));
//  HAL_UART_Receive_IT(&huart6, rx_buf6, sizeof(rx_buf6));
	USART_IT_Config();
	HAL_TIM_Base_Start_IT(&htim2);
	OLED_Init();
	extern uint8_t Uart_RxBuffer;
	extern uint8_t RxBuffer_2[RXBUFFERSIZE_2];
	extern uint8_t Uart_RxBuffer_2;
	extern uint8_t Uart_RxBuffer_3;
	int flag=0;
	uint16_t K210_data[10]={0};
//	OLED_Init();//OLED初始化
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		OLED_Update();
		
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
		{
			delay_ms(1500);
			int recognize[9]={0,0,0,0,0,0,0,0,0};
			while(index_1ms<12000)
			{		
					OLED_Update();
				  int max=0;
					int second=0;
					Usart2_SendChar('0');
//				  OLED_Printf(0, 16, OLED_8X16, ":%d,%d,%d,%d,%d,%d",K210_data[0],K210_data[1],K210_data[2],K210_data[3],K210_data[4],K210_data[5]);
					if(Uart_RxBuffer_2!=48)
					{
						recognize[Uart_RxBuffer_2-49]++;
					}
//					OLED_Printf(0, 0, OLED_8X16, ":%d,%d,%d,%d,%d,%d",RxBuffer_2[0],RxBuffer_2[1],RxBuffer_2[2],RxBuffer_2[3],RxBuffer_2[4],RxBuffer_2[5]);
					for(int j=0;j<9;j++)
						{
							if(recognize[j]>recognize[max])
							{
								max=j;
							}
						}
					for(int i=0;i<9;i++)
						{
							if((recognize[i]<recognize[max])&&(recognize[i]>recognize[second]))
							{
								second=i;
							}							
						}
						
					OLED_Printf(0, 32, OLED_8X16, "Recognize:%d",(max+1));
					OLED_Printf(0, 48, OLED_8X16, "Recognize:%d",(second+1));
					}
			if(index_1ms>12000)
			{
				OLED_Printf(0, 0, OLED_8X16, "ok:%d",1);

			}
			
			}

		}
			

//		OLED_Update();
//		Usart1_SendNumber(1);
//		Usart2_SendNumber(1);
//		Usart3_SendNumber(1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
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
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  if (huart->Instance == USART1) 
//  {

//    HAL_UART_Receive_IT(&huart1, (uint8_t *)&Uart_RxBuffer_1, 1); // �������ý���
//  }
//	
//	
//	if (huart->Instance == USART2) 
//  {
//		OLED_Printf(0, 0, OLED_8X16, "recieve:%d,%d,%d", rx_buf2[0],rx_buf2[1],rx_buf2[2]);
//		OLED_Printf(0, 16, OLED_8X16, "recieve:%d", Uart_RxBuffer_2);
//		if(Uart_Rx_Cnt_2 >= 255)  //����ж�
//	{
//		Uart_Rx_Cnt_2 = 0;
//		memset(rx_buf2,0x00,sizeof(rx_buf2));
//		HAL_UART_Transmit(&huart1, (uint8_t *)"shujuyichu", 10,0xFFFF); 
//	}
//	else
//	{
//		rx_buf2[Uart_Rx_Cnt_2++] = Uart_RxBuffer_2;
//	
//		
//		if((rx_buf2[Uart_Rx_Cnt_2-1] == 0x0A)&&(rx_buf2[Uart_Rx_Cnt_2-2] == 0x0D)) //�жϽ���λ
//		{
//       
//        // �������д���ֽ���Ϣ���ж�
//      
//      //printf("%d,%d",Position_error[0],Position_error[1]);

//      //��λ
//			Uart_Rx_Cnt_2 = 0;
//			memset(rx_buf2,0x00,sizeof(rx_buf2)); //�������
//		}
//	}
//    HAL_UART_Receive_IT(&huart2, (uint8_t *)&Uart_RxBuffer_2, 1); // �������ý���
//  }
//	
//	
//	if (huart->Instance == USART6) 
//  {

//    HAL_UART_Receive_IT(&huart6, (uint8_t *)&Uart_RxBuffer_6, 1); // �������ý���
//  }
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
