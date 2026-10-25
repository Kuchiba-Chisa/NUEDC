/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-04 15-50-04
 * @FilePath: \Project\RedServo\STM32\User\APP\app.c
 * @Description: 主程序入口，包含主循环和中断回调函数，以及一些全局变量的定义
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "app.h"
#include "Buzzer.h"
#include "at24c02.h"
#include "string.h"
#include <math.h>

//题目选择标志位
extern uint8_t Problem_Flag = 0;
//校准确认标志位
extern uint8_t Calibration_Flag =0;
//消息接收标志位
extern uint8_t K210_Flag =0;

//--------------------------舵机参考系---------------------
//手动调整
uint16_t Screen_Width = 310;
uint16_t Screen_Height = 294;
//中心位置处参数
extern uint16_t Centre_A = 1503;
extern uint16_t Centre_B = 1471;
// 舵机（左上位置）
extern uint16_t left_PWM = 1565;
extern uint16_t up_PWM =1523;
// 舵机（右下位置）
extern uint16_t right_PWM = 1441;
extern uint16_t down_PWM = 1419;

//--------------------------K210参考系---------------------
//左上角坐标
extern uint16_t K210_left = 0;
extern uint16_t K210_up= 0;
//右下角坐标
extern uint16_t K210_right = 0;
extern uint16_t K210_down= 0;
//宽度长度
extern uint16_t K210_Width = 0;
extern uint16_t K210_Height= 0;

extern uint16_t K210_data[8];

//实时位置
extern uint16_t pwm_A ;
extern uint16_t pwm_B ;

#define DEVICE_ADDRESS 0x50

/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
    OLED_Init();          // 0.96oled初始化
    OLED_Clear();
    USART_IT_Config();    // 总串口接收初始化
    PWM_Init();
    Yuntai_Control();        // 云台初始化
    HAL_TIM_Base_Start_IT(&htim4); // 启动定时器4 开始数据采样,执行控制
    Update_Parameters();
	  //硬件自检
		DEBUG_info("UART2", "Connect successfully!");
//    uint8_t data_str[255] = "test openmv connection";
//	  HAL_Delay(500);
//	  pwm_A = 2000;
//		pwm_B = 2000;
//		Yuntai_Control(); 
//	  HAL_Delay(500);
//		pwm_A = 1000;
//		pwm_B = 1000;
//		Yuntai_Control(); 
//		HAL_Delay(500);
//	  pwm_A = 2000;
//		pwm_B = 2000;
//		Yuntai_Control(); 
//    HAL_UART_Transmit(&huart2, test_str, sizeof(test_str)-1, 1000);
}



//上电与K210通讯拿参数
void Update_Parameters(void)
{
		OLED_ShowChar(2,2,'h',16);
    DEBUG_info("Reading","Message read start \r\n");
    uint8_t data_str[255]={0};
    //通知K210传递参数
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",1,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)
    {
        HAL_Delay(500);
        DEBUG_info("Reading","Waiting for reading.... \r\n");
    }
    K210_Flag = 0;
    //更新参数
    left_PWM =K210_data[0];
    up_PWM=K210_data[1];
    right_PWM=K210_data[2];
    down_PWM=K210_data[3];
    K210_left=K210_data[4];
    K210_up=K210_data[5];
    K210_right=K210_data[6];
    K210_down=K210_data[7];
    //计算参数
    Screen_Width =left_PWM - right_PWM;
    Screen_Height = down_PWM - up_PWM;
    //计算中间
    Centre_A =(left_PWM+right_PWM)/2;
    Centre_B =(up_PWM+down_PWM)/2;
    //计算长宽
    K210_Width = K210_right - K210_left;
    K210_Height= K210_down - K210_up;
		
//		DEBUG_info ("Reading","K210_left:%d,K210_up:%d",(int)K210_left,(int)K210_up);
//		DEBUG_info ("Reading","K210_right:%d,K210_down:%d",(int)K210_right,(int)K210_down);
		
    DEBUG_info("Reading","renew \r\n");
    Buzzer_LongBeep();
}

