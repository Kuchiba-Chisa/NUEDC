#include "I2C.h"
#include "stm32f4xx_hal.h"



uint32_t fac_us;

void HAL_Delay_us_init(uint8_t SYSCLK)
{
     fac_us=SYSCLK; 
}

void HAL_Delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload=SysTick->LOAD;
    ticks=nus*fac_us; 
    told=SysTick->VAL; 
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)tcnt+=told-tnow;
            else tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)break; 
        }
    };
}

void SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;	
 			
  GPIO_InitStruct.Pin = IIC_IO_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
}

void SDA_IN(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;	
	GPIO_InitStruct.Pin = IIC_IO_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  //GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	
}


void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = IIC_IO_SCL|IIC_IO_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 // GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL|IIC_IO_SDA, GPIO_PIN_SET);
	
}


void IIC_Start(void)
{
	SDA_OUT();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
	HAL_Delay_us(5);
	
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
//	IIC_SDA=1;
//	IIC_SCL=1;
//	DelayUs(5);
//	IIC_SDA=0;
//	DelayUs(2);
//	IIC_SCL=0;
}

void IIC_Stop(void)
{
	SDA_OUT();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_RESET);
	HAL_Delay_us(5);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_SET);
	
	HAL_Delay_us(5);
	
//	SDA_OUT();
//	IIC_SCL=0;
//	IIC_SDA=0;
//	DelayUs(5);
//	IIC_SCL=1;
//	IIC_SDA=1;
//	DelayUs(5);
}

void IIC_Ack(void)
{
	
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=0;
//	DelayUs(2);
//	IIC_SCL=1;
//	DelayUs(2);
//	IIC_SCL=0;
}
	

void IIC_NAck(void)
{
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
	SDA_OUT();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_SET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=1;
//	DelayUs(2);
//	IIC_SCL=1;
//	DelayUs(2);
//	IIC_SCL=0;
	
}

uint8_t IIC_Wait_Ack(void)
{
	
	uint8_t Time = 0;
	SDA_IN();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_SET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
	HAL_Delay_us(2);
	
	while(HAL_GPIO_ReadPin(GPIOB, IIC_IO_SDA))
	{
		Time++;
		if(Time>250)
		{
			IIC_Stop();
			return 1;
		}
		
	}
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
//	u8 Time=0;
//	SDA_IN();
//	IIC_SDA=1;
//	DelayUs(2);
//	IIC_SCL=1;
//	DelayUs(2);
//	while(READ_SDA)
//	{
//		Time++;
//		if(Time>250)
//		{
//			IIC_Stop();
//			return 1;
//		}
//	}
//	IIC_SCL=0;
	return 0;
}
	

void IIC_Send_Byte(uint8_t txd)
{
	
	uint8_t t;
	SDA_OUT();
	HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
	for(t=0;t<8;t++)
	{
		if((txd&0x80)>>7)
			HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOB, IIC_IO_SDA, GPIO_PIN_RESET);
		txd<<=1;
		HAL_Delay_us(2);
		HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
		HAL_Delay_us(2);
		HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
		//DelayUs(2);
	}
//	u8 t;
//	SDA_OUT();
//	IIC_SCL=0;
//	for(t=0;t<8;t++)
//	{
//		IIC_SDA=(txd&0x80)>>7;
//		txd<<=1;
//		DelayUs(2);
//		IIC_SCL=1;
//		DelayUs(2);
//		IIC_SCL=0;
//	//  DelayUs(2);
//	}
}


uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN();
  for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_RESET);
		HAL_Delay_us(2);
		HAL_GPIO_WritePin(GPIOB, IIC_IO_SCL, GPIO_PIN_SET);
		receive<<=1;
		if(HAL_GPIO_ReadPin(GPIOB, IIC_IO_SDA))
			receive++;
		HAL_Delay_us(1);
	}
	
	if(!ack) IIC_NAck();
	else IIC_Ack();
	
	return receive;		
