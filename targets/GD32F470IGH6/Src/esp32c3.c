/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ESP32C3 AT Command Handler Implementation
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

#include "esp32c3.h"
#include "toolkit.h"
#include "usart.h"
#include "los_task_pri.h"
#include <complex.h>
#include "modbus.h"
/* AT命令前缀宏定义 */

/* AT响应前缀宏定义 */
#define AT_RES_PREFIX_BLE_SENSOR    "+BLE_SENSOR:"     /* BLE传感器数据 */
#define AT_RES_PREFIX_UART_DEF    "+UART_DEF:"     /* UART默认配置 */



// 是否是HMI请求
bool is_hmi_request = true;

BLESensorData g_ble_sensor_data = {0};
char cmd_buf[DEFAULT_QUEUE_BUF_LEN] = {0};

/* 内部函数：实际发送数据到ESP32C3 (USART5) */
static void SendATRequest(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(USART5, data, len);
}

/* 检查字符串是否以指定前缀开头，返回0或1 */
static int32_t StrStartWith(const uint8_t *str, const char *prefix)
{
  if (!str || !prefix) return 0;
  while (*prefix != '\0') {
    if (*str != *prefix) {
      return 0;
    }
    str++;
    prefix++;
  }
  return 1;
}

bool parseBLESensor(char* cmd, BLESensorData *sensor_data) {
  char* token = cmd + strlen(AT_RES_PREFIX_BLE_SENSOR);
  char* next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  sensor_data->count = atoi(token);

  for (int32_t i = 0; i < BLE_SENSOR_COUNT; i++) {
    // 温度
    token = next + 1;
    next = strchr(token, ',');
    if (next == NULL) {
      return false;
    }
    *next = '\0';
    sensor_data->temp[i] = atoi(token);
    // 湿度
    token = next + 1;
    next = strchr(token, ',');
    if (next == NULL) {
      return false;
    }
    *next = '\0';
    sensor_data->humi[i] = atoi(token);
  }

  // 解析WiFi状态
  token = next + 1;
  next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  sensor_data->wifi_status = atoi(token);
  // 解析WiFi信号强度
  token = next + 1;
  next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  sensor_data->wifi_rssi = atoi(token);
  return true;
}

/* 发送AT响应到USART4或USART6 */
static void SendATResponse(const uint8_t *data, uint32_t len)
{

  // HMI请求，发送到USART6
  if (is_hmi_request) {
    Seria_SendArray(UART6, data, len);
    return;
  }
  // 非HMI请求，发送到USART4
  rs485_en(true);
  LOS_TaskDelay(5);
  Seria_SendArray(UART4, data, len);
  rs485_en(false);
  LOS_TaskDelay(5);
}

/* 处理ESP32C3的响应数据 */
void ATResponseHandle(uint8_t *res, uint32_t len) {
  if (res == NULL || len == 0) {
    return;
  }

  SEGGER_RTT_printf(0, "res = %s", res);
  if (StrStartWith(res, AT_RES_PREFIX_BLE_SENSOR)) {
    int32_t ret = parseBLESensor((char *)res, &g_ble_sensor_data);
    if (ret == 0) {
      // 传感器类型
      if (g_ble_sensor_data.count > 0) {
        g_sensor.TYPE |= SHT_Sensor;
        g_sensor.TYPE |= HMT_Sensor;
      }
      // BLE传感器数据
      // for (int32_t i = 0; i < BLE_SENSOR_COUNT; i++) {
      //   SEGGER_RTT_printf(0, "T%d=%d,H%d=%d ", i, g_ble_sensor_data.temp[i],
      //   i, g_ble_sensor_data.humi[i]);
      // }
      // SEGGER_RTT_printf(0, "count=%d,wifi=%d,rssi=%d\n",
      // g_ble_sensor_data.count, g_ble_sensor_data.wifi_status, g_ble_sensor_data.wifi_rssi);
    }
    return;
  } else if (StrStartWith(res, AT_RES_PREFIX_UART_DEF)) {
    SendATResponse(res, len);// 发送UART配置命令到USART6
    // 解析UART配置命令
    int baud = 0, dataBits = 0, stopBits = 0, parity = 0, addr = 0;
    if (parseUartConfigCommand((char *)res, &baud, &dataBits, &stopBits,
                               &parity, &addr)) {
      // 配置成功
      g_uart_config.baud = baud;
      g_uart_config.dataBits = dataBits;
      g_uart_config.stopBits = stopBits;
      g_uart_config.parity = parity;
      g_uart_config.addr = addr;
      // 重新配置RS485
      usart4_reconfig(baud, dataBits, stopBits, parity);
    }
    return;
  }

  /* 转发响应到USART6 */
  SendATResponse(res, len);
}

/* AT命令请求处理入口 */
void ATRequestHandle(uint8_t *req, uint32_t len, bool is_hmi)
{
  if (req == NULL || len == 0) {
    return;
  }
  // 标记是否是HMI请求
  is_hmi_request = is_hmi;
  SEGGER_RTT_printf(0, "req = %s, is_hmi = %d\n", req, is_hmi);

  // 发送命令到ESP32C3
  SendATRequest(req, len);
}

bool parseUartConfigCommand(char* cmd, int* baud, int* dataBits, int* stopBits, int* parity, int* addr) {
  char* token = cmd + strlen(AT_RES_PREFIX_UART_DEF);
  char* next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  *baud = atoi(token);

  token = next + 1;
  next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  *dataBits = atoi(token);

  token = next + 1;
  next = strchr(token, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  *stopBits = atoi(token);

  token = next + 1;
  if (token == NULL || *token == '\0') {
    return false;
  }

  next = strchr(token, ',');
  if (next == NULL) {
    *parity = atoi(token);
    *addr = 0;
  } else {
    *next = '\0';
    *parity = atoi(token);
    token = next + 1;
    if (token == NULL || *token == '\0') {
      return false;
    }
    *addr = atoi(token);
  }

  // Validation: ESP32-C3 ranges and requested numeric encoding
  if (*baud < 80 || *baud > 5000000) return false;
  if (*dataBits < 5 || *dataBits > 9) return false; // 5bit,6bit,7bit,8bit,9bit
  if (*stopBits < 1 || *stopBits > 3) return false; // 1=1bit,2=1.5bit,3=2bit
  if (*parity < 0 || *parity > 2) return false; // 0=None,1=Odd,2=Even
  if (*addr < 0 || *addr > 255) return false;

  return true;
}

void SendSensorToESPC3(void) {
  snprintf(cmd_buf, DEFAULT_QUEUE_BUF_LEN, "AT+SENSOR=%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", g_sensor.CO2, g_sensor.CH2O, g_sensor.TVOC, g_sensor.PM25, g_sensor.PM100, g_sensor.TEMP, g_sensor.RH, g_sensor.PM10, g_sensor.TYPE);
  SendATRequest((uint8_t*)cmd_buf, strlen(cmd_buf));
}
