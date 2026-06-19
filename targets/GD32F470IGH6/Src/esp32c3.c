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

/* AT命令类型枚举 */
typedef enum {
  AT_CMD_UNKNOWN = 0,
  AT_CMD_WIFI_SCAN,          /* AT+CWLAP   WiFi扫描 */
  AT_CMD_WIFI_CUR_AP,        /* AT+CWJAP?  查询当前连接的AP */
  AT_CMD_WIFI_CONNECT,       /* AT+CWJAP   WiFi连接 */
  AT_CMD_WIFI_DISCONNECT,    /* AT+CWQAP   WiFi断开 */
  AT_CMD_WIFI_STATUS,        /* AT+CWSTATE WiFi状态查询 */
  AT_CMD_NET_CONFIG,         /* AT+CIPSTA  静态IP配置 */
  AT_CMD_NET_DHCP,           /* AT+CWDHCP  DHCP配置 */
  AT_CMD_BLE_SCAN_START,     /* AT+BLESCAN 蓝牙扫描启动 */
  AT_CMD_BLE_SCAN_STOP,      /* AT+BLESCAN=0 蓝牙扫描停止 */
  AT_CMD_BLE_SCAN_RESULT,    /* AT+BLESCAN=? 蓝牙扫描结果 */
  AT_CMD_TCP_CONNECT,        /* AT+CIPSTART TCP连接 */
  AT_CMD_TCP_SEND,           /* AT+CIPSEND TCP发送 */
  AT_CMD_TCP_CLOSE,          /* AT+CIPCLOSE TCP关闭 */
  AT_CMD_TCP_STATUS,         /* AT+CIPSTATUS TCP状态 */
  AT_CMD_TCP_TRANSPARENT,    /* AT+CIPMODE TCP透传模式 */
  AT_CMD_TCP_TRANSMIT,       /* 透传数据 */
  AT_CMD_TEST,               /* AT 测试命令 */
  AT_CMD_GMR,                /* AT+GMR 版本信息 */
} AT_CMD_TYPE;

/* ESP32C3工作状态 */
typedef enum {
  ESP32_STATE_IDLE = 0,
  ESP32_STATE_WIFI_CONNECTING,
  ESP32_STATE_WIFI_CONNECTED,
  ESP32_STATE_TCP_CONNECTING,
  ESP32_STATE_TCP_CONNECTED,
  ESP32_STATE_TCP_TRANSPARENT,
  ESP32_STATE_BLE_SCANNING,
} ESP32_STATE;

/* ESP32C3状态 */
static volatile ESP32_STATE g_esp32_state = ESP32_STATE_IDLE;

/* 透传模式标志 */
static volatile uint8_t g_transparent_mode = 0;

/* TCP连接ID */
static volatile uint8_t g_tcp_link_id = 0;

/* 内部函数：实际发送数据到ESP32C3 (USART5) */
static void SendToESP32C3(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(USART5, data, len);
}

/* 字符串比较辅助函数 */
static int32_t StrStartsWith(const uint8_t *str, const char *prefix)
{
  if (str == NULL || prefix == NULL) {
    return 0;
  }
  uint32_t i = 0;
  while (prefix[i] != '\0') {
    if (str[i] == '\0' || str[i] != (uint8_t)prefix[i]) {
      return 0;
    }
    i++;
  }
  return 1;
}