//	u8 i,receive=0;
//	SDA_IN();
//	for(i=0;i<8;i++)
//	{
//		IIC_SCL=0;
//		DelayUs(2);
//		IIC_SCL=1;
//		receive<<=1;
//		if(READ_SDA)receive++;
//		DelayUs(1);
//	}
//	if(!ack) IIC_NAck();
//	else IIC_Ack();
//	return receive;
}




/*
 * 添加词条函数，
 * idNum：词条对应的识别号，1~255随意设置。识别到该号码对应的词条语音时，
 *        会将识别号存放到ASR_RESULT_ADDR处，等待主机读取，读取后清0
 * words：要识别汉字词条的拼音，汉字之间用空格隔开
 * 执行该函数，词条是自动往后排队添加的。
 */
//支持掉电保存
void AsrAddWords(uint8_t idNum, uint8_t words[], uint8_t len)
{	
//	int len = sizeof(words);
	int i;
	//int addr = Asr_Addr;
	IIC_Start(); //起始信号
	IIC_Send_Byte(Asr_Addr); //发送写指令
	IIC_Wait_Ack(); //应答
	IIC_Send_Byte(0xA0); //发送添加词条指令
	IIC_Wait_Ack(); //应答	
	IIC_Send_Byte(idNum); //发送词条编号
	IIC_Wait_Ack(); //应答
	//IIC_Stop();
	
	for(i = 0; i < len; ++i)
	{
		IIC_Send_Byte(words[i]);
		IIC_Wait_Ack(); //应答
	}
 
	IIC_Stop(); //停止
	HAL_Delay_us(20000);
}

//擦除全部词条
void AsrErase(void)
{	
	//WriteOneByte(0xF2, 0x65);
	IIC_Start(); //起始信号
	IIC_Send_Byte(0xF2); //发送写指令
	IIC_Wait_Ack(); //应答
	IIC_Send_Byte(0x65); //发送擦除指令
	IIC_Wait_Ack(); //应答
  IIC_Stop(); //停止
  HAL_Delay_us(60000);
}

//设置识别模式
//识别模式设置，值范围1~3
//1：循环识别模式。状态灯常亮（默认模式）    
//2：口令模式，以第一个词条为口令。状态灯常灭，当识别到口令词时常亮，等待识别到新的语音,并且读取识别结果后即灭掉
//3：按键模式，按下开始识别，不按不识别。支持掉电保存。状态灯随按键按下而亮起，不按不亮
void AsrSetMode(uint8_t mode)
{
	IIC_Start(); //起始信号
	IIC_Send_Byte(Asr_Addr); //发送写指令
	IIC_Wait_Ack(); //应答
	IIC_Send_Byte(0x66); //发送设置模式指令
	IIC_Wait_Ack(); //应答
	IIC_Send_Byte(mode); //发送模式
	IIC_Wait_Ack(); //应答
	IIC_Stop(); //停止	
	HAL_Delay_us(20000);
}

	//获取识别结果
int Asr_Result(void)
{
	int result = 0;
	IIC_Start(); //起始信号
	IIC_Send_Byte(Asr_Addr); //发送写指令
	IIC_Wait_Ack(); //应答
	IIC_Send_Byte(0x64); //识别结果存放处，通过不断读取此地址的值判断是否识别到语音，不同的值对应不同的语音，
	
	if (IIC_Wait_Ack()) //应答
	{
		IIC_Stop(); //停止

		return 0;
		
	}
	
	IIC_Stop(); //停止	
	IIC_Start(); //起始信号
	IIC_Send_Byte(Asr_Read); //发送读指令
	
	IIC_Wait_Ack(); //应答
	
	result = IIC_Read_Byte(0); //读取一个字节
	
	IIC_Stop(); //停止	
	
	//HAL_Delay(20);
	HAL_Delay_us(20000);
	return result;
}
