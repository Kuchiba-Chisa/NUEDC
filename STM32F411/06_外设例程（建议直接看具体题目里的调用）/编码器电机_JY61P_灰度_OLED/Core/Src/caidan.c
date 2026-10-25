#include "main.h"
#include "caidan.h"
#include "oled.h"

uint8_t key1,key2;
uint8_t  func_index = 0;//主程序此时所在界面的索引值
extern int Yaw;
extern float Velocity_Left,Velocity_Right;

typedef struct
{
    uint8_t current;//当前状态索引号
    uint8_t next; //向下一个
    uint8_t enter; //确定
    void (*current_operation)(void); //当前状态应该执行的操作
} Menu_table;

void (*current_operation_index)(void);

Menu_table  table[17]=
{
    {0,0,1,(*show0)},//一级界面
	
    {1,2, 5,(*show1)},//二级界面    第一行
    {2,3, 9,(*show2)},//二级界面    第二行
    {3,4,13,(*show3)},//二级界面   第三行
    {4,1, 0,(*show4)},//三级界面    Back
	
    {5, 6, 4, (*show5)},  //三级菜单 Back
    {6, 7, 5, (*show6)}, //三级菜单 
    {7, 8, 5, (*show7)}, //三级菜单 
    {8, 5, 5, (*show8)}, //三级菜单 

    {9 , 10, 4, (*show9)},    //三级菜单  Back
    {10, 11, 9, (*show10)}, //三级菜单
    {11, 12, 9, (*show11)}, //三级菜单 
    {12, 9 , 9, (*show12)},  //三级菜单 

    {13, 14, 4, (*show13)},  //三级菜单  Back
    {14, 15, 13, (*show14)}, //三级菜单
    {15, 16, 13, (*show15)}, //三级菜单 
    {16, 13, 13, (*show16)}, 
};



uint8_t KEY1_Get(void)
{
	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
	{
		HAL_Delay(20);
		while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1);
		HAL_Delay(20);
		key1=1;
	}
	else
		key1=0;

	return key1;
}

uint8_t KEY2_Get(void)
{
	if (HAL_GPIO_ReadPin(KEY_stop_GPIO_Port, KEY_stop_Pin) == 1)
	{
		HAL_Delay(20);
		while (HAL_GPIO_ReadPin(KEY_stop_GPIO_Port, KEY_stop_Pin) == 1);
		HAL_Delay(20);
		key2=1;
	}
	else
		key2=0;

	return key2;
}


void  Menu_key_set(void)
{
  key1 = KEY1_Get();
  key2 = KEY2_Get();
  if(key1 == 1)
  {   
    func_index=table[func_index].next;//按键next按下后的索引号
    OLED_Clear();
  }
  if(key2 == 1)
  {
    func_index=table[func_index].enter;
    OLED_Clear();
  }

  current_operation_index=table[func_index].current_operation;//执行当前索引号所对应的功能函数。
  (*current_operation_index)();//执行当前操作函数
}



void show0(void)//第1页
{
	OLED_ShowString(0, 0,"Data",OLED_8X16);
	OLED_ShowString(0,16,"yaw = ",OLED_8X16);
	OLED_ShowNum(48,16,Yaw,6,OLED_8X16);
	OLED_ShowString(0,32,"left = ",OLED_8X16);
	OLED_ShowNum(56,32,Velocity_Left,6,OLED_8X16);
	OLED_ShowString(0,48,"right = ",OLED_8X16);
	OLED_ShowNum(64,48,Velocity_Right,6,OLED_8X16);
//	OLED_ReverseArea(0,0,80,16);
	OLED_Update();
}


void show1(void)//第2页
{
	OLED_ShowString(0, 0,"test1    <",OLED_8X16);
	OLED_ShowString(0,16,"test2",OLED_8X16);
	OLED_ShowString(0,32,"test3",OLED_8X16);
	OLED_ShowString(0,48,"Back ",OLED_8X16);
	OLED_ReverseArea(0,0,80,16);
	OLED_Update();
}

