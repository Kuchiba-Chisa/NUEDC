#ifndef __I2C2_H
#define __I2C2_H
#include <stdint.h>
#include "main.h"

//hiwonder 语音模块例程
/*
 * 1.目前语音模块只能识别汉字，需要将准备识别的汉字转换成拼音字母且汉字之间使用空格分割，例如 你好对应为"ni hao"
 * 2.最多添加50个词条，每个词条最长为79个字符，每个词条最多10个汉字
 * 3.每个词条需要对应一个识别号（1~255随意设置）不同的语音词条可以对应同一个识别号，比如“幻尔科技”和“幻尔”都可以将识别号设置为同一个值
 * 4.模块上的STA状态灯：亮起表示正在识别语音，灭掉表示不会识别语音，当识别到语音时状态灯会变暗，或闪烁，等待读取后会恢复当前的状态指示
 */
 


//hiwonder 语音模块例程
/*
 * 只能识别汉字，将要识别的汉字转换成拼音字母，每个汉字之间空格隔开，比如：幻尔科技 --> huan er ke ji
 * 最多添加50个词条，每个词条最长为79个字符，每个词条最多10个汉字
 * 每个词条都对应一个识别号（1~255随意设置）不同的语音词条可以对应同一个识别号，
 * 比如“幻尔科技”和“幻尔”都可以将识别号设置为同一个值
 * 模块上的STA状态灯：亮起表示正在识别语音，灭掉表示不会识别语音，当识别到语音时状态灯会变暗，或闪烁，等待读取后会恢复当前的状态指示
 */
 


//*****软件模拟IIC，
//*****修改宏定义即可
//*****不同芯片时注意时钟和延时函数
#define    IIC_IO_SDA      GPIO_PIN_12  //SDA的IO口
#define    IIC_IO_SCL      GPIO_PIN_1  //SCL的IO口
#define    GPIOX           GPIOB       //GPIOx选择

#define    Asr_Addr        0xF2  
#define    Asr_Read        0xF3

void HAL_Delay_us_init(uint8_t SYSCLK);

void SDA_OUT(void);
void SDA_IN(void);
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(uint8_t ack);
void AsrAddWords(uint8_t idNum, uint8_t words[], uint8_t len);
void AsrErase(void);
void AsrSetMode(uint8_t mode);
int Asr_Result(void);

#endif
