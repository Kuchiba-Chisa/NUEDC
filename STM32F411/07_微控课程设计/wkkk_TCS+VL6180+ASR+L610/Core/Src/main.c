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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "debug.h"
#include "TCS.h"
#include "Vl6180.h"
#include <stdio.h> 
#include "L610_Init.h"
#include "L610_TCP.h"
#include "I2C2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern char *strx,*extstrx,*Readystrx; 
char RxBuffer[200],Rxcouter; 
extern char RxBuffer[200],Rxcouter; 
extern char *strstr(const char *, const char *); 
uint8_t Res; 
int Usart1_Rx_Flag=0;
int Flag=0;
int stop=0;
int lightflag=0;
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
//void Clear_Buffer(void) 
//{ 
// uint8_t i; 
// 
//for(i=0;i<Rxcouter;i++) 
// RxBuffer2[i]=0; 
// Rxcouter=0; 
//} 
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		UNUSED(huart);
		if(huart->Instance ==USART1){ 
		RxBuffer[Rxcouter++] = Res; 
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&Res, 1); 
		Usart1_Rx_Flag=1;
		if(Res=='a')
		{
			Flag=1;
			stop=0;
		}
		else if(Res=='b')
		{
			Flag=2;
			stop=0;
		}
		else if(Res=='c')
		{
			Flag=3;
			stop=0;
		}
		else if(Res=='d')
		{
			stop=1;
		}
		else if(Res=='e')
		{
			Flag=5;
			stop=0;
		}
		} 
   
}
const char TEXT1_Buffer[]={"ni hao"};
const char TEXT2_Buffer[]={"kai shi"};
const char TEXT3_Buffer[]={"kai deng"};
const char TEXT4_Buffer[]={"fa guang"};
const char TEXT5_Buffer[]={"xi dian"};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	extern int flag;
	uint8_t a ;
	uint8_t b=0;
	uint8_t buffer[10]={0};
	uint16_t Rawdata = 0;
	uint8_t buffer2;
	extern uint8_t Usart6_Rx_Flag;
  extern uint8_t Usart6_Rx_Buffer[RXBUFFERSIZE];
	int Cdata;
	int Rdata;
	int Gdata;
	int Bdata;
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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	TCS34725_ENABLE();
	SWJ_Init();
//	printf("ATR\r\n");
	HAL_UART_Receive_IT(&huart1,&Res,1); 

//	OPEN_MOUDLE();
//	flag=6;
//	Fibcom_L610_init_pre();

	if(VL6180X_GetID()==VL6180X_DEFAULT_ID)
		VL6180X_Init();
	if (0) //添加的词条和识别模式是可以掉电保存的，第一次设置完成后，可以将此段注释掉，即将1改为0，然后重新下载一次程序
	{
		AsrErase(); //擦除
	  HAL_Delay(60);
	  
		//1：循环识别模式。状态灯常亮（默认模式）    
		//2：口令模式，以第一个词条为口令。状态灯常灭，当识别到口令词时常亮，等待识别到新的语音,并且读取识别结果后即灭掉
		//3：按键模式，按下开始识别，不按不识别。支持掉电保存。状态灯随按键按下而亮起，不按不亮
		
		AsrAddWords(1, (uint8_t*)TEXT1_Buffer, strlen(TEXT1_Buffer));
		HAL_Delay(60);
		AsrAddWords(2, (uint8_t*)TEXT2_Buffer, strlen(TEXT2_Buffer));
		HAL_Delay(60);
		AsrAddWords(3, (uint8_t*)TEXT3_Buffer, strlen(TEXT3_Buffer));
		HAL_Delay(60);
		AsrAddWords(4, (uint8_t*)TEXT4_Buffer, strlen(TEXT4_Buffer));
		HAL_Delay(60);
		AsrAddWords(5, (uint8_t*)TEXT5_Buffer, strlen(TEXT5_Buffer));
		HAL_Delay(60);
		AsrSetMode(1); //设置模式
		HAL_Delay(60);
//		printf("Asr_Result111111:  \r\n"); 
	}
	Flag = 0;
  /* USER CODE END 2 */
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		flag=2;
//		DEBUG_info("start","1");
//		flag=6;
//		printf("AT+MIPCALL=1\r\n");
//		HAL_Delay(1000);			
//		Clear_Buffer();	
//		printf("AT+MIPCALL?\r\n");
//		Clear_Buffer();	
//		HAL_Delay(3000);
//		printf("AT+HMCON=0,60,\"<your-iotda-endpoint>.myhuaweicloud.com\",\"1883\",\"<device_id>\",\"<device_secret>\",0\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);	
//		printf("AT+HMPUB=1,\"$oc/devices/<device_id>/sys/properties/report\",76,\"{\"services\":[{\"service_id\":\"Sprayswitchcontrol\",\"properties\":{\"Switch\":1}}]}\"\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);	
//		printf("AT+CAVIMS=1\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);
//		printf("AT+GTSET=\"GPRSFIRST\",0\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);	
//		printf("AT+GTSET=\"CALLBREAK\",1\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);	
//		printf("AT+CLIP=1\r\n");
//		Clear_Buffer();	
//		HAL_Delay(500);	
//		printf("ATD<phone_number>\r\n");
//		HAL_Delay(2000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		HAL_Delay(1000);
//		UART_SendHex(0x30);		
//		HAL_Delay(1000);
//		UART_SendHex(0x31);
//		HAL_Delay(1000);
//		UART_SendHex(0x32);
//		HAL_Delay(1000);
//		UART_SendHex(0x33);

