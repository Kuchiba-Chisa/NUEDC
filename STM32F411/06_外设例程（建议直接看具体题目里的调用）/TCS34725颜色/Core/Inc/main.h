/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define I2C_READ 0x01
#define TCS34725_ENABLE_PON  0x01  //PON ENABLE
#define TCS34725_ENABLE_AEN  0x02  //AEN ENAGLE
#define TCS34725__STATUS_AVALID  0x01 //RGBC avalid
#define TCS34725_CMD_MASK    0x80 //Command Register

#define I2C_TCS34725_ADDR    0x29  //TCS34725's address 

#define TCS34725_ENABLE_ADDR 0x00  //Enable register's address
#define TCS34725_ID_ADDR     0x12  //ID register's address
#define TCS34725_STATUS_ADDR 0x13  //Status register's address
#define TCS34725_CDATA_ADDR  0x14  //Clear data low byte
#define TCS34725_CDATAH_ADDR 0x15  //Clear data high byte
#define TCS34725_RDATA_ADDR  0x16  //Red data low byte
#define TCS34725_RDATAH_ADDR 0x17  //Red data high byte
#define TCS34725_GDATA_ADDR  0x18  //Green data low byte
#define TCS34725_GDATAH_ADDR 0x19  //Green data high byte
#define TCS34725_BDATA_ADDR  0x1A  //Blue data low byte
#define TCS34725_BDATAH_ADDR 0x1B  //Blue data high byte

#define VL6180X_DEFAULT_ID         0xB4
#define VL6180X_DEFAULT_I2C_ADDR   0x29
#define VL6180X_REG_ID             0x000

#define VL6180X_REG_SYSTERM_INTERRUPT_CLEAR 0x15
#define VL6180X_REG_SYSRANGE_START 0x018  //2byte address
#define VL6180X_REG_RESULT_RANGE_STATUS   0x04D
#define VL6180X_REG_RESULT_INTERRUPT_STATUS 0x04F
#define VL6180X_REG_RESULT_RANGE_VAL 0x062

#define VL6180X_REG_ADDRESS_SIZE   2

#define VL6180X_ERROR_NONE         0  //no error
#define VL6180X_ERROR_SYSERR_1     1  //system error 1
#define VL6180X_ERROR_SYSERR_5     5  //system error 5
#define VL6180X_ERROR_ECEFAIL      6  //early convergence estimate fail
#define VL6180X_ERROR_NOCONVERGE   7  //no target detected
#define VL6180X_ERROR_RANGEIGNORE  8  //ignore threshold check failed
#define VL6180X_ERROR_SNR          11 //ambient condition too high
#define VL6180X_ERROR_RAWFLOW      12 //raw range algo underflow
#define VL6180X_ERROR_RAEUFLOW     13 //raw range algo overflow
#define VL6180X_ERROR_RANGEFLOW    14 //ranging algo underflow
#define VL6180X_ERROR_RANGEUFLOW   15 //ranging algo overflow
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
