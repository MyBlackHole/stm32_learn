/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    i2c.h
 * @brief   This file contains all the function prototypes for
 *          the i2c.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */

/* AT24C01/02每页有8个字节 */
//#define EEPROM_PAGESIZE    8
#define EEPROM_PAGESIZE 	   8
/* AT24C04/08A/16A每页有16个字节 */
//#define EEPROM_PAGESIZE           16			

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7      0X0A   

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX 300

#define EEPROM_MAX_TRIALS 300

/*信息输出*/
#define EEPROM_DEBUG_ON 0

#define EEPROM_INFO(fmt, arg...) printf("\r\n<<-EEPROM-INFO->> " fmt "\n", ##arg)
#define EEPROM_ERROR(fmt, arg...) printf("\r\n<<-EEPROM-ERROR->> " fmt "\n", ##arg)
#define EEPROM_DEBUG(fmt, arg...)                                              \
  do {                                                                         \
    if (EEPROM_DEBUG_ON)                                                       \
      printf("\r\n<<-EEPROM-DEBUG->> [%d]" fmt "\n", __LINE__, ##arg);             \
  } while (0)

/*
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1
 */

/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0 /* E2 = 0 */
#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

#define EEPROM_ADDRESS 0xA0
/* USER CODE END Private defines */
uint8_t I2C_Test(void);

void MX_I2C1_EE_Init(void);
void I2C_EE_BufferWrite(uint8_t *pBuffer, uint8_t WriteAddr,
                        uint16_t NumByteToWrite);
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer, uint8_t WriteAddr);
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer, uint8_t WriteAddr,
                          uint8_t NumByteToWrite);
uint32_t I2C_EE_BufferRead(uint8_t *pBuffer, uint8_t ReadAddr,
                           uint16_t NumByteToRead);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */
