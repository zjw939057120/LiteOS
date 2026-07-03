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
#define AT_PREFIX_CWMODE    "+CWMODE="    /* WiFi模式设置 */
#define AT_PREFIX_CWLAP     "+CWLAP"      /* 列出可用AP */
#define AT_PREFIX_CWJAP_Q   "+CWJAP?"     /* 查询当前连接的AP */
#define AT_PREFIX_CWJAP_E   "+CWJAP="     /* WiFi连接AP */
#define AT_PREFIX_CWQAP     "+CWQAP"      /* WiFi断开连接 */
#define AT_PREFIX_CIPSTA    "+CIPSTA"     /* 静态IP配置 */
#define AT_PREFIX_CWDHCP    "+CWDHCP"     /* DHCP配置 */
#define AT_PREFIX_CWSTATE   "+CWSTATE"    /* WiFi状态查询 */
#define AT_PREFIX_BLEINIT   "+BLEINIT="   /* BLE初始化角色 */
#define AT_PREFIX_BLESCAN   "+BLESCAN"    /* BLE扫描 */
#define AT_PREFIX_CIPSTART  "+CIPSTART"   /* TCP连接 */
#define AT_PREFIX_CIPSEND   "+CIPSEND"    /* TCP发送 */
#define AT_PREFIX_CIPCLOSE  "+CIPCLOSE"   /* TCP关闭 */
#define AT_PREFIX_CIPSTATUS "+CIPSTATUS"  /* TCP状态查询 */
#define AT_PREFIX_CIPMODE   "+CIPMODE"    /* TCP透传模式 */
#define AT_PREFIX_GMR       "+GMR"        /* 版本信息查询 */

/* AT响应前缀宏定义 */
#define AT_RES_PREFIX_CWLAP     "+CWLAP:"      /* WiFi扫描结果 */
#define AT_RES_PREFIX_SENSOR    "+SENSOR:"     /* BLE传感器数据 */

/* AT命令类型枚举 */
typedef enum {
  AT_CMD_UNKNOWN = 0,
  AT_CMD_WIFI_MODE,          /* AT+CWMODE= WiFi模式设置 */
  AT_CMD_WIFI_SCAN,          /* AT+CWLAP   列出可用AP */
  AT_CMD_WIFI_CUR_AP,        /* AT+CWJAP?  查询当前连接的AP */
  AT_CMD_WIFI_CONNECT,       /* AT+CWJAP=  WiFi连接 */
  AT_CMD_WIFI_DISCONNECT,    /* AT+CWQAP   WiFi断开 */
  AT_CMD_WIFI_STATUS,        /* AT+CWSTATE WiFi状态查询 */
  AT_CMD_NET_CONFIG,         /* AT+CIPSTA  静态IP配置 */
  AT_CMD_NET_DHCP,           /* AT+CWDHCP  DHCP配置 */
  AT_CMD_BLE_INIT,           /* AT+BLEINIT= BLE初始化角色 */
  AT_CMD_BLE_SCAN_START,     /* AT+BLESCAN 蓝牙扫描启动 */
  AT_CMD_BLE_SCAN_STOP,      /* AT+BLESCAN=0 蓝牙扫描停止 */
  AT_CMD_TCP_CONNECT,        /* AT+CIPSTART TCP连接 */
  AT_CMD_TCP_SEND,           /* AT+CIPSEND TCP发送 */
  AT_CMD_TCP_CLOSE,          /* AT+CIPCLOSE TCP关闭 */
  AT_CMD_TCP_STATUS,         /* AT+CIPSTATUS TCP状态 */
  AT_CMD_TCP_TRANSPARENT,    /* AT+CIPMODE TCP透传模式 */
  AT_CMD_TCP_TRANSMIT,       /* 透传数据 */
  AT_CMD_TEST,               /* AT 测试命令 */
  AT_CMD_GMR,                /* AT+GMR 版本信息 */
} AT_CMD_TYPE;

/* AT命令结构体 */
typedef struct {
  AT_CMD_TYPE type;
  uint8_t cmd_buf[256];
  uint32_t cmd_len;
} AT_CMD;

BLESensorData g_ble_sensor_data = {0};

/* 内部函数：实际发送数据到ESP32C3 (USART5) */
static void SendToESP32C3(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(USART5, data, len);
}

/* 检查字符串是否以指定前缀开头，返回0或1 */
static int32_t StrStartsWith(const uint8_t *str, const char *prefix)
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
static int32_t StrFind(const uint8_t *text, const char *pattern)
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
  if (!data || !sensor_data || len == 0) {
    return -1;
  }

  uint32_t i = 0;
  if (!StrStartsWith(data, AT_RES_PREFIX_SENSOR)) {
    return -1;
  }
  i += 8;

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
  if (cmd[0] != 'A' || cmd[1] != 'T') {
    return AT_CMD_UNKNOWN;
  }
  /* 纯AT测试命令 */
  if (len == 4 && cmd[2] == '\r' && cmd[3] == '\n') {
    return AT_CMD_TEST;
  }
  /* 解析具体命令 */
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWMODE)) {
    return AT_CMD_WIFI_MODE;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWLAP)) {
    return AT_CMD_WIFI_SCAN;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWJAP_Q)) {
    return AT_CMD_WIFI_CUR_AP;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWJAP_E)) {
    return AT_CMD_WIFI_CONNECT;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWQAP)) {
    return AT_CMD_WIFI_DISCONNECT;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPSTA)) {
    return AT_CMD_NET_CONFIG;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWDHCP)) {
    return AT_CMD_NET_DHCP;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CWSTATE)) {
    return AT_CMD_WIFI_STATUS;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_BLEINIT)) {
    return AT_CMD_BLE_INIT;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_BLESCAN)) {
    if (StrFind(cmd, "=0") >= 0) {
      return AT_CMD_BLE_SCAN_STOP;
    }
    return AT_CMD_BLE_SCAN_START;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPSTART)) {
    return AT_CMD_TCP_CONNECT;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPSEND)) {
    return AT_CMD_TCP_SEND;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPCLOSE)) {
    return AT_CMD_TCP_CLOSE;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPSTATUS)) {
    return AT_CMD_TCP_STATUS;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_CIPMODE)) {
    return AT_CMD_TCP_TRANSPARENT;
  }
  if (StrStartsWith(&cmd[2], AT_PREFIX_GMR)) {
    return AT_CMD_GMR;
  }
  return AT_CMD_UNKNOWN;
}