//校准模式(待优化)
void System_Calibration(void)
{ 
     uint8_t data_str[255]={0};
    //移动到左上角
    pwm_A = left_PWM;
    pwm_B= up_PWM;
    DEBUG_info("System_Calibration","by hand model start/r/n");
    //进入手动模式
    while(Calibration_Flag !=1)
    {
        if (HAL_GPIO_ReadPin(KEY_11_GPIO_Port, KEY_11_Pin) == 0) 
				{
					pwm_B -= 5;
//					DEBUG_info("System_Calibration","1 UP/r/n");
				}
        //下
        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 0) 
					{
					pwm_B += 5;
//					DEBUG_info("System_Calibration","1 DOWN/r/n");
				}
        //左
        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 0) 
					{
					pwm_A += 5;
//					DEBUG_info("System_Calibration","1 LEFT/r/n");
				}
        //右
        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 0) 
					{
					pwm_A -= 5;
//					DEBUG_info("System_Calibration","1 RIGHT/r/n");
				}
        HAL_Delay(30);
        Yuntai_Control();
				
    }
    Calibration_Flag = 0;
    //保存当前值为新的左上角参数
    left_PWM = pwm_A;
    up_PWM = pwm_B;
    //向K210确定坐标
    strcpy((char*)data_str, "");
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",2,left_PWM,up_PWM);
    Usart2_SendString(data_str);
    
    while(K210_Flag == 0)HAL_Delay(10);
    
    K210_Flag = 0;
    K210_left = K210_data[0];
    K210_up = K210_data[1];
		DEBUG_info ("ZUO_SHANG","left_PWM:%d,up_PWM:%d",(int)pwm_A,(int)pwm_B);
		DEBUG_info ("ZUO_SHANG_XIA_K210","K210_left:%d,K210_up:%d",(int)K210_left,(int)K210_up);
		DEBUG_info("System_Calibration","ZUOSHANG renew/r/n");
    
    //移动到右下角
    pwm_A =  left_PWM -Screen_Width;
    pwm_B =  up_PWM + Screen_Height;
    //进入手动模式
    while(Calibration_Flag !=1)
    {
//        if (HAL_GPIO_ReadPin(KEY_11_GPIO_Port, KEY_11_Pin) == 1) pwm_B -= 1;
//        //下
//        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) pwm_B += 1;
//        //左
//        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1) pwm_A += 1;
//        //右
//        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) pwm_A -= 1;
			
			
				if (HAL_GPIO_ReadPin(KEY_11_GPIO_Port, KEY_11_Pin) == 0) 
				{
					pwm_B -= 5;
//					DEBUG_info("System_Calibration","2 UP/r/n");
				}
        //下
        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 0) 
					{
					pwm_B += 5;
//					DEBUG_info("System_Calibration","2 DOWN/r/n");
				}
        //左
        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 0) 
					{
					pwm_A += 5;
//					DEBUG_info("System_Calibration","2 LEFT/r/n");
				}
        //右
        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 0) 
					{
					pwm_A -= 5;
//					DEBUG_info("System_Calibration","2 RIGHT/r/n");
				}
					
        HAL_Delay(30);
        Yuntai_Control();
    }
    //保存当前值为新的右下角参数
    right_PWM = pwm_A;
    down_PWM = pwm_B;
    //向K210确定坐标
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",3,right_PWM,down_PWM);
    Usart2_SendString(data_str);
    
    
    while(K210_Flag == 0)HAL_Delay(10); //等待K210回应
    
    K210_Flag = 0;
    K210_right = K210_data[2];
    K210_down = K210_data[3];
		DEBUG_info("System_Calibration","YOUXIA renew/r/n");
    DEBUG_info ("YOU_XIA_PWM","right_PWM:%d,down_PWM:%d",(int)pwm_A,(int)pwm_B);
		DEBUG_info ("YOU_XIA_K210","K210_right:%d,K210_down:%d",(int)K210_right,(int)K210_down);
		
    DEBUG_info("System_Calibration","By_hand Model Over/r/n");
    
    //---------计算参数--------------------
    
    //---------舵机坐标系
  
    //计算长宽
    Screen_Width =left_PWM - right_PWM;
    Screen_Height = down_PWM - up_PWM;
     
    //计算中间
    Centre_A =(left_PWM+right_PWM)/2;
    Centre_B =(up_PWM+down_PWM)/2;

    //---------K210坐标系
    //计算长宽
    K210_Width = K210_left - K210_right;
    K210_Height= K210_down - K210_up;
    
    //------------保存参数------------------
    sprintf((char*)data_str, "id:%d,left_PWM:%d,up_PWM:%d,right_PWM:%d,down_PWM:%d,K210_left:%d,K210_up:%d,K210_right:%d,K210_down:%d\r\n", 
        5, left_PWM, up_PWM, right_PWM, down_PWM, K210_left, K210_up, K210_right, K210_down);
		Usart2_SendString(data_str);
    //while(K210_Flag == 0)HAL_Delay(10); //等待K210回应
    Problem_Flag =1;
}


