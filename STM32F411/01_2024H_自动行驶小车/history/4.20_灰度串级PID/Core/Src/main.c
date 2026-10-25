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
#include "encoder.h"
#include "pwm.h"
#include "debug.h"
#include "GWKJ.h"
#include "Imu.h"
#include "key.h"
#include "pid.h"
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define constrain(value, min_val, max_val) \
    ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))
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
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	Set_PWM(0,0);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_ALL);
	//白色是高电平，为1，黑色是低电平，为2
	uint8_t BIT[8]={0,0,0,0,0,0,0,0};
	extern int Flag;
	int stopflag=0;
	int findflag=0;
	extern float offset;
	extern int IMU_flag;
	extern int turn;
	extern int BASE_SPEED;
	extern int Yaw_Init;
	extern float Gyro_Init;  
	extern float current_angle;
	float last_tick=0;
	GW_digital_Init();
	OLED_Init();//OLED初始化
//	GW_analog_Init();

	int flag_turn=0;
		int flag_sharp_Rturn=0;
	int flag_sharp_Lturn=0;
//	uint8_t Problem_Flag = 0;
//	uint8_t Calibration_Flag;
	DEBUG_info("Main Init",":over");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  OLED_Update();

//		Set_PWM(1000,-1000);

//		DEBUG_info("Encoder_Left111",":%d",(int)Read_Encoder(4));
//		DEBUG_info("Encoder_Left222",":%d",(int)Read_Encoder(5));
//		
//		delay_ms(1000);
//		
//		Set_PWM(-1000,1000);
//		DEBUG_info("Encoder_Left111",":%d",(int)Read_Encoder(4));
//		DEBUG_info("Encoder_Left222",":%d",(int)Read_Encoder(5));
//		
//		delay_ms(1000);
//		GW_digital_eight(BIT);
		
//			DEBUG_info("Grey",":%d,%d,%d,%d,%d,%d,%d,%d",(int)BIT[0],(int)BIT[1],(int)BIT[2],(int)BIT[3],(int)BIT[4],(int)BIT[5],(int)BIT[6],(int)BIT[7]);
		
//		IMU_Proc();
//			DEBUG_info("Yaw",":%f",Yaw);
//		DEBUG_info("Gyro",":%f",Gyro);
//		DEBUG_info("flag",":%d",Problem_Flag);
//			ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//			int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//			float pid_output = ComputePID(offset);   // ִ��PID����
//			AdjustMotorSpeed(pid_output);
			if(Flag==1)
			{
				IMU_Init();    // 初始化陀螺仪
				Gyro_Init = Gyro; // 记录初始角度
				ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
				int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���
				IMU_Proc();
				if(turn == 1)
				{
					Set_PWM(BASE_SPEED,BASE_SPEED);
					delay_ms(10);
					 findflag=2;
						turn =0;
				}
				if(turn == 2)
				{
					Set_PWM(BASE_SPEED,BASE_SPEED);
					delay_ms(10);
           findflag=1;
						turn =0;
				}
				float outer_pid_output = ComputePID(offset);   // ִ��PID����
				outer_pid_output = constrain(outer_pid_output, -15, 15);
				current_angle = Yaw - Yaw_Init;
				uint32_t current_tick = HAL_GetTick();
				float dt = (current_tick - last_tick) / 1000.0f; // 转换为秒
				dt = constrain(dt, 0.001, 0.1); // 限制 dt 范围
				last_tick = current_tick;
				float motor_correction = angle_PID_Controller(outer_pid_output, current_angle, dt);
				AdjustMotorSpeed(motor_correction);
//				if(BIT[5]==1&&BIT[6]==1&&BIT[4]==0&&BIT[7]==0)
//				{
//					IMU_Init();
//					IMU_Proc();
//					int offset_Yaw = CalculateOffset_IMU(-150);
//					float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//					AdjustMotorSpeed(pid_output_IMU);
//					delay_ms(700);0
//					flag_turn=1;
//				}
//				if((BIT[0]==0&&BIT[3]==0&&BIT[1]==1&&BIT[2]==1)||(BIT[0]==0&&BIT[3]==0&&BIT[1]==0&&BIT[2]==1)||(BIT[0]==0&&BIT[1]==0&&BIT[2]==1&&BIT[3]==0))
//				{
//					IMU_Init();
//					IMU_Proc();
//					int offset_Yaw = CalculateOffset_IMU(120);
//					float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//					AdjustMotorSpeed(pid_output_IMU);
//					delay_ms(800);
//				}


			 while(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&&findflag==1)
			 {
				 Set_PWM(-2500,2500);
				 ReadSensors(BIT);
				 if(BIT[3]==0||BIT[4]==0)
				 {
					 findflag=0;
				 }
			 }
		 		while(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&&findflag==2)
			 {
				 Set_PWM(2500,-2500);
				 ReadSensors(BIT);
				 if(BIT[3]==0||BIT[4]==0)
				 {
					 findflag=0;
				 }
			 }

//	   if(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&& flag_sharp_Rturn==0&&flag_sharp_Lturn==0){
//		
//           Set_PWM(0,0);		
//           delay_ms(1500);

//				   Set_PWM(-2200,2200);
//					 delay_ms(900);
//				  	flag_turn=1;
//			    flag_sharp_Rturn++;
//				
//			}
//		 
//			
//				if(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&& flag_sharp_Rturn!=0&&flag_sharp_Lturn<=1){
//		
//           Set_PWM(0,0);		
//           delay_ms(1500);

//				   Set_PWM(1800,-1800);
//					 delay_ms(900);
//				  	flag_turn=1;
//			    flag_sharp_Lturn++;
//				
//			}
			}
			
