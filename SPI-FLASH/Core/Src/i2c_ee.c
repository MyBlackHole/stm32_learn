/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    i2c.c
 * @brief   This file provides code for the configuration
 *          of the I2C instances.
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
#include "i2c_ee.h"

/* USER CODE BEGIN 0 */
#define DATA_Size 256
#define EEP_Firstpage 0x00
uint8_t I2c_Buf_Write[DATA_Size];
uint8_t I2c_Buf_Read[DATA_Size];

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_EE_Init(void) {

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  hi2c1.Init.OwnAddress1 = I2C_OWN_ADDRESS7;
  hi2c1.Init.OwnAddress2 = 0;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (i2cHandle->Instance == I2C1) {
    /* USER CODE BEGIN I2C1_MspInit 0 */

    /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    // /* I2C1 interrupt Init */
    // HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    // HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

    /* USER CODE BEGIN I2C1_MspInit 1 */
    /* Force the I2C peripheral clock reset */
    __HAL_RCC_I2C1_FORCE_RESET();
    /* Release the I2C peripheral clock reset */
    __HAL_RCC_I2C1_RELEASE_RESET();
    /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2cHandle) {

  if (i2cHandle->Instance == I2C1) {
    /* USER CODE BEGIN I2C1_MspDeInit 0 */

    /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    /* USER CODE BEGIN I2C1_MspDeInit 1 */

    /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief   将缓冲区中的数据写到I2C EEPROM中
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 *     @arg NumByteToWrite:写的字节数
 * @retval  无
 */
void I2C_EE_BufferWrite(uint8_t *pBuffer, uint8_t WriteAddr,
                        uint16_t NumByteToWrite) {
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

  /* If WriteAddr is I2C_PageSize aligned  */
  if (Addr == 0) {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0) {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else {
      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr += EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else {
    /* If NumByteToWrite < I2C_PageSize */
    if (NumOfPage == 0) {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else {
      NumByteToWrite -= count;
      NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

      if (count != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;
      }

      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr += EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }
      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
 * @brief   写一个字节到I2C EEPROM中
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 * @retval  无
 */
uint32_t I2C_EE_ByteWrite(uint8_t *pBuffer, uint8_t WriteAddr) {
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, (uint16_t)WriteAddr,
                             I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100);

  /* Check the communication status */
  if (status != HAL_OK) {
    /* Execute user timeout callback */
    // I2Cx_Error(Addr);
  }
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
  }

  /* Check if the EEPROM is ready for a new operation */
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, EEPROM_MAX_TRIALS,
                               I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
    ;

  /* Wait for the end of the transfer */
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
  }
  return status;
}

/**
 * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
 *          不能超过EEPROM页的大小，AT24C02每页有8个字节
 * @param
 *		@arg pBuffer:缓冲区指针
 *		@arg WriteAddr:写地址
 *     @arg NumByteToWrite:写的字节数
 * @retval  无
 */
uint32_t I2C_EE_PageWrite(uint8_t *pBuffer, uint8_t WriteAddr,
                          uint8_t NumByteToWrite) {
  HAL_StatusTypeDef status = HAL_OK;
  /* Write EEPROM_PAGESIZE */
  status =
      HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, WriteAddr, I2C_MEMADD_SIZE_8BIT,
                        (uint8_t *)(pBuffer), NumByteToWrite, 100);

  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
  }

  /* Check if the EEPROM is ready for a new operation */
  while (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDRESS, EEPROM_MAX_TRIALS,
                               I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
    ;

  /* Wait for the end of the transfer */
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
  }
  return status;
}

/**
 * @brief   从EEPROM里面读取一块数据
 * @param
 *		@arg pBuffer:存放从EEPROM读取的数据的缓冲区指针
 *		@arg WriteAddr:接收数据的EEPROM的地址
 *     @arg NumByteToWrite:要从EEPROM读取的字节数
 * @retval  无
 */
uint32_t I2C_EE_BufferRead(uint8_t *pBuffer, uint8_t ReadAddr,
                           uint16_t NumByteToRead) {
  HAL_StatusTypeDef status = HAL_OK;

  status =
      HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, ReadAddr, I2C_MEMADD_SIZE_8BIT,
                       (uint8_t *)pBuffer, NumByteToRead, 1000);

  return status;
}

/**
 * @brief  I2C(AT24C02)读写测试
 * @param  无
 * @retval 正常返回1 ，不正常返回0
 */
uint8_t I2C_Test(void) {
  uint16_t i;

  EEPROM_INFO("写入的数据");

  // 填充缓冲
  for (i = 0; i < DATA_Size; i++) 
  {
    if (i % 16 == 0)
      printf("\n\r");
    I2c_Buf_Write[i] = i;
    printf("0x%02X ", I2c_Buf_Write[i]);
  }

  // 将I2c_Buf_Write中顺序递增的数据写入EERPOM中
  I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, DATA_Size);

  EEPROM_INFO("读出的数据");
  // 将EEPROM读出数据顺序保持到I2c_Buf_Read中
  I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, DATA_Size);
  // 将I2c_Buf_Read中的数据通过串口打印
  for (i = 0; i < DATA_Size; i++) {
    if (I2c_Buf_Read[i] != I2c_Buf_Write[i]) {
      printf("0x%02X ", I2c_Buf_Read[i]);
      EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致");
      return 0;
    }
    if (i % 16 == 0)
      printf("\n\r");
    printf("0x%02X ", I2c_Buf_Read[i]);
  }
  EEPROM_INFO("I2C(AT24C02)读写测试成功");
  return 1;
}
/* USER CODE END 1 */