/* 查找子字符串 */
static uint8_t *StrFind(const uint8_t *haystack, const char *needle)
{
  if (haystack == NULL || needle == NULL || needle[0] == '\0') {
    return (uint8_t *)haystack;
  }
  uint32_t i = 0;
  while (haystack[i] != '\0') {
    uint32_t j = 0;
    while (haystack[i + j] != '\0' && needle[j] != '\0' &&
           haystack[i + j] == (uint8_t)needle[j]) {
      j++;
    }
    if (needle[j] == '\0') {
      return (uint8_t *)&haystack[i];
    }
    i++;
  }
  return NULL;
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
  if (len > 2 && StrStartsWith(&cmd[2], "+CWJAP?")) {
    return AT_CMD_WIFI_CUR_AP;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CWLAP")) {
    return AT_CMD_WIFI_SCAN;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CWJAP=")) {
    return AT_CMD_WIFI_CONNECT;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CWQAP")) {
    return AT_CMD_WIFI_DISCONNECT;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPSTA")) {
    return AT_CMD_NET_CONFIG;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CWDHCP")) {
    return AT_CMD_NET_DHCP;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CWSTATE")) {
    return AT_CMD_WIFI_STATUS;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+BLESCAN")) {
    if (StrFind(cmd, "=0") != NULL) {
      return AT_CMD_BLE_SCAN_STOP;
    }
    if (StrFind(cmd, "=?") != NULL) {
      return AT_CMD_BLE_SCAN_RESULT;
    }
    return AT_CMD_BLE_SCAN_START;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPSTART")) {
    return AT_CMD_TCP_CONNECT;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPSEND")) {
    return AT_CMD_TCP_SEND;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPCLOSE")) {
    return AT_CMD_TCP_CLOSE;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPSTATUS")) {
    return AT_CMD_TCP_STATUS;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+CIPMODE")) {
    return AT_CMD_TCP_TRANSPARENT;
  }
  if (len > 2 && StrStartsWith(&cmd[2], "+GMR")) {
    return AT_CMD_GMR;
  }
  return AT_CMD_UNKNOWN;
}

/* 网络配置 - WiFi连接 */
static void HandleWiFiConnect(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CWJAP="ssid","password" */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_WIFI_CONNECTING;
}

/* 网络配置 - WiFi断开 */
static void HandleWiFiDisconnect(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CWQAP */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_IDLE;
}

/* 蓝牙扫描 - 启动 */
static void HandleBLEScanStart(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+BLESCAN=<interval>,<window>,<duration> */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_BLE_SCANNING;
}

/* 蓝牙扫描 - 停止 */
static void HandleBLEScanStop(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+BLESCAN=0 */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_IDLE;
}

/* TCP连接 */
static void HandleTCPConnect(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPSTART=<link_id>,"TCP","ip",<port> */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_TCP_CONNECTING;
  for (uint32_t i = 0; i < len; i++) {
    if (cmd[i] == '=' && i + 1 < len) {
      g_tcp_link_id = cmd[i + 1] - '0';
      break;
    }
  }
}

/* TCP关闭 */
static void HandleTCPClose(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPCLOSE=<link_id> */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_WIFI_CONNECTED;
}

/* TCP透传模式设置 */
static void HandleTCPTransparent(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPMODE=<mode>  0:正常模式 1:透传模式 */
  SendToESP32C3(cmd, len);
  if (StrFind(cmd, "=1") != NULL) {
    g_transparent_mode = 1;
    g_esp32_state = ESP32_STATE_TCP_TRANSPARENT;
  } else {
    g_transparent_mode = 0;
  }
}

/* 发送AT响应到USART6 */
static void SendResponseToUSART6(const uint8_t *data, uint32_t len)
{
  Seria_SendArray(UART6, data, len);
}

