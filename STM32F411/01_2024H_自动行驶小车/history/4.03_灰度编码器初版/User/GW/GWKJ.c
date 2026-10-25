#include "GWKJ.h"
#include "stm32f4xx_hal.h"
extern I2C_HandleTypeDef hi2c1;
// 读取数据
int8_t hw_i2c_read(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *data, uint8_t data_length)
{
    if (HAL_I2C_Master_Receive(hi2c, dev_addr, data, data_length, HAL_MAX_DELAY) != HAL_OK)
    {
        // 通信错误
        return 1;
    }
    return 0;
}

// 写入数据
int8_t hw_i2c_write(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, const uint8_t *data, uint8_t data_length)
{
    if (HAL_I2C_Master_Transmit(hi2c, dev_addr, (uint8_t*)data, data_length, HAL_MAX_DELAY) != HAL_OK)
    {
        // 通信错误
        return 1;
    }
    return 0;
}

// 读取一个字节
int8_t hw_i2c_read_byte(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *data)
{
    return hw_i2c_read(hi2c, dev_addr, data, 1);
}

// 写入一个字节
int8_t hw_i2c_write_byte(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, const uint8_t data)
{
    return hw_i2c_write(hi2c, dev_addr, &data, 1);
}
// 内存读取
int8_t hw_i2c_mem_read(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t mem_addr, uint8_t *data, uint8_t data_length)
{
    if (HAL_I2C_Mem_Read(hi2c, dev_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, data, data_length, HAL_MAX_DELAY) != HAL_OK)
    {
        return 1;  // 错误
    }
    return 0;
}

// 内存写入
int8_t hw_i2c_mem_write(I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t mem_addr, const uint8_t *data, uint8_t data_length)
{
    if (HAL_I2C_Mem_Write(hi2c, dev_addr, mem_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, data_length, HAL_MAX_DELAY) != HAL_OK)
    {
        return 1;  // 错误
    }
    return 0;
}
// 扫描I2C总线上的设备
uint8_t i2c_scan(I2C_HandleTypeDef *hi2c, uint8_t *scan_addr)
{
    uint8_t count = 0;
    uint8_t data;

    for (uint8_t i = 1; i < 127; ++i) {
        if (hw_i2c_read(hi2c, i << 1, &data, 1) == 0) {
            scan_addr[count] = i;
            ++count;
        }
    }

    return count;
}
void GW_digital_Init(void)
{
    uint8_t ping_response;
    uint8_t scan_addr[128] = {0};
    volatile uint8_t count;

    // 初始化硬件I2C（如果还没做过初始化）

    // 读取设备来清除任何起始条件的问题
    hw_i2c_mem_read(&hi2c1, GW_GRAY_ADDR_DEF, GW_GRAY_PING, &ping_response, 1);
    hw_i2c_mem_read(&hi2c1, GW_GRAY_ADDR_DEF, GW_GRAY_PING, &ping_response, 1);

    // 开始扫描I2C设备
    count = i2c_scan(&hi2c1, scan_addr);

    // 切换到数字模式
    hw_i2c_write_byte(&hi2c1, GW_GRAY_ADDR_DEF, GW_GRAY_DIGITAL_MODE);
}

uint8_t GW_digital_Read(void)
{
    uint8_t digital_data;
    hw_i2c_read_byte(&hi2c1, GW_GRAY_ADDR_DEF, &digital_data);
    return digital_data;
}
uint8_t GW_digital_Read_Bit(uint8_t Num) {
   return GET_NTH_BIT(GW_digital_Read(), Num);
}
void GW_digital_eight(uint8_t *gray_sensor) {
    uint8_t data = GW_digital_Read();
    SEP_ALL_BIT8(data, gray_sensor[0], gray_sensor[1], gray_sensor[2], gray_sensor[3],
                 gray_sensor[4], gray_sensor[5], gray_sensor[6], gray_sensor[7]);
}
	
//#include "GWKJ.h"
//#include "stm32f4xx_hal.h"

//#define I2C_TIMEOUT 100
//#define GRAY_ADDR 0x4C // 7位地址

//extern I2C_HandleTypeDef hi2c1; // 确保已初始化硬件I2C