void show2(void)//第3页
{
	OLED_ShowString(0, 0,"test1",OLED_8X16);
	OLED_ShowString(0,16,"test2    <",OLED_8X16);
	OLED_ShowString(0,32,"test3",OLED_8X16);
	OLED_ShowString(0,48,"Back ",OLED_8X16);
	OLED_ReverseArea(0,16,80,16);
	OLED_Update();
}
void show3(void)//第4页
{
	OLED_ShowString(0, 0,"test1",OLED_8X16);
	OLED_ShowString(0,16,"test2",OLED_8X16);
	OLED_ShowString(0,32,"test3    <",OLED_8X16);
	OLED_ShowString(0,48,"Back ",OLED_8X16);
	OLED_ReverseArea(0,32,80,16);
	OLED_Update();
}
void show4(void)//第5页
{
	OLED_ShowString(0, 0,"test1    ",OLED_8X16);
	OLED_ShowString(0,16,"test2",OLED_8X16);
	OLED_ShowString(0,32,"test3",OLED_8X16);
	OLED_ShowString(0,48,"Back     <",OLED_8X16);
	OLED_ReverseArea(0,48,80,16);
	OLED_Update();
}





void show5(void)//第6页
{
	OLED_ShowString(0, 0,"Back     <",OLED_8X16);
	OLED_ShowString(0,16,"test1_1",OLED_8X16);
	OLED_ShowString(0,32,"test1_2",OLED_8X16);
	OLED_ShowString(0,48,"test1_3",OLED_8X16);
	OLED_ReverseArea(0,0,80,16);
	OLED_Update();
}
void show6(void)//第7页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test1_1  <",OLED_8X16);
	OLED_ShowString(0,32,"test1_2",OLED_8X16);
	OLED_ShowString(0,48,"test1_3",OLED_8X16);
	OLED_ReverseArea(0,16,80,16);
	OLED_Update();
}
void show7(void)//第8页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test1_1",OLED_8X16);
	OLED_ShowString(0,32,"test1_2  <",OLED_8X16);
	OLED_ShowString(0,48,"test1_3",OLED_8X16);
	OLED_ReverseArea(0,32,80,16);
	OLED_Update();
}
void show8(void)//第9页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test1_1",OLED_8X16);
	OLED_ShowString(0,32,"test1_2",OLED_8X16);
	OLED_ShowString(0,48,"test1_3  <",OLED_8X16);
	OLED_ReverseArea(0,48,80,16);
	OLED_Update();
}


void show9(void)//第10页
{
	OLED_ShowString(0, 0,"Back     <",OLED_8X16);
	OLED_ShowString(0,16,"test2_1",OLED_8X16);
	OLED_ShowString(0,32,"test2_2",OLED_8X16);
	OLED_ShowString(0,48,"test2_3",OLED_8X16);
	OLED_ReverseArea(0,0,80,16);
	OLED_Update();
}
void show10(void)//第11页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test2_1  <",OLED_8X16);
	OLED_ShowString(0,32,"test2_2",OLED_8X16);
	OLED_ShowString(0,48,"test2_3",OLED_8X16);
	OLED_ReverseArea(0,16,80,16);
	OLED_Update();
}
void show11(void)//第12页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test2_1",OLED_8X16);
	OLED_ShowString(0,32,"test2_2  <",OLED_8X16);
	OLED_ShowString(0,48,"test2_3",OLED_8X16);
	OLED_ReverseArea(0,32,80,16);
	OLED_Update();
}
void show12(void)//第13页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test2_1",OLED_8X16);
	OLED_ShowString(0,32,"test2_2",OLED_8X16);
	OLED_ShowString(0,48,"test2_3  <",OLED_8X16);
	OLED_ReverseArea(0,48,80,16);
	OLED_Update();
}


void show13(void)//第14页
{
	OLED_ShowString(0, 0,"Back     <",OLED_8X16);
	OLED_ShowString(0,16,"test3_1",OLED_8X16);
	OLED_ShowString(0,32,"test3_2",OLED_8X16);
	OLED_ShowString(0,48,"test3_3",OLED_8X16);
	OLED_ReverseArea(0,0,80,16);
	OLED_Update();
}
void show14(void)//第15页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test3_1  <",OLED_8X16);
	OLED_ShowString(0,32,"test3_2",OLED_8X16);
	OLED_ShowString(0,48,"test3_3",OLED_8X16);
	OLED_ReverseArea(0,16,80,16);
	OLED_Update();
}
void show15(void)//第16页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test3_1",OLED_8X16);
	OLED_ShowString(0,32,"test3_2  <",OLED_8X16);
	OLED_ShowString(0,48,"test3_3",OLED_8X16);
	OLED_ReverseArea(0,32,80,16);
	OLED_Update();
}
void show16(void)//第17页
{
	OLED_ShowString(0, 0,"Back     ",OLED_8X16);
	OLED_ShowString(0,16,"test3_1",OLED_8X16);
	OLED_ShowString(0,32,"test3_2",OLED_8X16);
	OLED_ShowString(0,48,"test3_3  <",OLED_8X16);
	OLED_ReverseArea(0,48,80,16);
	OLED_Update();
}