/* 处理ESP32C3的响应数据 */
void ATRespHandle(const uint8_t *array, uint32_t len)
{
  if (array == NULL || len == 0) {
    return;
  }

  /* 在透传模式下，数据直接转发到USART6 */
  if (g_transparent_mode == 1 && g_esp32_state == ESP32_STATE_TCP_TRANSPARENT) {
    /* 检查是否是退出透传的特定序列 (+++) */
    if (len == 3 && array[0] == '+' && array[1] == '+' && array[2] == '+') {
      g_transparent_mode = 0;
      g_esp32_state = ESP32_STATE_TCP_CONNECTED;
      SendResponseToUSART6(array, len);
      return;
    }
    /* 透传数据直接转发 */
    SendResponseToUSART6(array, len);
    return;
  }

  /* 检查连接状态变化 */
  if (StrFind(array, "WIFI CONNECTED") != NULL ||
      StrFind(array, "WIFI GOT IP") != NULL) {
    g_esp32_state = ESP32_STATE_WIFI_CONNECTED;
  }
  if (StrFind(array, "WIFI DISCONNECTED") != NULL) {
    g_esp32_state = ESP32_STATE_IDLE;
    g_transparent_mode = 0;
  }
  /* TCP连接成功检测: 需要排除WIFI CONNECTED的误匹配 */
  if (g_esp32_state == ESP32_STATE_TCP_CONNECTING) {
    uint8_t *connect_pos = StrFind(array, "CONNECT");
    if (connect_pos != NULL) {
      /* 检查不是"WIFI CONNECTED" */
      if (connect_pos > array && *(connect_pos - 1) != ' ') {
        g_esp32_state = ESP32_STATE_TCP_CONNECTED;
      } else if (StrFind(array, "OK") != NULL && StrFind(array, "WIFI") == NULL) {
        g_esp32_state = ESP32_STATE_TCP_CONNECTED;
      }
    }
  }
  if (StrFind(array, "CLOSED") != NULL) {
    g_esp32_state = ESP32_STATE_WIFI_CONNECTED;
    g_transparent_mode = 0;
  }
  if (StrFind(array, "SCAN DONE") != NULL ||
      StrFind(array, "+BLESCAN:END") != NULL) {
    g_esp32_state = ESP32_STATE_IDLE;
  }

  /* 转发响应到USART6 */
  SendResponseToUSART6(array, len);
}

/* AT命令请求处理入口 */
void ATReqHandle(const uint8_t *array, uint32_t len)
{
  if (array == NULL || len == 0) {
    return;
  }

  /* 在透传模式下，数据直接透传到ESP32C3 */
  if (g_transparent_mode == 1 && g_esp32_state == ESP32_STATE_TCP_TRANSPARENT) {
    SendToESP32C3(array, len);
    return;
  }

  /* 解析AT命令类型 */
  AT_CMD_TYPE cmd_type = ParseATCommand(array, len);
  SEGGER_RTT_printf(0, "cmd_type: %d\n", cmd_type);

  /* 根据命令类型分发处理 */
  switch (cmd_type) {
    case AT_CMD_TEST:
    case AT_CMD_GMR:
    case AT_CMD_WIFI_SCAN:
    case AT_CMD_WIFI_CUR_AP:
    case AT_CMD_WIFI_STATUS:
    case AT_CMD_NET_CONFIG:
    case AT_CMD_NET_DHCP:
    case AT_CMD_BLE_SCAN_RESULT:
    case AT_CMD_TCP_SEND:
    case AT_CMD_TCP_STATUS:
      /* 简单命令，直接转发 */
      SendToESP32C3(array, len);
      break;

    case AT_CMD_WIFI_CONNECT:
      HandleWiFiConnect(array, len);
      break;

    case AT_CMD_WIFI_DISCONNECT:
      HandleWiFiDisconnect(array, len);
      break;

    case AT_CMD_BLE_SCAN_START:
      HandleBLEScanStart(array, len);
      break;

    case AT_CMD_BLE_SCAN_STOP:
      HandleBLEScanStop(array, len);
      break;

    case AT_CMD_TCP_CONNECT:
      HandleTCPConnect(array, len);
      break;

    case AT_CMD_TCP_CLOSE:
      HandleTCPClose(array, len);
      break;

    case AT_CMD_TCP_TRANSPARENT:
      HandleTCPTransparent(array, len);
      break;

    case AT_CMD_UNKNOWN:
    default:
      /* 未知命令，直接转发到ESP32C3 */
      SendToESP32C3(array, len);
      break;
  }
}
