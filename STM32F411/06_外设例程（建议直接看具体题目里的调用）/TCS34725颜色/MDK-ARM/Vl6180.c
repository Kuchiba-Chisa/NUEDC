#include "Vl6180.h"

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
		VL6180X_WriteByte(0x0014, 0x24); // Configures interrupt on new Sample Ready threshold event?
		
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