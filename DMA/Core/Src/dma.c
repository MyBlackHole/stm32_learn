#include "main.h"
#include "stm32f1xx_hal.h"
#include <dma.h>
#include <stdio.h>

/* 相关宏定义，使用存储器到存储器传输必须使用DMA1 */
DMA_HandleTypeDef DMA_Handle;
extern UART_HandleTypeDef huart1;
#define BUFFER_SIZE 32
uint8_t SendBuff[SENDBUFF_SIZE];

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
  const关键字将aSRC_Const_Buffer数组变量定义为常量类型 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] = {
    0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10, 0x11121314, 0x15161718,
    0x191A1B1C, 0x1D1E1F20, 0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
    0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40, 0x41424344, 0x45464748,
    0x494A4B4C, 0x4D4E4F50, 0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
    0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70, 0x71727374, 0x75767778,
    0x797A7B7C, 0x7D7E7F80};
/* 定义DMA传输目标存储器 */
uint32_t aDST_Buffer[BUFFER_SIZE];

uint8_t Buffercmp(const uint32_t *pBuffer, uint32_t *pBuffer1,
                  uint16_t BufferLength) {
  /* 数据长度递减 */
  while (BufferLength--) {
    /* 判断两个数据源是否对应相等 */
    if (*pBuffer != *pBuffer1) {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1;
}

/**
 * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
 * @param  无
 * @retval 无
 */
void MX_DMA_Init(void) {
  __HAL_RCC_DMA1_CLK_ENABLE();

  // 数据流选择
  DMA_Handle.Instance = DMA1_Channel4;
  // 存储器到外设HAL_DMA_Init(&DMA_Handle);
  DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  // 外设非增量模式/* Associate the DMA handle */
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
  // 存储器增量模式__HAL_LINKDMA(&UartHandle, hdmatx, DMA_Handle);
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
  // 外设数据长度:8位
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  // 存储器数据长度:8位
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  // 外设普通模式
  DMA_Handle.Init.Mode = DMA_NORMAL;
  // 中等优先级
  DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;

  HAL_DMA_Init(&DMA_Handle);
  /* Associate the DMA handle */
  __HAL_LINKDMA(&huart1, hdmatx, DMA_Handle);
}

void DMA_Test(void) {
  printf("\r\n USART1 DMA TX 测试 \r\n");

  /*填充将要发送的数据*/
  for (int i = 0; i < SENDBUFF_SIZE; i++) {
    SendBuff[i] = 'B';
  }

  /*为演示DMA持续运行而CPU还能处理其它事情，持续使用DMA发送数据，量非常大，
   *长时间运行可能会导致电脑端串口调试助手会卡死，鼠标乱飞的情况，
   *或把DMA配置中的循环模式改为单次模式*/

  /* USART1 向 DMA发出TX请求 */
  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)SendBuff, SENDBUFF_SIZE);
}

// // 内存到内存
// void MX_DMA_Init(void) {
//   __HAL_RCC_DMA1_CLK_ENABLE();
//   // 数据流选择
//   DMA_Handle.Instance = DMA1_Channel6;
//   // 存储器到外设HAL_DMA_Init(&DMA_Handle);
//   DMA_Handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
//   // 外设非增量模式/* Associate the DMA handle */
//   DMA_Handle.Init.PeriphInc = DMA_PINC_ENABLE;
//   // 存储器增量模式__HAL_LINKDMA(&UartHandle,hdmatx, DMA_Handle);
//   DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
//   // 外设数据长度:8位
//   DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
//   // 存储器数据长度:8位
//   DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
//   // 外设普通模式
//   DMA_Handle.Init.Mode = DMA_NORMAL;
//   // 中等优先级
//   DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;

//   /* 完成DMA数据流参数配置 */
//   HAL_DMA_Init(&DMA_Handle);
// }

// void DMA_Test(void) {
//   /* 定义存放比较结果变量 */
//   uint8_t TransferStatus;
//   HAL_StatusTypeDef DMA_status = HAL_ERROR;
//   DMA_status = HAL_DMA_Start(&DMA_Handle, (uint32_t)aSRC_Const_Buffer,
//                              (uint32_t)aDST_Buffer, BUFFER_SIZE);

//   /* 判断DMA状态 */
//   if (DMA_status != HAL_OK) {
//     /* DMA出错就让程序运行下面循环：打印状态 */
//     while (1) {
//       printf("DMA_status:[%d]\n", DMA_status);
//       HAL_Delay(0xFFFFFF);
//     }
//   }

//   /* 等待DMA传输完成 */
//   while (__HAL_DMA_GET_FLAG(&DMA_Handle, DMA_FLAG_TC6) == RESET) {
//   }

//   /* 比较源数据与传输后数据 */
//   TransferStatus = Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);

//   /* 判断源数据与传输后数据比较结果*/
//   if (TransferStatus == 0) {
//     /* 源数据与传输后数据不相等 */
//     printf("源数据与传输后数据不相等:[%d]\n", TransferStatus);
//   } else {
//     /* 源数据与传输后数据相等 */
//     printf("源数据与传输后数据相等:[%d]\n", TransferStatus);
//   }
// }
