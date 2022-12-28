/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.h
 * @brief   This file contains all the function prototypes for
 *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */

/* Private typedef -----------------------------------------------------------*/
// #define  sFLASH_ID                       0xEF3015     //W25X16
// #define  sFLASH_ID                       0xEF4015	    //W25Q16
#define sFLASH_ID 0XEF4017 // W25Q64
// #define  sFLASH_ID                       0XEF4018    //W25Q128

// #define SPI_FLASH_PageSize            4096
#define SPI_FLASH_PageSize 256
#define SPI_FLASH_PerWritePageSize 256

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

#define WIP_Flag 0x01 /* Write In Progress (WIP) flag */
#define Dummy_Byte 0x9F

#define SPI_FLASH_CS_LOW()                                                     \
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET) // 输出低电平
#define SPI_FLASH_CS_HIGH()                                                    \
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET) // 设置为高电平

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*信息输出*/
#define FLASH_DEBUG_ON 1

#define FLASH_INFO(fmt, arg...) printf("\r\n<<-FLASH-INFO->> " fmt "\n", ##arg)
#define FLASH_ERROR(fmt, arg...)                                               \
  printf("\r\n<<-FLASH-ERROR->> " fmt "\n", ##arg)
#define FLASH_DEBUG(fmt, arg...)                                               \
  do {                                                                         \
    if (FLASH_DEBUG_ON)                                                        \
      printf("\r\n<<-FLASH-DEBUG->> [%d]" fmt "\n", __LINE__, ##arg);          \
  } while (0)

/* USER CODE END Private defines */
void SPI_Test(void);

void MX_SPI1_Init(void);
// void SPI_FLASH_Init(void);

void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr,
                         uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr,
                           uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr,
                          uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
