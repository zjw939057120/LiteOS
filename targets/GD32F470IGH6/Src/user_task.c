/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: User Task Implementation
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

#include "los_task_pri.h"
#include "demo_entry.h"
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "queue.h"

#define TASK_DELAY 1000

UINT32 g_UartTaskId = 0;
UINT32 g_RecvUart0TaskId = 0;
UINT32 g_RecvUart1TaskId = 0;
UINT32 g_RecvUart2TaskId = 0;
UINT32 g_RecvUart3TaskId = 0;
UINT32 g_RecvUart4TaskId = 0;
UINT32 g_RecvUart5TaskId = 0;
UINT32 g_RecvUart6TaskId = 0;

UINT32 UartTaskEntry(VOID)
{
    GpioInit();
    UsartInit();
    I2cInit();
    while (1) {
        SEGGER_RTT_printf(0, "UartTaskEntry %d\n", g_UartTaskId);
        Usart0Req();
        Usart1Req();
        Usart2Req();
        Usart3Req();
        Usart4Req();
        Usart5Req();
        Usart6Req();
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart0TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart0, buff, &recvLen);
        uint16_t TVOC = 0;
        TVOC = buff[8] * 256 + buff[9]; // Data[8]*256+Data[9]
        SEGGER_RTT_printf(0, "RecvUart0TaskEntry %d, recvLen = %d, ret = %d, TVOC = %d\n",g_RecvUart0TaskId, recvLen, ret, TVOC);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart1TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart1, buff, &recvLen);
        uint16_t PPB = 0;
        PPB = (buff[2] * 256 + buff[3]); // (Data[2]*256+Data[3])
        SEGGER_RTT_printf(0, "RecvUart1TaskEntry %d, recvLen = %d, ret = %d, PPB = %d\n",g_RecvUart1TaskId, recvLen, ret, PPB);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart2TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart2, buff, &recvLen);
        uint16_t CO2 = 0;
        CO2 = (buff[3] * 256 + buff[4]); // (Data[3]*256+Data[4])
        SEGGER_RTT_printf(0, "RecvUart2TaskEntry %d, recvLen = %d, ret = %d, CO2 = %d\n",g_RecvUart2TaskId, recvLen, ret, CO2);
        SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart3TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart3, buff, &recvLen);
        SEGGER_RTT_printf(0, "RecvUart3TaskEntry %d, recvLen = %d, ret = %d\n",g_RecvUart3TaskId, recvLen, ret);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart4TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart4, buff, &recvLen);
        SEGGER_RTT_printf(0, "RecvUart4TaskEntry %d, recvLen = %d, ret = %d\n",g_RecvUart4TaskId, recvLen, ret);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart5TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart5, buff, &recvLen);
        SEGGER_RTT_printf(0, "RecvUart5TaskEntry %d, recvLen = %d, ret = %d\n",g_RecvUart5TaskId, recvLen, ret);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUart6TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        UINT32 ret = QueueRecv(g_queueId_uart6, buff, &recvLen);
        SEGGER_RTT_printf(0, "RecvUart6TaskEntry %d, recvLen = %d, ret = %d\n",g_RecvUart6TaskId, recvLen, ret);
        // SEGGER_RTT_printf_hex(buff, recvLen);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 UartTaskCreate(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)UartTaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "UartTask";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_UartTaskId, &uartTask);
}

UINT32 RecvUart0Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart0TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart0Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart0TaskId, &uartTask);
}
UINT32 RecvUart1Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart1TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart1Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart1TaskId, &uartTask);
}
UINT32 RecvUart2Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart2TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart2Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart2TaskId, &uartTask);
}
UINT32 RecvUart3Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart3TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart3Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart3TaskId, &uartTask);
}
UINT32 RecvUart4Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart4TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart4Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart4TaskId, &uartTask);
}
UINT32 RecvUart5Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart5TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart5Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart5TaskId, &uartTask);
}
UINT32 RecvUart6Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUart6TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUart6Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUart6TaskId, &uartTask);
}

VOID app_init(VOID)
{
    QueueInit();
    (VOID)UartTaskCreate();
    (VOID)RecvUart0Create();
    (VOID)RecvUart1Create();
    (VOID)RecvUart2Create();
    (VOID)RecvUart3Create();
    (VOID)RecvUart4Create();
    (VOID)RecvUart5Create();
    (VOID)RecvUart6Create();
    SEGGER_RTT_printf(0, "app init!\n");
    DemoEntry();
}
