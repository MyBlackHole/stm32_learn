/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"
#include <errno.h>
#include <stdio.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

#ifdef __GNUC__
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
// int _isatty(int fd) {
//   if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
//     return 1;

//   errno = EBADF;
//   return 0;
// }

int _write(int fd, char *ptr, int len) {
  HAL_StatusTypeDef hstatus;

  if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
    hstatus = HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return len;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

// int _close(int fd) {
//   if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
//     return 0;

//   errno = EBADF;
//   return -1;
// }

// int _lseek(int fd, int ptr, int dir) {
//   (void)fd;
//   (void)ptr;
//   (void)dir;

//   errno = EBADF;
//   return -1;
// }

int _read(int fd, char *ptr, int len) {
  HAL_StatusTypeDef hstatus;

  if (fd == STDIN_FILENO) {
    hstatus = HAL_UART_Receive(&huart1, (uint8_t *)ptr, 1, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return 1;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

// int _fstat(int fd, struct stat *st) {
//   if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
//     st->st_mode = S_IFCHR;
//     return 0;
//   }

//   errno = EBADF;
//   return 0;
// }
#endif

// /**
//   * 函数功能: 重定向c库函数printf到DEBUG_USARTx
//   * 输入参数: 无
//   * 返 回 值: 无
//   * 说    明：无
//   */
// int fputc(int ch, FILE *f)
// {
//   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
//   return ch;
// }
//
// /**
//   * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
//   * 输入参数: 无
//   * 返 回 值: 无
//   * 说    明：无
//   */
// int fgetc(FILE *f)
// {
//   uint8_t ch = 0;
//   HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
//   return ch;
// }

void MX_USART1_UART_Init(void) {

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
  // // 开启串口接收中断
  // __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (uartHandle->Instance == USART1) {
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspInit 1 */

    /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle) {

  if (uartHandle->Instance == USART1) {
    /* USER CODE BEGIN USART1_MspDeInit 0 */

    /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */