/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-03 15-28-12
 * @FilePath: \Project\RedServo\STM32\User\KEY\key.c
 * @Description: 键盘扫描
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "key.h"
#include "main.h"
#include "debug.h"
#include "pwm.h"

#define KEY1_Pin KEY_1_Pin
#define KEY2_Pin KEY_2_Pin
#define KEY3_Pin KEY_3_Pin
#define KEY4_Pin KEY_4_Pin
#define KEY0_Pin KEY_0_Pin


//#define DEBOUNCE_DELAY 200 // 设置消抖延时为200毫秒


/**
 * @description: 按键初始化 (使用CubeMX自动生成的宏定义，就不用写这个函数了)
 * @return {*}
 */
void Key_Init(void)
{

}



//题目1
void Key_1_Callback(void)
{
		DEBUG_info("key",":Key_1_Callback/n");

}



// 题目2
void Key_2_Callback(void)
{
		DEBUG_info("key",":Key_2_Callback/n");
}


//题目3
void Key_3_Callback(void)
{
		DEBUG_info("key",":Key_3_Callback/n");
}




//题目四
void Key_4_Callback(void){
	    DEBUG_info("key",":Key_4_Callback/n");

}

//灰度传感器校准
void Key_0_Callback(void){
	    DEBUG_info("key",":Key_0_Callback/n");

}



/**
 * @description: 按键检测，外部中断回调函数
 * @param {uint16_t} GPIO_Pin
 * @return {*}
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    /* Prevent unused argument(s) compilation warning */
    UNUSED(GPIO_Pin);
    /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_GPIO_EXTI_Callback could be implemented in the user file
    */
    // 按键按下
    if(GPIO_Pin == KEY1_Pin)
    {
				Key_1_Callback();
//        Debounce(GPIO_Pin);       
    }
    else if(GPIO_Pin == KEY2_Pin)
    {
				Key_2_Callback();
        // 按键2按下的处理代码
//        Debounce(GPIO_Pin);
    }
    else if(GPIO_Pin == KEY3_Pin)
    {
				Key_3_Callback();
        // 按键3按下的处理代码
//        Debounce(GPIO_Pin);
    }
     else if(GPIO_Pin == KEY4_Pin)
    {
				Key_4_Callback();
        // 按键4按下的处理代码
//        Debounce(GPIO_Pin);
    }
		else if(GPIO_Pin == KEY0_Pin)
    {
				Key_0_Callback();
        // 按键4按下的处理代码
    }
}



