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

#include "queue.h"
#include "los_hwi.h"
#include "los_queue.h"

#define UART_QUEUE_SIZE        64
#define UART_QUEUE_BUF_MAX_LEN 1
#define UART_QUEUE_REC_DELAY   5

void QueueInit(void)
{
    LOS_QueueCreate("q0", UART_QUEUE_SIZE, &g_queueId_0, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q1", UART_QUEUE_SIZE, &g_queueId_1, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q2", UART_QUEUE_SIZE, &g_queueId_2, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q3", UART_QUEUE_SIZE, &g_queueId_3, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q4", UART_QUEUE_SIZE, &g_queueId_4, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q5", UART_QUEUE_SIZE, &g_queueId_5, 0, UART_QUEUE_BUF_MAX_LEN);
    LOS_QueueCreate("q6", UART_QUEUE_SIZE, &g_queueId_6, 0, UART_QUEUE_BUF_MAX_LEN);
}

UINT32 QueueRead(UINT32 queueId, VOID *bufferAddr, UINT32 *bufferSize) {
  return LOS_QueueReadCopy(queueId, bufferAddr, bufferSize, 0);
}

UINT32 QueueWrite(UINT32 queueId, VOID *bufferAddr, UINT32 bufferSize) {
  return LOS_QueueWriteCopy(queueId, bufferAddr, bufferSize, 0);
}
