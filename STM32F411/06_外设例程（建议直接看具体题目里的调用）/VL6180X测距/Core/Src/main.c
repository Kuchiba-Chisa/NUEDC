/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
static void VL6180X_Init(void);
//write one byte to VL6180X
static HAL_StatusTypeDef VL6180X_WriteByte(uint16_t regAddress, uint8_t data);
//read one byte from VL6180X
static HAL_StatusTypeDef VL6180X_ReadByte(uint16_t regAddress, uint8_t* data);
//get id of VL6180X
static uint8_t VL6180X_GetID(void);
//get the range from VL6180X
static uint8_t VL6180X_GetRange(void);
//redirect printf to UART2
int fputc(int ch, FILE* f);
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
	uint8_t buffer;
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
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  if(VL6180X_GetID()==VL6180X_DEFAULT_ID)
		VL6180X_Init();
  while (1)
  {
		//get range
		buffer = VL6180X_GetRange();
		printf("Range: %d mm \r\n", buffer);
		HAL_Delay(3000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
  * @brief Init Vl1680x
  * @param  None
  * @retval None
  */
void VL6180X_Init(void)
{
	uint8_t data=0;
	VL6180X_ReadByte(0x016,&data);
	if(data ==1)
	{
		VL6180X_WriteByte(0x0207, 0x01); 
		VL6180X_WriteByte(0x0208, 0x01);
		VL6180X_WriteByte(0x0096, 0x00); 
		VL6180X_WriteByte(0x0097, 0xfd); 
		VL6180X_WriteByte(0x00e3, 0x01); 
		VL6180X_WriteByte(0x00e4, 0x03);
		VL6180X_WriteByte(0x00e5, 0x02); 
		VL6180X_WriteByte(0x00e6, 0x01); 
		VL6180X_WriteByte(0x00e7, 0x03); 
		VL6180X_WriteByte(0x00f5, 0x02); 
		VL6180X_WriteByte(0x00d9, 0x05); 
		VL6180X_WriteByte(0x00db, 0xce); 
		VL6180X_WriteByte(0x00dc, 0x03); 
		VL6180X_WriteByte(0x00dd, 0xf8); 
		VL6180X_WriteByte(0x009f, 0x00); 
		VL6180X_WriteByte(0x00a3, 0x3c); 
		VL6180X_WriteByte(0x00b7, 0x00); 
		VL6180X_WriteByte(0x00bb, 0x3c); 
		VL6180X_WriteByte(0x00b2, 0x09);
		VL6180X_WriteByte(0x00ca, 0x09); 
		VL6180X_WriteByte(0x0198, 0x01);
		VL6180X_WriteByte(0x01b0, 0x17); 
		VL6180X_WriteByte(0x01ad, 0x00);
		VL6180X_WriteByte(0x00ff, 0x05);
		VL6180X_WriteByte(0x0100, 0x05);
		VL6180X_WriteByte(0x0199, 0x05);
		VL6180X_WriteByte(0x01a6, 0x1b);
		VL6180X_WriteByte(0x01ac, 0x3e); 
		VL6180X_WriteByte(0x01a7, 0x1f);
		VL6180X_WriteByte(0x0030, 0x00);
		// Recommended : Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x0011, 0x10); // Enables polling for new Sample ready when measurement completes
		VL6180X_WriteByte(0x010a, 0x30); // Set the averaging sample period (compromise between lower noise and increased execution time)
		VL6180X_WriteByte(0x003f, 0x46); // Sets the light and dark gain (upper nibble). Dark gain should not be changed.
		VL6180X_WriteByte(0x0031, 0xFF); // sets the # of range measurements after which auto calibration of system is performed 
		VL6180X_WriteByte(0x0040, 0x63); // Set ALS integration time to 100ms
		VL6180X_WriteByte(0x002e, 0x01); // perform a single temperature calibration of the ranging sensor 
	  //Optional: Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x001b, 0x09); // Set default ranging inter-measurement period to 100ms
		VL6180X_WriteByte(0x003e, 0x31); // Set default ALS inter-measurement period to 500ms
		VL6180X_WriteByte(0x0014, 0x24); // Configures interrupt on new Sample Ready threshold event’
		
		VL6180X_WriteByte(0x0016, 0x00);
}
	
}

/**
  * @brief  Read one byte to  Vl1680x
	* @param  regAddress: Address to Read
	* @param  data: Point to the memory for data
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef VL6180X_ReadByte(uint16_t regAddress, uint8_t* data)
{
	uint16_t DevAddress=0;
	DevAddress = (VL6180X_DEFAULT_I2C_ADDR<<1)|1;
	return 	HAL_I2C_Mem_Read(&hi2c2, DevAddress, regAddress, VL6180X_REG_ADDRESS_SIZE, data,1,30);
}

/**
  * @brief  Write one byte to  Vl1680x
	* @param  regAddress: Address to write
	* @param  data: Data to be writed
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef VL6180X_WriteByte(uint16_t regAddress, uint8_t data)
{
	uint16_t DevAddress=0;
	DevAddress = (VL6180X_DEFAULT_I2C_ADDR<<1)&0xFE;
	return HAL_I2C_Mem_Write(&hi2c2, DevAddress, regAddress, VL6180X_REG_ADDRESS_SIZE, &data,1,30);
}

/**
  * @brief  Get ID of Vl1680x
	* @param  None
  * @retval Vl1680x's ID
  */
uint8_t VL6180X_GetID(void)
{
	uint8_t ID;
	if(VL6180X_ReadByte( VL6180X_REG_ID, &ID))
		ID =0;//get fail, return 0
	return ID;		
}

/**
  * @brief  Get result of range
	* @param  None
  * @retval Result of range
  */
uint8_t VL6180X_GetRange(void)
{
	uint8_t status,range;
	//get status 
	VL6180X_ReadByte( VL6180X_REG_RESULT_RANGE_STATUS, &status);
	
	while(!(status&0x01))
	{
		//Not ready
		VL6180X_ReadByte( VL6180X_REG_RESULT_RANGE_STATUS, &status);
		HAL_Delay(10);
	}
	
	//start range by write 0x01 to SYSRANGE_START Register(0x18) 
	range=1;
	VL6180X_WriteByte(VL6180X_REG_SYSRANGE_START,range);
	//Wait for ranging completion
	VL6180X_ReadByte( VL6180X_REG_RESULT_INTERRUPT_STATUS, &status);
	while(!(status&0x04))
	{
		VL6180X_ReadByte( VL6180X_REG_RESULT_INTERRUPT_STATUS, &status);
		HAL_Delay(100);
	}
	//get range
	VL6180X_ReadByte(VL6180X_REG_RESULT_RANGE_VAL,&range);
	//clear interrupt status
	VL6180X_WriteByte(VL6180X_REG_SYSTERM_INTERRUPT_CLEAR,0x07);
	return range;	
}

/**
  * @brief Redirect fpuc Function to UART2
  * @param  ch:  Data to be send
  * @param  f: Not used
  * @retval None
  */
int fputc(int ch, FILE* f)
{
	//HAL¿âÐÎÊ½
	//HAL_UART_Transmit(&huart2,(uint8_t*)&ch,1,0xffff);	
	//UART register begin
	while( (huart2.Instance->SR & 0x40) == 0 );
	huart2.Instance->DR = ch;
	//UART register end
	return ch;
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
