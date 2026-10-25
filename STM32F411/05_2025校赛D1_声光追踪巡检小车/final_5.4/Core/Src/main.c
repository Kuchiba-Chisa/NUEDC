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
#include "Buzzer.h"
#include "distance.h"
#include "usart_2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float Kp = 0.7;  // 比例增益
float Kd = 0.8;  // 微分增益
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
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	Set_PWM(0,0);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_ALL);	
	USART2_Init();
//	Debug_Init();	//白色是高电平，为1，黑色是低电平，为2
	uint8_t BIT[8]={0,0,0,0,0,0,0,0};
	extern int Flag;
	int stopflag=0;
	int findflag=0;
	int jichu_flag = 0;
	extern float offset;
	extern int IMU_flag;
	extern int turn;
	extern int BASE_SPEED;
	extern float Yaw;
	extern float Yaw_Init;
	float speed_left_actual;
  float speed_right_actual;
  extern int motor_left;
  extern int motor_right;
	extern uint8_t RxBuffer_2[RXBUFFERSIZE_2];
	extern uint8_t Uart_RxBuffer_2;
	float output_left, output_right;
	GW_digital_Init();
	OLED_Init();//OLED初始化
//	GW_analog_Init();
	IMU_Init();
	IMU_Proc();
	Yaw_Init=Yaw;
	int flag_turn=0;
		int flag_sharp_Rturn=0;
	int flag_sharp_Lturn=0;
	int beepflag=1;
	int crossflag=0;
	int bzflag=0;
	uint16_t K210_data[10]={0};

	extern float distance;
//	uint8_t Problem_Flag = 0;
//	uint8_t Calibration_Flag;
//	DEBUG_info("Main Init",":over");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//			Usart2_SendNumber(0);
	    OLED_Update();
//			OLED_Printf(0, 0, OLED_8X16, "recieve:%d,%d,%d", RxBuffer_2[0],RxBuffer_2[1],RxBuffer_2[2]);

//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0)//读取PA0引脚电平，如果低电平，说明有障碍物
//		{
//				OLED_Printf(0, 16, OLED_8X16, "bz:%d", 1);//有
//		}
//		else
//		{
//				OLED_Printf(0, 16, OLED_8X16, "bz:%d", 0);
//		}
//		Set_PWM(2000,2000);//2000-13100 1000-14100 3000 -12100
//		OLED_ShowFloatNum(0, 16,left_motor_actual , 4, 4,OLED_8X16);
//		OLED_ShowSignedNum(0,32,(int)Read_Encoder(4),4,OLED_8X16);
//			speed_left_actual=Read_Encoder(4);
//			speed_right_actual=Read_Encoder(5);
//			OLED_Printf(0, 16, OLED_8X16, "left:%f", speed_left_actual);
//			OLED_Printf(0, 0, OLED_8X16, "right:%f", speed_right_actual);

//			DEBUG_info("Encoder_Left111",":%f",speed_left_actual);
//			DEBUG_info("Encoder_Left222",":%f",speed_right_actual);
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
				IMU_Proc();
				OLED_Printf(0, 0, OLED_8X16, "Yaw:%f", Yaw);
//				OLED_Printf(0, 32, OLED_8X16, "bz:%f", bzflag);
				
				if(beepflag==1)
				{
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					beepflag=0;
				}
				get_distance();
				ReadSensors(BIT);               // ��ȡ�Ҷȴ���������
				int offset = CalculateOffset(BIT); // ����ƫ�����ĵľ���
				if(turn == 1)	//左拐
				{
					Set_PWM(3000,3000);
					delay_ms(100);
					 findflag=1;
						turn =0;

				}
