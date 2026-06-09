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
#include "sensor.h"

#define TASK_DELAY 1000

UINT32 g_UartTaskId = 0;
UINT32 g_RecvUsart0TaskId = 0;
UINT32 g_RecvUsart1TaskId = 0;
UINT32 g_RecvUsart2TaskId = 0;
UINT32 g_RecvUsart3TaskId = 0;
UINT32 g_RecvUsart4TaskId = 0;
UINT32 g_RecvUsart5TaskId = 0;
UINT32 g_RecvUsart6TaskId = 0;

UINT32 SensorTaskEntry(VOID)
{
    GpioInit();
    UsartInit();
    I2cInit();
    while (1) {
        SEGGER_RTT_printf(0, "SensorTaskEntry loop\n");
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

UINT32 RecvUsart0TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
      resetSensorTVOC(&g_sensor);
      recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
      ret = QueueRecv(g_queueId_uart0, buff, &recvLen);
      // SEGGER_RTT_printf_hex(buff, recvLen);
      DecodeSensorDataTVOC(buff, &g_sensor);

      SEGGER_RTT_printf(
          0, "RecvUsart0TaskEntry recvLen = %d, ret = %d, TVOC = %d\n", recvLen,
          ret, g_sensor.TVOC);
      LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart1TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
      resetSensorPPB(&g_sensor);
      recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
      ret = QueueRecv(g_queueId_uart1, buff, &recvLen);
      // SEGGER_RTT_printf_hex(buff, recvLen);
      DecodeSensorDataPPB(buff, &g_sensor);

      SEGGER_RTT_printf(
          0, "RecvUsart1TaskEntry recvLen = %d, ret = %d, PPB = %d\n", recvLen,
          ret, g_sensor.PPB);
      LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart2TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
      resetSensorCO2(&g_sensor);
      recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
      ret = QueueRecv(g_queueId_uart2, buff, &recvLen);
      // SEGGER_RTT_printf_hex(buff, recvLen);
      DecodeSensorDataCO2(buff, &g_sensor);

      SEGGER_RTT_printf(
          0, "RecvUsart2TaskEntry recvLen = %d, ret = %d, CO2 = %d\n", recvLen,
          ret, g_sensor.CO2);
      LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart3TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
      resetSensorPM10(&g_sensor);
      resetSensorPM25(&g_sensor);
      resetSensorPM100(&g_sensor);
      recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
      ret = QueueRecv(g_queueId_uart3, buff, &recvLen);
      // SEGGER_RTT_printf_hex(buff, recvLen);
      DecodeSensorDataPM10(buff, &g_sensor);
      DecodeSensorDataPM25(buff, &g_sensor);
      DecodeSensorDataPM100(buff, &g_sensor);

      SEGGER_RTT_printf(0,
                        "RecvUsart3TaskEntry recvLen = %d, ret = %d, PM10 = "
                        "%d, PM25 = %d, PM100 = %d\n",
                        recvLen, ret, g_sensor.PM10, g_sensor.PM25,
                        g_sensor.PM100);
      LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart4TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        ret = QueueRecv(g_queueId_uart4, buff, &recvLen);
        // SEGGER_RTT_printf_hex(buff, recvLen);

        SEGGER_RTT_printf(0, "RecvUsart4TaskEntry recvLen = %d, ret = %d\n", recvLen, ret);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart5TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
    while (1) {
        recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
        ret = QueueRecv(g_queueId_uart5, buff, &recvLen);
        // SEGGER_RTT_printf_hex(buff, recvLen);

        SEGGER_RTT_printf(0, "RecvUsart5TaskEntry recvLen = %d, ret = %d\n", recvLen, ret);
        LOS_TaskDelay(TASK_DELAY);
    }
    return 0;
}

UINT32 RecvUsart6TaskEntry(VOID)
{
    UINT8 buff[DEFAULT_QUEUE_BUF_MAX_LEN];
    UINT32 recvLen = 0;
    UINT32 ret = 0;
        while (1) {
          resetModbus(&g_modbus);
          recvLen = DEFAULT_QUEUE_BUF_MAX_LEN;
          ret = QueueRecv(g_queueId_uart6, buff, &recvLen);
          SEGGER_RTT_printf_hex(buff, recvLen);
          DecodeModbusData(buff, &g_modbus);

          SEGGER_RTT_printf(0, "RecvUsart6TaskEntry recvLen = %d, ret = %d, address = %d, func_code = %d, reg_addr = %d, reg_number = %d, crc_sum = %d\n",
                            recvLen, ret, g_modbus.address, g_modbus.func_code, g_modbus.reg_addr, g_modbus.reg_number, g_modbus.crc_sum);
          LOS_TaskDelay(TASK_DELAY);
        }
    return 0;
}

UINT32 SensorTaskCreate(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)SensorTaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "SensorTask";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_UartTaskId, &uartTask);
}

UINT32 RecvUsart0Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart0TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart0Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart0TaskId, &uartTask);
}
UINT32 RecvUsart1Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart1TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart1Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart1TaskId, &uartTask);
}
UINT32 RecvUsart2Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart2TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart2Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart2TaskId, &uartTask);
}
UINT32 RecvUsart3Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart3TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart3Task";
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart3TaskId, &uartTask);
}
UINT32 RecvUsart4Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart4TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart4Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart4TaskId, &uartTask);
}
UINT32 RecvUsart5Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart5TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart5Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart5TaskId, &uartTask);
}
UINT32 RecvUsart6Create(VOID)
{
    INT32 ret;
    TSK_INIT_PARAM_S uartTask;

    ret = memset_s(&uartTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return ret;
    }
    uartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvUsart6TaskEntry;
    uartTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uartTask.pcName = "RecvUsart6Task"; 
    uartTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    uartTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&g_RecvUsart6TaskId, &uartTask);
}

VOID app_init(VOID)
{
    QueueInit();
    SensorTaskCreate();
    RecvUsart0Create();
    RecvUsart1Create();
    RecvUsart2Create();
    RecvUsart3Create();
    RecvUsart4Create();
    RecvUsart5Create();
    RecvUsart6Create();
    SEGGER_RTT_printf(0, "app init!\n");
    DemoEntry();
}
