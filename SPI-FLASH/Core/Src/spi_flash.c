/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
/* Includes ------------------------------------------------------------------*/
#include "spi_flash.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN 0 */
/* 发送缓冲区初始化 */
typedef enum { FAILED = 0, PASSED = !FAILED } TestStatus;
static __IO uint32_t SPITimeout = SPIT_LONG_TIMEOUT;
/* 获取缓冲区的长度 */
#define TxBufferSize1 (countof(TxBuffer1) - 1)
#define RxBufferSize1 (countof(TxBuffer1) - 1)
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define BufferSize (countof(Tx_Buffer) - 1)

#define FLASH_WriteAddress 0x00000
#define FLASH_ReadAddress FLASH_WriteAddress
#define FLASH_SectorToErase FLASH_WriteAddress

// 读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;
uint8_t Tx_Buffer[] = "感谢您选用野火stm32开发板教程, [BlackHole]";
uint8_t Rx_Buffer[BufferSize];

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);
TestStatus SPIBuffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2,
                        uint16_t BufferLength);

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void) {

  /* USER CODE BEGIN SPI1_Init 0 */

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.Mode = SPI_MODE_MASTER;

  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */
  // 使能外设
  __HAL_SPI_ENABLE(&hspi1);
  /* USER CODE END SPI1_Init 2 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (spiHandle->Instance == SPI1) {
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
    /* USER CODE BEGIN SPI1_MspInit 1 */

    /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {

  if (spiHandle->Instance == SPI1) {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */

    /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    /* SPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
    /* USER CODE BEGIN SPI1_MspDeInit 1 */

    /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief  擦除FLASH扇区
 * @param  SectorAddr：要擦除的扇区地址
 * @retval 无
 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr) {
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 发送扇区擦除指令*/
  SPI_FLASH_SendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
 * @brief  擦除FLASH扇区，整片擦除
 * @param  无
 * @retval 无
 */
void SPI_FLASH_BulkErase(void) {
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整块 Erase */
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 发送整块擦除指令*/
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
 * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
 * @retval 无
 */
void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr,
                         uint16_t NumByteToWrite) {
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();
  /* 写页写指令*/
  SPI_FLASH_SendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if (NumByteToWrite > SPI_FLASH_PerWritePageSize) {
    NumByteToWrite = SPI_FLASH_PerWritePageSize;
    FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* 写入数据*/
  while (NumByteToWrite--) {
    /* 发送当前要写入的字节数据 */
    SPI_FLASH_SendByte(*pBuffer);
    /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param  WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度
 * @retval 无
 */
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr,
                           uint16_t NumByteToWrite) {
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  /*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;

  /*差count个数据值，刚好可以对齐到页地址*/
  count = SPI_FLASH_PageSize - Addr;
  /*计算出要写多少整数页*/
  NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
  /*mod运算求余，计算出剩余不满一页的字节数*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
  if (Addr == 0) {
    /* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    } else {
      /* NumByteToWrite > SPI_FLASH_PageSize */
      /*先把整数页都写了*/
      while (NumOfPage--) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr += SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      /*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  /* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else {
    /* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) {
      /*当前页剩余的count个位置比NumOfSingle小，写不完*/
      if (NumOfSingle > count) {
        temp = NumOfSingle - count;

        /*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;

        /*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      } else {
        /*当前页剩余的count个位置能写完NumOfSingle个数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    } else {
      /* NumByteToWrite > SPI_FLASH_PageSize */
      /*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr += count;
      pBuffer += count;

      /*把整数页都写了*/
      while (NumOfPage--) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr += SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
      /*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
 * @brief  读取FLASH数据
 * @param 	pBuffer，存储读出数据的指针
 * @param   ReadAddr，读取地址
 * @param   NumByteToRead，读取数据长度
 * @retval 无
 */
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr,
                          uint16_t NumByteToRead) {
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();

  /* 发送 读 指令 */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  /* 读取数据 */
  while (NumByteToRead--) {
    /* 读取一个字节*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
}

/**
 * @brief  读取FLASH ID
 * @param 	无
 * @retval FLASH ID
 */
uint32_t SPI_FLASH_ReadID(void) {
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 开始通讯：CS低电平 */
  SPI_FLASH_CS_LOW();

  /* 发送JEDEC指令，读取ID */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* 读取一个字节数据 */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 停止通讯：CS高电平 */
  SPI_FLASH_CS_HIGH();

  /*把数据组合起来，作为函数的返回值*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
 * @brief  读取FLASH Device ID
 * @param 	无
 * @retval FLASH Device ID
 */
uint32_t SPI_FLASH_ReadDeviceID(void) {
  uint32_t Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_DeviceID);
  // SPI_FLASH_SendByte(W25X_ManufactDeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return Temp;
}
/*******************************************************************************
 * Function Name  : SPI_FLASH_StartReadSequence
 * Description    : Initiates a read data byte (READ) sequence from the Flash.
 *                  This is done by driving the /CS line low to select the
 *device, then the READ instruction is transmitted followed by 3 bytes address.
 *This function exit and keep the /CS line low, so the Flash still being
 *selected. With this technique the whole content of the Flash is read with a
 *single READ instruction. Input          : - ReadAddr : FLASH's internal
 *address to read from. Output         : None Return         : None
 *******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr) {
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from
   * -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/**
 * @brief  使用SPI读取一个字节的数据
 * @param  无
 * @retval 返回接收到的数据
 */
uint8_t SPI_FLASH_ReadByte(void) { return (SPI_FLASH_SendByte(Dummy_Byte)); }

/**
 * @brief  使用SPI发送一个字节的数据
 * @param  byte：要发送的数据
 * @retval 返回接收到的数据
 */
uint8_t SPI_FLASH_SendByte(uint8_t byte) {
  // SPITimeout = SPIT_FLAG_TIMEOUT;

  // /* 等待发送缓冲区为空，TXE事件 */
  // while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET) {
  //   if ((SPITimeout--) == 0)
  //     return SPI_TIMEOUT_UserCallback(0);
  // }

  // /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  // WRITE_REG(hspi1.Instance->DR, byte);

  // SPITimeout = SPIT_FLAG_TIMEOUT;

  // /* 等待接收缓冲区非空，RXNE事件 */
  // while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) == RESET) {
  //   if ((SPITimeout--) == 0)
  //     return SPI_TIMEOUT_UserCallback(1);
  // }

  // /* 读取数据寄存器，获取接收缓冲区数据 */
  // return READ_REG(hspi1.Instance->DR);

	uint8_t ret;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &ret, 1, 100);
	return ret;
}

/*******************************************************************************
 * Function Name  : SPI_FLASH_SendHalfWord
 * Description    : Sends a Half Word through the SPI interface and return the
 *                  Half Word received from the SPI bus.
 * Input          : Half Word : Half Word to send.
 * Output         : None
 * Return         : The value of the received Half Word.
 *******************************************************************************/
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord) {

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Loop while DR register in not emplty */
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE) == RESET) {
    if ((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(2);
  }

  /* Send Half Word through the SPIx peripheral */
  WRITE_REG(hspi1.Instance->DR, HalfWord);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Wait to receive a Half Word */
  while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE) == RESET) {
    if ((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(3);
  }
  /* Return the Half Word read from the SPI bus */
  return READ_REG(hspi1.Instance->DR);
}

/**
 * @brief  向FLASH发送 写使能 命令
 * @param  none
 * @retval none
 */
void SPI_FLASH_WriteEnable(void) {
  /* 通讯开始：CS低 */
  SPI_FLASH_CS_LOW();

  /* 发送写使能命令*/
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /*通讯结束：CS高 */
  SPI_FLASH_CS_HIGH();
}

/**
 * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
 * @param  none
 * @retval none
 */
void SPI_FLASH_WaitForWriteEnd(void) {
  uint8_t FLASH_Status = 0;

  /* 选择 FLASH: CS 低 */
  SPI_FLASH_CS_LOW();

  /* 发送 读状态寄存器 命令 */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 若FLASH忙碌，则等待 */
  do {
    /* 读取FLASH芯片的状态寄存器 */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);

    {
      if ((SPITimeout--) == 0) {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    }
  } while ((FLASH_Status & WIP_Flag) == SET); /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
  SPI_FLASH_CS_HIGH();
}

// 进入掉电模式
void SPI_Flash_PowerDown(void) {
  /* 选择 FLASH: CS 低 */
  SPI_FLASH_CS_LOW();

  /* 发送 掉电 命令 */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* 停止信号  FLASH: CS 高 */
  SPI_FLASH_CS_HIGH();
}

// 唤醒
void SPI_Flash_WAKEUP(void) {
  /*选择 FLASH: CS 低 */
  SPI_FLASH_CS_LOW();

  /* 发上 上电 命令 */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* 停止信号 FLASH: CS 高 */
  SPI_FLASH_CS_HIGH(); // 等待TRES1
}

/**
 * @brief  等待超时回调函数
 * @param  None.
 * @retval None.
 */
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode) {
  /* 等待超时后的处理,输出错误信息 */
  FLASH_ERROR("SPI 等待超时!errorCode = %d", errorCode);
  return 0;
}

void SPI_Test(void) {

  for (;FlashID != sFLASH_ID;) {
    HAL_Delay(1000);
  /* 获取 Flash Device ID */
    DeviceID = SPI_FLASH_ReadDeviceID();
    HAL_Delay(1000);
    /* 获取 SPI Flash ID */
    FlashID = SPI_FLASH_ReadID();

    printf("\r\nFlashID is 0x%lX,  Manufacturer Device ID is 0x%lX\r\n", FlashID,
           DeviceID);
  }

  printf("\r\nFlashID is 0x%lX,  Manufacturer Device ID is 0x%lX\r\n", FlashID,
         DeviceID);

  /* 检验 SPI Flash ID */
  // if (1) {
  if (FlashID == sFLASH_ID) {
    printf("\r\n检测到SPI FLASH W25Q64 !\r\n");

    /* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
    SPI_FLASH_SectorErase(FLASH_SectorToErase);

    /* 将发送缓冲区的数据写到flash中 */
    SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
    printf("\r\n写入的数据为：\r\n[%s]", Tx_Buffer);

    /* 将刚刚写入的数据读出来放到接收缓冲区中 */
    SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
    printf("\r\n读出的数据为：\r\n[%s]", Rx_Buffer);

    /* 检查写入的数据与读出的数据是否相等 */
    TransferStatus1 = SPIBuffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

    if (PASSED == TransferStatus1) {
      printf("\r\n16M串行flash(W25Q64)测试成功!\n\r");
    } else {
      printf("\r\n16M串行flash(W25Q64)测试失败!\n\r");
    }
  } else {
    // if (FlashID == sFLASH_ID)
    printf("\r\n获取不到 W25Q64 ID!\n\r");
  }

  SPI_Flash_PowerDown();
}

/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
 */
TestStatus SPIBuffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2,
                        uint16_t BufferLength) {
  while (BufferLength--) {
    if (*pBuffer1 != *pBuffer2) {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}
/* USER CODE END 1 */
