#include "TCS.h"
/**
  * @brief  TCS34725 Enable Function.
  * @retval None
  */
void TCS34725_ENABLE()
{
	uint8_t buffer=0; 
	HAL_I2C_Mem_Read(&hi2c1,  I2C_TCS34725_ADDR<<1|I2C_READ, TCS34725_ID_ADDR|TCS34725_CMD_MASK, 1,&buffer,1,500);//读取ID  
  buffer = TCS34725_ENABLE_PON;//使能颜色传感器PON
	HAL_I2C_Mem_Write(&hi2c1,I2C_TCS34725_ADDR<<1,  TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK,1,&buffer,1,300);
  HAL_Delay(5);
  HAL_I2C_Mem_Read(&hi2c1,I2C_TCS34725_ADDR<<1|I2C_READ,TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK,1,&buffer,1,300);//读取Enable状态
  buffer |= 0x2; 
  HAL_I2C_Mem_Write(&hi2c1,I2C_TCS34725_ADDR<<1,TCS34725_ENABLE_ADDR|TCS34725_CMD_MASK,1,&buffer,1,300);//使能AEN
}
/**
  * @brief  Get the RAW data of the specified color.
  * @param  hi2c:Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  MemAddress: Internal memory address
  * @retval the 16 bits RAW data 
  */
uint16_t TCS34725_Get_RawData(I2C_HandleTypeDef *hi2c,uint16_t MemAddress)
{
	uint16_t tempRawValue=0;
	uint8_t buffer[2]={0};
	HAL_I2C_Mem_Read(hi2c,I2C_TCS34725_ADDR<<1|I2C_READ,MemAddress,1,buffer,2,1000);
	tempRawValue = buffer[1]<<8|buffer[0];
	return  tempRawValue;
}

/**
  * @brief  Get the RAW data of the specified color.
  * @param  RGBc: the char that define the RGBC 
  * @param  RawData: the 16 bits RAW data
  * @retval None
  */
void Send_RAWData_To_UART(char RGBC, uint16_t RawData)
{
	char value[9];
	value[0]=RGBC;
	value[1]='-';
	for( int j=6;j>1;j--)
	{
			value[j]= 48+RawData%10;
			RawData/=10;
	}
	value[7]='\r';
	value[8]='\n';
	HAL_UART_Transmit(&huart2,(uint8_t*)value,9,10000);	
}