//题目1
void Problem1(void){
        //执行一次
        pwm_A =  Centre_A;
        pwm_B =  Centre_B;
        Yuntaiz_A_Move(pwm_A,2);   // 云台丝滑移动
        Yuntaiz_B_Move(pwm_B,2);   // 云台丝滑移动
        HAL_Delay(50    );
				DEBUG_info ("Problem1","Finished");
        Buzzer_LongBeep();
}

//题目2
void Problem2(void){
    
        Yuntaiz_AB_Move(Centre_A,Centre_B,5);   //运动到中间
        Yuntaiz_AB_Move(left_PWM,up_PWM,5);//斜着移动到最左上角  
        Yuntaiz_A_Move(right_PWM,5);   //运动到最右方
        Yuntaiz_B_Move(down_PWM,5);   //运动到最下方
        Yuntaiz_A_Move(left_PWM,5);   //运动到最左方
        Yuntaiz_B_Move(up_PWM,5);   //
        HAL_Delay(100);
        Yuntaiz_AB_Move(Centre_A,Centre_B,5);   //运动到中间
				DEBUG_info ("Problem2","Finished");
}

//题目4
void Problem4(void){
    //A4巡线
    Yuntaiz_AB_Move(Centre_A,Centre_B,5);//斜着移动到校准后中心  
    uint16_t  A_x,A_y,B_x,B_y,C_x,C_y,D_x,D_y;
    //向K210请求4个点的坐标
    uint8_t data_str[255]={0};
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",4,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)HAL_Delay(10);  //等待K210回应
    Buzzer_ShortBeep();
    K210_Flag = 0;
    A_x = K210_data[0];
    A_y = K210_data[1];
    B_x = K210_data[2];
    B_y = K210_data[3];
    C_x = K210_data[4];
    C_y = K210_data[5];
    D_x = K210_data[6];
    D_y = K210_data[7];  
    
		DEBUG_info ("Problem3_original_position:","A_x:%d,A_y:%d",(int)A_x,(int)A_y);
		DEBUG_info ("Problem3_original_position:","B_x:%d,B_y:%d",(int)B_x,(int)B_y);
		DEBUG_info ("Problem3_original_position:","C_x:%d,C_y:%d",(int)C_x,(int)C_y);
		DEBUG_info ("Problem3_original_position:","D_x:%d,D_y:%d",(int)D_x,(int)D_y);
		

    
    //计算坐标映射
    float ka = (float)Screen_Width/(float)K210_Width;
    float kb = (float)Screen_Height/(float)K210_Height;
    
    DEBUG_info ("Problem3","Screen_Width:%f,K210_Width:%f\n",(float)Screen_Width,(float)K210_Width);
    DEBUG_info("Problem3","Screen_Height:%f,K210_Height:%f\n",(float)Screen_Height,(float)K210_Height);
    DEBUG_info ("Problem3","ka:%f,kb:%f\n",ka,kb);

    DEBUG_info ("Problem3_K210_ZBX","K210_left:%d,K210_up:%d,K210_right:%d,K210_down:%d\n",K210_left,K210_up,K210_right,K210_down);
		DEBUG_info ("Problem3_MOTO_PWM","left_PWM:%d,up_PWM:%d,right_PWM:%d,down_PWM:%d\n",left_PWM,up_PWM,right_PWM,down_PWM);
    //计算A点坐标
    A_x = right_PWM + round((A_x - K210_right)*ka);
    A_y = down_PWM  + round((A_y - K210_down))*kb ;
    

    //计算B点坐标
    B_x = right_PWM + round((B_x - K210_right)*ka);
    B_y = down_PWM  + round((B_y - K210_down))*kb ;
     
     
    //计算C点坐标
    C_x = right_PWM + round((C_x - K210_right)*ka);
    C_y = down_PWM  + round((C_y - K210_down))*kb ;

    //计算D点坐标
    D_x = right_PWM + round((D_x - K210_right)*ka);
    D_y = down_PWM  + round((D_y - K210_down))*kb ;
		
		DEBUG_info ("Problem3_afterchange_final","A_x:%d,A_y:%d\n",A_x,A_y);
		DEBUG_info ("Problem3_afterchange_final","B_x:%d,B_y:%d\n",B_x,B_y);
		DEBUG_info ("Problem3_afterchange_final","C_x:%d,C_y:%d\n",C_x,C_y);
		DEBUG_info ("Problem3_afterchange_final","D_x:%d,D_y:%d\n",D_x,D_y);

    //运动到A点
    Yuntaiz_AB_Move(A_x,A_y,5);  
    //运动到B点
    Yuntaiz_AB_Move(B_x,B_y,5);
    //运动到C点
    Yuntaiz_AB_Move(C_x,C_y,5);
    //运动到D点
    Yuntaiz_AB_Move(D_x,D_y,5);
    //运动到A点
    Yuntaiz_AB_Move(A_x,A_y,5);  
        
		DEBUG_info ("Problem3","Finished");
}

//题目3
void Problem3(void){
    Yuntaiz_AB_Move(left_PWM,up_PWM,5);//斜着移动到最左上角  
    uint16_t  A_x,A_y,B_x,B_y,C_x,C_y,D_x,D_y;
    //向K210请求4个点的坐标
    uint8_t data_str[255]={0};
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",6,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)HAL_Delay(10);  //等待K210回应
    Buzzer_ShortBeep();
    K210_Flag = 0;
 
		while(1)
    {
        if (K210_Flag == 1)
        {
            printf("Calculation");
            //进行pid控制
            uint16_t red_x=K210_data[2] ,red_y=K210_data[3],target_x=K210_data[0],target_y=K210_data[1];
						DEBUG_info ("Problem3:","target_x:%d,target_y:%d,red_x:%d,red_y:%d",target_x,target_y,red_x,red_y);
//            if(red_x > 2000)
//            {
//                red_x -=2000;
//            }
            int16_t x_err =  red_x - target_x, y_err = red_y - target_y;
            if(abs((int)(x_err)) <=1  && abs((int)(y_err))<=1)
            {
                 //声光提示
                Buzzer_LongBeep();
                LED_Toggle(2);
                HAL_Delay(500);
                LED_Toggle(2);
                HAL_Delay(500);
                LED_Toggle(2);
            }
            else{
                   Yuntai_PID(x_err,y_err);
            }

            K210_Flag = 0;
        }
    }
		DEBUG_info ("Problem3","Finished");
}


/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
    static uint8_t last_Flag =0;
    Menu_Refresh(); // 刷新菜单
    switch (Problem_Flag)
    {
    case 0:
		{
			last_Flag = Problem_Flag;	
//			DEBUG_info("App_Task","case 0 \r\n");
		}
            	
    case 1:
        if(last_Flag != Problem_Flag)
        {
            Problem1();
					  DEBUG_info("App_Task","case 1 \r\n");
            last_Flag = Problem_Flag;
        }
        break;
    case 2:
        if(last_Flag != Problem_Flag) 
        {
            Problem2();
					  DEBUG_info("App_Task","case 2 \r\n");
            last_Flag = Problem_Flag;
        }
        break;
    case 3:
        if(last_Flag != Problem_Flag)
        {
            Problem3();
					  DEBUG_info("App_Task","case 3 \r\n");
            last_Flag = Problem_Flag;
        }
        break;
        
    case 4:
         if(last_Flag != Problem_Flag)
        {
            Problem4();
					  DEBUG_info("App_Task","case 4 \r\n");
            last_Flag = Problem_Flag;
        }
        break;
    case 9:
        //未校准
		DEBUG_info("App_Task","case 9 \r\n");
        if(Calibration_Flag ==0 )
        {
            //进行校准
            System_Calibration();

        }
        else
        {
					  //上
            if (HAL_GPIO_ReadPin(KEY_11_GPIO_Port, KEY_11_Pin) == 1) 
						{
							pwm_B -= 1;
							DEBUG_info("App_Task","case 9 up \r\n");
						}
							
            //下
            if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) 
							{
							pwm_B += 1;
							DEBUG_info("App_Task","case 9 down \r\n");
						}
            //左
            if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1)
							{
							pwm_A += 1;
							DEBUG_info("App_Task","case 9 left \r\n");
						}
            //右
            if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) 
							{
							pwm_A -= 1;
							DEBUG_info("App_Task","case 9 right \r\n");
						}
            HAL_Delay(1);
            Yuntai_Control();
					  DEBUG_info("App_Task","case 9,jiaozhun by hand \r\n");
        }
        //云台手动移动模式
        

        break;
    default:
        break;
    }
}




// 定时器中断回调函数(1ms一次)
uint8_t LED_Heartbeat = 0; // LED心跳
uint16_t encoder_count = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4) // 判断中断是否来自于定时器4
    {
        // 心跳(50ms一次)
        LED_Heartbeat++;
        if (LED_Heartbeat == 30)
        {
            LED_Toggle(1);
        }
    }
}

 
