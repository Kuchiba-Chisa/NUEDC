#include "main.h"
#include "gw_grayscale_sensor.h"
#include "debug.h"

/* 启用传感器自带模拟量归一化(v3.6+固件) */
#define USE_GW_GRAY_NORMALIZE 0

#define GW_GRAY_ADDR GW_GRAY_ADDR_DEF

extern I2C_HandleTypeDef hi2c1;


void Grey_Sensor(void)
{
		volatile int ret_transmit, ret_receive;
		uint8_t recv_value = 0;
		uint8_t i2c_cmd;
		uint8_t sensor_arr[8];


#if (USE_GW_GRAY_NORMALIZE)
	uint8_t write_data = 0xFF;
	ret_transmit = HAL_I2C_Mem_Write(&hi2c1,
	                        GW_GRAY_ADDR << 1, GW_GRAY_ANALOG_NORMALIZE, 1,
	                        &write_data, 1, 100);
	if (ret_transmit != HAL_OK) {
		// i2c 错误
	}
#endif

	/* 设置模拟数据模式, 设置完毕之后, 循环读取8个探头的模拟数据 */
	i2c_cmd = GW_GRAY_ANALOG_MODE;
	ret_transmit = HAL_I2C_Master_Transmit(&hi2c1, GW_GRAY_ADDR << 1, &i2c_cmd, 1, 100);
	if (ret_transmit != HAL_OK) {
		DEBUG_info("Grey","Fault");
	}
	
	
		ret_receive = HAL_I2C_Master_Receive(&hi2c1, GW_GRAY_ADDR << 1, sensor_arr, 8, 100);
 		if (ret_receive) {
			DEBUG_info("Grey","Fault"); //i2c 错误
		
		// sensor_arr[0] 是探头1
		// ...
		// sensor_arr[7] 是探头8
		DEBUG_info("Grey_Sensor",":%d,%d,%d",sensor_arr[0],sensor_arr[1],sensor_arr[2]);
		HAL_Delay(1);
	}
}