///******************** 底层I2C驱动 ********************/
//int8_t sw_i2c_write(uint8_t dev_addr, const uint8_t *data, uint8_t len) {
//    return (HAL_I2C_Master_Transmit(&hi2c1, dev_addr<<1, (uint8_t*)data, len, I2C_TIMEOUT) == HAL_OK) ? 0 : 1;
//}

//int8_t sw_i2c_read(uint8_t dev_addr, uint8_t *data, uint8_t len) {
//    return (HAL_I2C_Master_Receive(&hi2c1, dev_addr<<1, data, len, I2C_TIMEOUT) == HAL_OK) ? 0 : 1;
//}

//int8_t sw_i2c_mem_write(uint8_t dev_addr, uint8_t mem_addr, const uint8_t *data, uint8_t len) {
//    return (HAL_I2C_Mem_Write(&hi2c1, dev_addr<<1, mem_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, len, I2C_TIMEOUT) == HAL_OK) ? 0 : 1;
//}

//int8_t sw_i2c_mem_read(uint8_t dev_addr, uint8_t mem_addr, uint8_t *data, uint8_t len) {
//    return (HAL_I2C_Mem_Read(&hi2c1, dev_addr<<1, mem_addr, I2C_MEMADD_SIZE_8BIT, data, len, I2C_TIMEOUT) == HAL_OK) ? 0 : 1;
//}

///********************* 应用层函数 **********************/
//uint8_t i2c_scan(uint8_t *scan_addr) {
//    uint8_t count = 0;
//    for(uint8_t addr = 1; addr < 127; addr++) {
//        if(HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, I2C_TIMEOUT) == HAL_OK) {
//            scan_addr[count++] = addr;
//        }
//    }
//    return count;
//}

//void GW_digital_Init(void) {
//	  uint8_t ping_response;
//    uint8_t scan_addr[128] = {0};
//    uint8_t count;
//		sw_i2c_mem_read(GRAY_ADDR, GW_GRAY_PING, &ping_response, 1); // 首次可能失败
//    HAL_Delay(10);
//    sw_i2c_mem_read(GRAY_ADDR, GW_GRAY_PING, &ping_response, 1); // 二次确保通信
//		
//		count = i2c_scan(scan_addr);
//    if(count == 0) {
//        printf("未发现I2C设备!\n");
//        return;
//    }

//    // 设置数字模式
//    uint8_t mode = GW_GRAY_DIGITAL_MODE;
//		sw_i2c_write(GRAY_ADDR, &mode, 1);
//    if(sw_i2c_write(GRAY_ADDR, &mode, 1)) {
//        printf("数字模式设置失败!\n");
//	}
//}

//uint8_t GW_digital_Read(void) {
//    // 数字模式直接读取1字节状态 (需确认实际数据寄存器地址)
//    uint8_t data;
//    sw_i2c_mem_read(GRAY_ADDR, GW_GRAY_ANALOG_BASE_, &data, 1); // 假设数据寄存器基地址为0xB0
//    return data;
//}

//uint8_t GW_digital_Read_Bit(uint8_t Num) {
//    return GET_NTH_BIT(GW_digital_Read(), Num);
//}

//void GW_digital_eight(uint8_t *gray_sensor) {
//    uint8_t data = GW_digital_Read();
//    SEP_ALL_BIT8(data, gray_sensor[0], gray_sensor[1], gray_sensor[2], gray_sensor[3],
//                 gray_sensor[4], gray_sensor[5], gray_sensor[6], gray_sensor[7]);
//}

//void GW_analog_Init(void) {
//    // 设置模拟模式 (基地址0xB0 + 通道使能)
//    uint8_t ch_enable = GW_GRAY_ANALOG_CH_EN_ALL;
//    sw_i2c_mem_write(GRAY_ADDR, GW_GRAY_ANALOG_CHANNEL_ENABLE, &ch_enable, 1);
//}

//void GW_analog_Read(uint8_t *analog_data) {
//    // 按通道读取8个模拟值
//    for(uint8_t ch=1; ch<=8; ch++) {
//        sw_i2c_mem_read(GRAY_ADDR, GW_GRAY_ANALOG(ch), &analog_data[ch-1], 1);
//    }
//}