//		printf("%d\r\n",flag);
//		flag=6;
//		HAL_Delay(500);
//		printf("ATI\r\n"); 
//		flag=2;
//		printf("Received: %s\r\n", RxBuffer);
//		strx=strstr((const char*)RxBuffer,(const char*)"OK"); 
//		if(strx==NULL) 
//		{ 
//			Clear_Buffer(); 
//			DEBUG_info("Receive","fall");
//			HAL_Delay(500); 
//		} 
//		else 
//		{ 
//			Clear_Buffer(); 
//			DEBUG_info("Receive","success");
//			HAL_Delay(500); 
//		} 
//		HAL_I2C_Mem_Read(&hi2c1,I2C_TCS34725_ADDR<<1|I2C_READ,TCS34725_STATUS_ADDR|TCS34725_CMD_MASK,1,buffer,1,3000);
//		if(buffer[0]&TCS34725__STATUS_AVALID)
//		{
//			//RGBC Valid
//			//Get Clear RAW data
//			Rawdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_CDATA_ADDR|TCS34725_CMD_MASK);
//			//Send Clear RAW data to UART
//			Send_RAWData_To_UART('C',Rawdata);
//			//Get RED RAW data
//			Rawdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_RDATA_ADDR|TCS34725_CMD_MASK);
//			//Send RED RAW data to UART
//			Send_RAWData_To_UART('R',Rawdata);
//			//Get Green RAW data
//			Rawdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_GDATA_ADDR|TCS34725_CMD_MASK);
//			//Send Green RAW data to UART
//			Send_RAWData_To_UART('G',Rawdata);
//			//Get Blue RAW data
//			Rawdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_BDATA_ADDR|TCS34725_CMD_MASK);
//			//Send Blue RAW data to UART
//			Send_RAWData_To_UART('B',Rawdata);
//		}
		flag=2;
//		printf("Received: %s\r\n", Usart6_Rx_Buffer);
//		DEBUG_info("Received","%s\r\n",RxBuffer);