//				
				if(turn == 2)	//右拐
				{
					Set_PWM(3000,3000);
					delay_ms(100);
          findflag=2;
					turn =0;
					crossflag++;
				}
				if(crossflag==1)		//到B点
				{
					delay_ms(150);
					crossflag++;
					while(Yaw>-70)
					{
						IMU_Proc();
						OLED_Printf(0, 16, OLED_8X16, "Yaw:%f", Yaw);
						Set_PWM(-1000,1000);
					}
					Set_PWM(2000,2000);
					delay_ms(700);
					Set_PWM(0,0);
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					bzflag=1;
					BASE_SPEED=4000;
				}
				
				if((bzflag==1)&&(distance>8)&&(distance<20))
				//if(bzflag==1&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0))//读取PA0引脚电平，如果低电平，说明有障碍物
				{	//十字路口处先直走
						BASE_SPEED=2000;
						bzflag=2;
						OLED_Printf(0, 16, OLED_8X16, "bz:%d", 1);//有
						while(BIT[0]==1||BIT[1]==1||BIT[6]==1||BIT[7]==1)
						{
							ReadSensors(BIT);
							Set_PWM(-1000,-1000);
						}
				}					
				
				if((bzflag==2)&&(BIT[0]==0&&BIT[1]==0&&BIT[7]==0&&BIT[6]==0))		//判断十字路口直走后倒退到十字路口
				{					
					bzflag=3;
					Set_PWM(1000,1000);
					delay_ms(1500);	
					IMU_Proc();
					while(Yaw<45)	//十字路口直走后回来再左转
					{
						Set_PWM(1000,-1000);
						IMU_Proc();
					}
					Buzzer_LongBeep();
			
			
				}
				
				if((bzflag==3)&&(distance>8)&&(distance<15))
				//if(bzflag==3&&(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)==0||HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==0))//读取PA0引脚电平，如果低电平，说明有障碍物
					{		//十字路口左拐避障
						bzflag=4;
						while(Yaw>-73)	//十字路口左拐后掉头
					{
						Set_PWM(-1000,1000);
						IMU_Proc();
					}
						Set_PWM(1000,1000);
						jichu_flag = 1 ;
						Buzzer_LongBeep();
					}			
				float pid_output = ComputePID(offset);   
				AdjustMotorSpeed(pid_output);
				if(jichu_flag==1&&BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&&BIT[7]==1)
				{
					Set_PWM(0,0);
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					Buzzer_ShortBeep();
					delay_ms(500);
					Flag=3;
					findflag=0;
				}				

				while(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&&findflag==2&&(bzflag==0||bzflag==1))
				{		//左转
				 Set_PWM(-2000,2000);
				 ReadSensors(BIT);
				 if(BIT[3]==0||BIT[4]==0)
					{
						findflag=0;
					}	
				}
		 		while(BIT[0]==1&&BIT[1]==1&&BIT[2]==1&&BIT[3]==1&&BIT[4]==1&&BIT[5]==1&&BIT[6]==1&&findflag==1&&(bzflag==0||bzflag==1))
				{		//右转
					Set_PWM(2000,-2000);
					ReadSensors(BIT);
					if(BIT[3]==0||BIT[4]==0)
					{
						findflag=0;
					}
				}
			}
			
			if(Flag==2)
			{
					Usart2_SendChar('0');
					BASE_SPEED=1000;
					IMU_Proc();
					float offset_Yaw=Yaw+63;
					float angle_pid=angle_PID_Controller(offset_Yaw,1);
					AdjustMotorSpeed(angle_pid);
					ReadSensors(BIT);
					if(BIT[3]==0||BIT[4]==0)
					{
						Set_PWM(0,0);
	
						Flag=0;
					}

					
			}
			if(Flag==3)
			{
				Usart2_SendChar('1');
				delay_ms(100);
				Flag=0;
			}
			if(Flag==4)
			{
				get_distance();
			}		
//			ReadSensors(BIT);               
//      int offset = CalculateOffset(BIT);      
//      float pid_output = ComputePID(offset);   
//      AdjustMotorSpeed(pid_output);            		
		}}
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
