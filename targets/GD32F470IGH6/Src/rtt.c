/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Timer Driver Initialization Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-03-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "rtt.h"
#include "toolkit.h"
#include "usart.h"
#include "los_task_pri.h"

#define RTT_RECV_BUF_SIZE 64

void RTTHandle(void) {
  UINT8 buff[RTT_RECV_BUF_SIZE];
  UINT32 recvLen = 0;
  uint32_t USARTx = USART5; // 默认5号串口

  while (1) {
    /* Wait until data is available */
    while (!SEGGER_RTT_HasKey()) {
      LOS_TaskDelay(1000);
    }

    /* Read all available characters at once */
    recvLen = SEGGER_RTT_Read(0, buff, RTT_RECV_BUF_SIZE);
    if (recvLen == 0) {
      continue;
    }

    /* Echo back received characters */
    SEGGER_RTT_printf_string(buff, recvLen);
    SEGGER_RTT_printf(0, "%s recvLen = %d\n", __func__, recvLen);
    
    //+++切换透传串口
    if (recvLen == 4 && buff[0] == 0x2B && buff[1] == 0x2B && buff[2] == 0x2B &&
        buff[3] == 0x2B) {
      USARTx = buff[4];
      continue;
    }

    switch (USARTx) {
    case USART0:
      PassthroughUSART0(buff, recvLen);
      break;
    case USART1:
      PassthroughUSART1(buff, recvLen);
      break;
    case USART2:
      PassthroughUSART2(buff, recvLen);
      break;
    case UART3:
      PassthroughUSART3(buff, recvLen);
      break;
    case UART4:
      PassthroughUSART4(buff, recvLen);
      break;
    case USART5:
      PassthroughUSART5(buff, recvLen);
      break;
    case UART6:
      PassthroughUART6(buff, recvLen);
      break;
    default:
      break;
    }
    //透传数据
    Seria_SendArray(USARTx,buff,recvLen);
  }
}

void PassthroughUSART0(uint8_t *array, uint32_t length) {
  Seria_SendArray(USART0, array, length);
}

void PassthroughUSART1(uint8_t *array, uint32_t length) {
  Seria_SendArray(USART1, array, length);
}

void PassthroughUSART2(uint8_t *array, uint32_t length) {
  Seria_SendArray(USART2, array, length);
}

void PassthroughUSART3(uint8_t *array, uint32_t length) {
  Seria_SendArray(UART3, array, length);
}

void PassthroughUSART4(uint8_t *array, uint32_t length) {
  Seria_SendArray(UART4, array, length);
}
void PassthroughUSART5(uint8_t *array, uint32_t length) {
  // AT指令添加\r\n后缀
  array[length] = '\r';
  array[length + 1] = '\n';
  length += 2;
  Seria_SendArray(USART5, array, length);
}
void PassthroughUART6(uint8_t *array, uint32_t length) {
  Seria_SendArray(UART6, array, length);
}