//			if(Flag==2)
//			{
//				stopflag=1;
//				Set_PWM(0,0);
//				while(stopflag==1)
//				{
//					IMU_Proc();
//					int offset_Yaw = CalculateOffset_IMU(0);
//					float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//					AdjustMotorSpeed(pid_output_IMU);
//					GW_digital_eight(BIT);
//					if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0)
//					{
//						Set_PWM(0,0);
//						stopflag=0;
//					}
//				}
//				while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(300);
//							Set_PWM(0,0);
//							stopflag=1;
//						}
//					}
//				while(stopflag==1)
//					{
//						IMU_flag=0;
//						IMU_Proc();
//						int offset_Yaw = CalculateOffset_IMU(183);
//						float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//						AdjustMotorSpeed(pid_output_IMU);
//						GW_digital_eight(BIT);
//						if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0)
//						{
//							Set_PWM(0,0);
//							stopflag=0;
//						}
//					}
//				while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(200);
//							Set_PWM(0,0);
//							stopflag=1;
//							Flag=0;
//						}
//					}
//				}
//					
//			if(Flag==3)
//			{
//				IMU_Init();
//				stopflag=1;
//				Set_PWM(0,0);
//				while(stopflag==1)
//					{
//						IMU_Proc();
//						float offset_Yaw = CalculateOffset_IMU(39);
//						float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//						AdjustMotorSpeed(pid_output_IMU);
//						GW_digital_eight(BIT);
//						if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0||BIT[1]==0||BIT[6]==0)
//						{
//							delay_ms(100);
//							Set_PWM(0,0);
//							stopflag=0;
//						}
//					}
//				
//				while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						float offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(500);
//							Set_PWM(0,0);
//							stopflag=1;
//						}
//					}
//					
//				while(stopflag==1)
//					{
//						IMU_Proc();
//						float offset_Yaw = CalculateOffset_IMU(143.5);
//						float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//						AdjustMotorSpeed(pid_output_IMU);
//						GW_digital_eight(BIT);
//						if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0||BIT[1]==0||BIT[6]==0)
//						{
//							Set_PWM(0,0);
//							stopflag=0;
//						}
//					}

//				while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(200);
//							Set_PWM(0,0);
//							stopflag=1;
//							Flag=0;
//						}
//					}
//			}
//			
//			if(Flag == 4)
//			{
//				int count = 0;
//				IMU_Init();
//				stopflag=1;
//				Set_PWM(0,0);
//				while(count<4)
//				{
//					while(stopflag==1)
//					{
//						IMU_Proc();
//						float offset_Yaw = CalculateOffset_IMU(39);
//						float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//						AdjustMotorSpeed(pid_output_IMU);
//						GW_digital_eight(BIT);
//						if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0||BIT[1]==0||BIT[6]==0)
//						{
//							delay_ms(100);
//							Set_PWM(0,0);
//							stopflag=0;
//						}
//					}
//				
//					while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						float offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(500);
//							Set_PWM(0,0);
//							stopflag=1;
//						}
//					}
//					
//					while(stopflag==1)
//					{
//						IMU_Proc();
//						float offset_Yaw = CalculateOffset_IMU(143);
//						float pid_output_IMU = angle_PID_Controller(offset_Yaw , 1);
//						AdjustMotorSpeed(pid_output_IMU);
//						GW_digital_eight(BIT);
//						if(BIT[2]==0||BIT[3]==0||BIT[4]==0||BIT[5]==0||BIT[1]==0||BIT[6]==0)
//						{
//							Set_PWM(0,0);
//							stopflag=0;
//						}
//					}

//					while(stopflag==0)
//					{
//						ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
//						int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���       
//						float pid_output = ComputePID(offset);   // ִ��PID����
//						AdjustMotorSpeed(pid_output);
//						if(BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1)
//						{
//							delay_ms(200);
//							Set_PWM(0,0);
//							stopflag=1;
//							Flag=0;
//						}
//					}
//				count++;
//			}
//				
//			
//	}
//				
			
//			ReadSensors(BIT);               // ��ȡ�Ҷȴ���������

//      int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���
//			DEBUG_info("Offset",":%f",(float)offset);
//       
//      float pid_output = ComputePID(offset);   // ִ��PID����
//      AdjustMotorSpeed(pid_output);            // ��������ٶ�
//			
			
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