//		DEBUG_info("Received","%d\r\n",Usart6_Rx_Flag);
//		HAL_Delay(1000);
//		buffer2 = VL6180X_GetRange();
//		DEBUG_info("Range",":%d",(int)buffer2);
//		UART6_SendChar('1');
//		HAL_Delay(1000);
//		UART6_SendChar('0');
//		HAL_Delay(1000);
//		UART6_SendChar('2');

		if(Flag == 5&&stop==0) //上位机登录后，发送e，开始执行
		{
			flag=2;
			DEBUG_info("Received","%s\r\n",RxBuffer);
//			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

//			Usart6_Rx_Flag=0;
			HAL_Delay(1000);
			buffer2 = VL6180X_GetRange();			//读取距离传感器
			uint8_t temp1=(int)buffer2;
			uint8_t temp3[2]={(int)buffer2,0};
			char temp4=buffer2;
			flag=2;
			DEBUG_info("Range",":%d",(int)buffer2); 
//			HAL_UART_Transmit(&huart1,temp3,1,5);
			char tempStr[10];  
			sprintf(tempStr, "%d", temp1);  

			UART1_SendString(tempStr);		//发送距离数据给上位机
			UART1_SendString("\r\n");
			uint8_t result=0;
			result = Asr_Result(); //返回识别结果，即识别到的词条编号
			DEBUG_info("result",":%d",(int)result); 

			if(result!=0) //对语音模块说“开灯”，开灯
			{
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
			}
			else if(result==2||result==5)//对语音说“西电”，关灯
			{
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
			}
			if(temp1>30)					//若距离（），传给f4再传给fpga驱动电机
			{				
				HAL_Delay(100);
				UART6_SendChar('1');			//给f4发1
				HAL_Delay(100);
				DEBUG_info("uart","ok"); 

				UART1_SendString("jiashui");		//给上位机发“加水”
				UART1_SendString("\r\n");
				lightflag=1;
			}
			if(temp1<10&&lightflag==1)
			{
				HAL_Delay(10000);
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
				lightflag=0;
			}
			//实时检测颜色
		HAL_I2C_Mem_Read(&hi2c1,I2C_TCS34725_ADDR<<1|I2C_READ,TCS34725_STATUS_ADDR|TCS34725_CMD_MASK,1,buffer,1,3000);
		if(buffer[0]&TCS34725__STATUS_AVALID)
		{
			//RGBC Valid
			//Get Clear RAW data
			Cdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_CDATA_ADDR|TCS34725_CMD_MASK);
			//Send Clear RAW data to UART
			Send_RAWData_To_UART('C',Cdata);
			//Get RED RAW data
			Rdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_RDATA_ADDR|TCS34725_CMD_MASK);
			//Send RED RAW data to UART
			Send_RAWData_To_UART('R',Rdata);
			//Get Green RAW data
			Gdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_GDATA_ADDR|TCS34725_CMD_MASK);
			//Send Green RAW data to UART
			Send_RAWData_To_UART('G',Gdata);
			//Get Blue RAW data
			Bdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_BDATA_ADDR|TCS34725_CMD_MASK);
			//Send Blue RAW data to UART
			Send_RAWData_To_UART('B',Bdata);
		}
		if(900<Bdata&&Bdata)//检测到蓝色（异物），给f4发2
		{
			UART6_SendChar('2');
			HAL_Delay(1000);
			DEBUG_info("uart","ok"); 
			
			UART1_SendString("jinggao");//给上位机发送“警告”
			UART1_SendString("\r\n");
		
		}
		}
		if(Flag==1&&stop==0) //测试模式，发送a测颜色，传回B颜色值。
		{
			HAL_I2C_Mem_Read(&hi2c1,I2C_TCS34725_ADDR<<1|I2C_READ,TCS34725_STATUS_ADDR|TCS34725_CMD_MASK,1,buffer,1,3000);
		  if(buffer[0]&TCS34725__STATUS_AVALID)
			{
			//RGBC Valid
			//Get Clear RAW data
				Cdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_CDATA_ADDR|TCS34725_CMD_MASK);
			//Send Clear RAW data to UART
				Send_RAWData_To_UART('C',Cdata);
			//Get RED RAW data
				Rdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_RDATA_ADDR|TCS34725_CMD_MASK);
			//Send RED RAW data to UART
				Send_RAWData_To_UART('R',Rdata);
			//Get Green RAW data
				Gdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_GDATA_ADDR|TCS34725_CMD_MASK);
			//Send Green RAW data to UART
				Send_RAWData_To_UART('G',Gdata);
			//Get Blue RAW data
				Bdata =  TCS34725_Get_RawData(&hi2c1,TCS34725_BDATA_ADDR|TCS34725_CMD_MASK);
			//Send Blue RAW data to UART
				Send_RAWData_To_UART('B',Bdata);
			}
			char tempStr2[10];  
			sprintf(tempStr2, "%d", Bdata); 
			UART1_SendString(tempStr2);
			UART1_SendString("\r\n");
			HAL_Delay(2000);
		}
		if(Flag==2&&stop==0) //测试模式发b，检测距离，传回距离值
		{
			buffer2 = VL6180X_GetRange();			//读取距离传感器
			uint8_t temp1=(int)buffer2;
			char tempStr[10];  
			sprintf(tempStr, "%d", temp1); 
			DEBUG_info("Range",":%d",(int)buffer2);
			UART1_SendString(tempStr);		//发送距离数据给上位机
			UART1_SendString("\r\n");
			HAL_Delay(2000);
		}
		if(Flag==3&&stop==0) //测试模式发c，测试舵机，传回ok
		{
			UART6_SendChar('1');
			HAL_Delay(2000);
			UART1_SendString("ok");		//给上位机发“加水”
			UART1_SendString("\r\n");
			DEBUG_info("uart","ok");
		}

		//UART_SendString("45");
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
