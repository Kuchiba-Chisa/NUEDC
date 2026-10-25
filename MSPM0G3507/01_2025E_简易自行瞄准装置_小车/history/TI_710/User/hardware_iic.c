#include "hardware_iic.h"
unsigned char IIC_ReadByte(unsigned char Salve_Adress,unsigned char Reg_Address)
{
	return hardware_IIC_ReadByte(Salve_Adress,Reg_Address);
}
unsigned char IIC_ReadBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *Result,unsigned char len)
{
	return hardware_IIC_ReadBytes(Salve_Adress,Reg_Address,Result,len);
}
unsigned char IIC_WriteByte(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char data)
{
	return hardware_IIC_WirteByte(Salve_Adress,Reg_Address,data);
}
unsigned char IIC_WriteBytes(unsigned char Salve_Adress,unsigned char Reg_Address,unsigned char *data,unsigned char len)
{
	return hardware_IIC_WirteBytes(Salve_Adress,Reg_Address,data,len);
}
unsigned char Ping(void)
{
	unsigned char dat;
	dat=IIC_ReadByte(GW_GRAY_ADDR_DEF,GW_GRAY_PING);
	if(dat==GW_GRAY_PING_OK)
	{
			return 0;
	}	
	else return 1;
}
unsigned char IIC_Get_Digtal(void)
{
	unsigned char dat;
	dat=IIC_ReadByte(GW_GRAY_ADDR_DEF,GW_GRAY_DIGITAL_MODE);
	return dat;
}
unsigned char IIC_Get_Anolog(unsigned char * Result,unsigned char len)
{
	if(IIC_ReadBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG_BASE_,Result,len))return 1;
	else return 0;
}
unsigned char IIC_Get_Single_Anolog(unsigned char Channel)
{
	unsigned char dat;
	IIC_ReadBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG(Channel),&dat,1);
	return dat;
}
unsigned char IIC_Get_Normalize(unsigned char * Result,unsigned char len)
{
	if(IIC_ReadBytes(GW_GRAY_ADDR_DEF,GW_GRAY_ANALOG_NORMALIZE,Result,len))return 1;
	else return 0;
}
unsigned short IIC_Get_Offset(void )
{
	unsigned char dat[2]={0};
	IIC_ReadBytes(GW_GRAY_ADDR_DEF,Offset,dat,2);
	return (unsigned short)dat[0]|(unsigned short)dat[1]<<8;
}

