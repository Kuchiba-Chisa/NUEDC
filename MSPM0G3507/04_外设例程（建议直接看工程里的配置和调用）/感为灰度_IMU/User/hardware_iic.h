#include "IIC.h"
#include "gw_grayscale_sensor.h"
unsigned char Ping(void);
unsigned char IIC_Get_Digtal(void);
unsigned char IIC_Get_Anolog(unsigned char * Result,unsigned char len);
unsigned char IIC_Get_Single_Anolog(unsigned char Channel);
unsigned char IIC_Get_Normalize(unsigned char * Result,unsigned char len);
unsigned short IIC_Get_Offset(void );

