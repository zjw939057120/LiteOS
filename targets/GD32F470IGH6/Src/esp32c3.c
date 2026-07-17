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

/* AT命令类型枚举 */
typedef enum {
  AT_CMD_UNKNOWN = 0,
} AT_CMD_TYPE;

BLESensorData g_ble_sensor_data = {0};
char cmd_buf[DEFAULT_QUEUE_BUF_LEN] = {0};

/* 内部函数：实际发送数据到ESP32C3 (USART5) */
static void SendToESP32C3(const uint8_t *data, uint32_t len)
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

/* 查找子字符串，返回匹配位置索引，未找到返回-1 */
int32_t StrWith(const uint8_t *text, const char *pattern)
{
  if (!text || !pattern) return -1;

  uint32_t text_len = 0;
  const uint8_t *p = text;
  while (*p != '\0') {
    text_len++;
    p++;
  }
  uint32_t pattern_len = 0;
  const char *n = pattern;
  while (*n != '\0') {
    pattern_len++;
    n++;
  }

  /* 边界检查：模式串不能为空，且不能长于主串 */
  if (pattern_len == 0 || pattern_len > text_len) return -1;

  for (uint32_t i = 0; i <= text_len - pattern_len; i++) {
    /* 使用 memcmp 进行内存块比较，通常比逐字符循环更快 */
    if (memcmp(text + i, pattern, pattern_len) == 0) {
      return (int)i;
    }
  }
  return -1;
}

static int32_t StrToInt(const uint8_t *str, uint32_t *consumed) {
  if (!str || !consumed) return 0;
  *consumed = 0;
  int32_t result = 0;
  int8_t sign = 1;
  if (*str == '-') {
    sign = -1;
    str++;
    (*consumed)++;
  } else if (*str == '+') {
    str++;
    (*consumed)++;
  }
  while (*str >= '0' && *str <= '9') {
    result = result * 10 + (*str - '0');
    str++;
    (*consumed)++;
  }
  return result * sign;
}

int32_t parseBLESensor(const uint8_t *data, uint32_t len, BLESensorData *sensor_data) {
  uint32_t i = strlen(AT_RES_PREFIX_BLE_SENSOR);

  for (int32_t j = 0; j < BLE_SENSOR_COUNT; j++) {
    while (i < len && data[i] == ',') i++;
    if (i >= len) return -1;
    uint32_t consumed = 0;
    sensor_data->temp[j] = (int16_t)StrToInt(&data[i], &consumed);
    if (consumed == 0) return -1;
    i += consumed;

    while (i < len && data[i] == ',') i++;
    if (i >= len) return -1;
    consumed = 0;
    sensor_data->humi[j] = (int16_t)StrToInt(&data[i], &consumed);
    if (consumed == 0) return -1;
    i += consumed;
  }

  while (i < len && data[i] == ',') i++;
  if (i >= len) return -1;
  uint32_t consumed = 0;
  sensor_data->wifi_status = (int8_t)StrToInt(&data[i], &consumed);
  if (consumed == 0) return -1;
  i += consumed;

  while (i < len && data[i] == ',') i++;
  if (i >= len) return -1;
  consumed = 0;
  sensor_data->wifi_rssi = (int8_t)StrToInt(&data[i], &consumed);
  if (consumed == 0) return -1;

  return 0;
}

/* 解析AT命令类型 */
static AT_CMD_TYPE ParseATCommand(const uint8_t *cmd, uint32_t len)
{
  if (cmd == NULL || len < 2) {
    return AT_CMD_UNKNOWN;
  }
  /* 检查是否是AT开头 */
  else if (cmd[0] != 'A' || cmd[1] != 'T') {
    return AT_CMD_UNKNOWN;
  }
  return AT_CMD_UNKNOWN;
}

/* 发送AT响应到USART6 */
static void SendResponseToUSART6(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(UART6, data, len);
}

/* 处理ESP32C3的响应数据 */
void ATResponseHandle(uint8_t *res, uint32_t len) {
  if (res == NULL || len == 0) {
    return;
  }

  SEGGER_RTT_printf(0, "res = %s", res);
  if (StrStartWith(res, AT_RES_PREFIX_BLE_SENSOR)) {
    int32_t ret = parseBLESensor(res, len, &g_ble_sensor_data);
    if (ret == 0) {
      // 传感器类型
      g_sensor.TYPE |= SHT_Sensor;
      g_sensor.TYPE |= HMT_Sensor;
      // BLE传感器数据
      // for (int32_t i = 0; i < BLE_SENSOR_COUNT; i++) {
      //   SEGGER_RTT_printf(0, "T%d=%d,H%d=%d ", i, g_ble_sensor_data.temp[i], i, g_ble_sensor_data.humi[i]);
      // }
      // SEGGER_RTT_printf(0, "wifi=%d,rssi=%d\n", g_ble_sensor_data.wifi_status, g_ble_sensor_data.wifi_rssi);
    }
    // BLE传感器数据不转发
    return;
  }else if (StrStartWith(res, AT_RES_PREFIX_UART_DEF)) {
      // 解析UART配置命令
      int baud = 0, dataBits = 0, stopBits = 0, parity = 0, addr = 0;
      if (parseUartConfigCommand((char*)res, &baud, &dataBits, &stopBits, &parity, &addr)) {
        // 配置成功
        g_uart_config.baud = baud;
        g_uart_config.dataBits = dataBits;
        g_uart_config.stopBits = stopBits;
        g_uart_config.parity = parity;
        g_uart_config.addr = addr;
        //重新配置RS485
        usart4_reconfig(baud, dataBits, stopBits, parity);
      }
  }

  /* 转发响应到USART6 */
  SendResponseToUSART6(res, len);
}

/* AT命令请求处理入口 */
void ATRequestHandle(uint8_t *req, uint32_t len)
{
  /* 解析AT命令类型 */
  AT_CMD_TYPE cmd_type = ParseATCommand(req, len);
  SEGGER_RTT_printf(0, "req = %s, cmd_type = %d\n", req, cmd_type);
  /* 根据命令类型分发处理 */
  switch (cmd_type) {
    case AT_CMD_UNKNOWN: {
      // 发送命令到ESP32C3
      SendToESP32C3(req, len);
    }
    break;
    default: {
      // 发送命令到ESP32C3
      SendToESP32C3(req, len);
    }
    break;
    }
}

bool parseUartConfigCommand(char* cmd, int* baud, int* dataBits, int* stopBits, int* parity, int* addr) {
  char* paramStart = cmd + strlen(AT_RES_PREFIX_UART_DEF);
  char* next = strchr(paramStart, ',');
  if (next == NULL) {
    return false;
  }
  *next = '\0';
  *baud = atoi(paramStart);

  char* token = next + 1;
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
  SendToESP32C3((uint8_t*)cmd_buf, strlen(cmd_buf));
}