/* 发送AT响应到USART6 */
static void SendResponseToUSART6(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(UART6, data, len);
}

/* WiFi扫描结果计数 */
static volatile uint8_t g_wifi_scan_count = 0;

/* 处理ESP32C3的响应数据 */
void ATResponseHandle(const uint8_t *res, uint32_t len) {
  if (res == NULL || len == 0) {
    return;
  }

  SEGGER_RTT_printf(0, "res = %s", res);
  /* WiFi扫描结果处理 */
  if (StrStartsWith(res, AT_RES_PREFIX_CWLAP)) {
    /* 解析SSID: +CWLAP:(...,"ssid",...) */
    int quote_start_idx = StrFind(res, ",\"");
    if (quote_start_idx >= 0) {
      int ssid_start_idx = quote_start_idx + 2;
      int quote_end_idx = StrFind(res + ssid_start_idx, "\"");
      if (quote_end_idx >= 0) {
        int ssid_len = quote_end_idx;
        /* 过滤空名称 */
        if (ssid_len == 0) return;
        /* 过滤非ASCII字符的SSID */
        for (int i = 0; i < ssid_len; i++) {
          if (res[ssid_start_idx + i] < 0x20 || res[ssid_start_idx + i] > 0x7E) {
            SEGGER_RTT_printf(0, "filter non-ascii ssid: %s\n", res);
            return;
          }
        }
      }
    }
    g_wifi_scan_count++;
    if (g_wifi_scan_count > 20) {
      /* 超过20条，过滤不转发 */
      SEGGER_RTT_printf(0, "filter wifi scan result #%d\n", g_wifi_scan_count);
      return;
    }
  }

  // 传感器数据处理
  else if (StrStartsWith(res, AT_RES_PREFIX_SENSOR)) {
    int32_t ret = parseBLESensor(res, len, &g_ble_sensor_data);
    if (ret == 0) {
      // 传感器类型
      g_sensor.TYPE |= SHT_Sensor;
      g_sensor.TYPE |= HMT_Sensor;
      for (int32_t i = 0; i < BLE_SENSOR_COUNT; i++) {
        SEGGER_RTT_printf(0, "T%d=%d,H%d=%d ", i, g_ble_sensor_data.temp[i], i, g_ble_sensor_data.humi[i]);
      }
      // 传感器数据
      SEGGER_RTT_printf(0, "wifi=%d,rssi=%d\n", g_ble_sensor_data.wifi_status, g_ble_sensor_data.wifi_rssi);
    }
    return;
  }

  /* 转发响应到USART6 */
  SendResponseToUSART6(res, len);
}

/* AT命令请求处理入口 */
void ATRequestHandle(const uint8_t *req, uint32_t len)
{
  /* 解析AT命令类型 */
  AT_CMD_TYPE cmd_type = ParseATCommand(req, len);
  SEGGER_RTT_printf(0, "req = %s, cmd_type = %d\n", req, cmd_type);

  /* 根据命令类型分发处理 */
  switch (cmd_type) {
    case AT_CMD_TEST:
      break;

    case AT_CMD_GMR:
      break;

    case AT_CMD_WIFI_MODE:
      break;

    case AT_CMD_WIFI_SCAN:
  /* WiFi扫描开始，重置计数 */
    g_wifi_scan_count = 0;
      break;

    case AT_CMD_WIFI_CUR_AP:
      break;

    case AT_CMD_WIFI_CONNECT:
      break;

    case AT_CMD_WIFI_DISCONNECT:
      break;

    case AT_CMD_WIFI_STATUS:
      break;

    case AT_CMD_NET_CONFIG:
      break;

    case AT_CMD_NET_DHCP:
      break;

    case AT_CMD_BLE_INIT:
      break;

    case AT_CMD_BLE_SCAN_START:
      break;

    case AT_CMD_BLE_SCAN_STOP:
      break;

    case AT_CMD_TCP_CONNECT:
      break;

    case AT_CMD_TCP_SEND:
      break;

    case AT_CMD_TCP_CLOSE:
      break;

    case AT_CMD_TCP_STATUS:
      break;

    case AT_CMD_TCP_TRANSPARENT:
      break;

    case AT_CMD_TCP_TRANSMIT:
      break;

    case AT_CMD_UNKNOWN:
    default:
      break;
  }

  SendToESP32C3(req, len);
}
