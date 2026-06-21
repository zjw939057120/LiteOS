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

/* AT命令结构体 */
typedef struct {
  AT_CMD_TYPE type;
  uint8_t cmd_buf[256];
  uint32_t cmd_len;
} AT_CMD;

/* ESP32C3状态 */
static volatile ESP32_STATE g_esp32_state = ESP32_STATE_IDLE;

/* 透传模式标志 */
static volatile uint8_t g_transparent_mode = 0;

/* TCP连接ID */
static volatile uint8_t g_tcp_link_id = 0;

/* 初始化完成标志 */
static volatile uint8_t g_init_done = 0;



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
  while (*prefix != '\0') {
    if (*str != *prefix) {
      return 0;
    }
    str++;
    prefix++;
  }
  return 1;
}

/* 查找子字符串 */
static uint8_t *StrFind(const uint8_t *haystack, const char *needle)
{
  if (haystack == NULL || needle == NULL) {
    return NULL;
  }
  uint32_t needle_len = 0;
  const char *p = needle;
  while (*p != '\0') {
    needle_len++;
    p++;
  }
  if (needle_len == 0) {
    return (uint8_t *)haystack;
  }
  uint32_t i = 0;
  while (haystack[i] != '\0') {
    uint32_t j = 0;
    while (j < needle_len && haystack[i + j] == (uint8_t)needle[j]) {
      j++;
    }
    if (j == needle_len) {
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
    if (StrFind(cmd, "=0") != NULL) {
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

/* 网络配置 - WiFi连接 */
static void HandleWiFiConnect(const uint8_t *cmd, uint32_t len)
{
  /* 转发AT+CWJAP命令到ESP32C3 */
  /* 格式: AT+CWJAP="ssid","password" */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_WIFI_CONNECTING;
}

/* 网络配置 - WiFi断开 */
static void HandleWiFiDisconnect(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_IDLE;
}

/* 网络配置 - 静态IP设置 */
static void HandleNetConfig(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPSTA=<ip>[,<gateway>[,<netmask>]] */
  SendToESP32C3(cmd, len);
}

/* 网络配置 - DHCP设置 */
static void HandleNetDHCP(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CWDHCP=<mode>,<en> */
  SendToESP32C3(cmd, len);
}

/* 网络配置 - WiFi状态查询 */
static void HandleWiFiStatus(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
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
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_IDLE;
}

/* TCP连接 */
static void HandleTCPConnect(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPSTART=<link_id>,"TCP","ip",<port> */
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_TCP_CONNECTING;
  /* 解析link_id */
  if (len > 12) {
    g_tcp_link_id = cmd[12] - '0';
  }
}

/* TCP发送 */
static void HandleTCPSend(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPSEND=<link_id>,<length> */
  SendToESP32C3(cmd, len);
}

/* TCP关闭 */
static void HandleTCPClose(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
  g_esp32_state = ESP32_STATE_WIFI_CONNECTED;
}

/* TCP状态查询 */
static void HandleTCPStatus(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
}

/* TCP透传模式设置 */
static void HandleTCPTransparent(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPMODE=<mode>  0:正常模式 1:透传模式 */
  SendToESP32C3(cmd, len);
  /* 检查是否启用透传 */
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
void ATResHandle(const uint8_t *array, uint32_t len)
{
  if (array == NULL || len == 0) {
    return;
  } else if (array[0] == 0x00 || len == 1) {
    return;
  } else if (StrStartsWith(array, "\r\nOK") ||
             StrStartsWith(array, "\r\nERROR") ||
             StrStartsWith(array, "\r\nSEND OK") ||
             StrStartsWith(array, "\r\nSEND FAIL") ||
             StrStartsWith(array, "\r\nbusy p")) {
    /* 过滤AT响应状态行 */
    SEGGER_RTT_printf(0, "filter res = %s", &array[2]);
    return;
  } else if  ((StrStartsWith(array, "ATE0") || StrStartsWith(array, "ATE1"))) {
    /* 过滤ATE0和ATE1命令 */
    SEGGER_RTT_printf(0, "filter res = %s", array);
    return;
  } else if (StrStartsWith(array, "\r\nready")) {
    /* 收到ready，每次都关闭AT回显 */
    SendToESP32C3((const uint8_t *)"ATE0\r\n", 6);
    LOS_TaskDelay(20);
    /* 首次初始化WiFi和BLE */
    if (!g_init_done) {
      SendToESP32C3((const uint8_t *)"AT+CWMODE=1\r\n", 13);
      LOS_TaskDelay(20);
      SendToESP32C3((const uint8_t *)"AT+BLEINIT=1\r\n", 14);
      LOS_TaskDelay(20);
      g_init_done = 1;
    }
    return;
  }
  SEGGER_RTT_printf(0, "res = %s", array);

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
    /* 查询WiFi状态 */
    SendToESP32C3((const uint8_t *)"AT+CWSTATE?\r\n", 13);
  }
  if (StrFind(array, "WIFI DISCONNECTED") != NULL) {
    g_esp32_state = ESP32_STATE_IDLE;
    g_transparent_mode = 0;
    /* 查询WiFi状态 */
    SendToESP32C3((const uint8_t *)"AT+CWSTATE?\r\n", 13);
  }
  if (StrFind(array, "CONNECT") != NULL) {
    if (g_esp32_state == ESP32_STATE_TCP_CONNECTING) {
      g_esp32_state = ESP32_STATE_TCP_CONNECTED;
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

/* 处理透传数据发送 */
static void HandleTransparentData(const uint8_t *data, uint32_t len)
{
  if (g_transparent_mode == 1 && g_esp32_state == ESP32_STATE_TCP_TRANSPARENT) {
    /* 透传数据直接发送到ESP32C3 */
    SendToESP32C3(data, len);
  }
}

/* AT命令请求处理入口 */
void ATReqHandle(const uint8_t *array, uint32_t len)
{
  if (array == NULL || len == 0) {
    return;
  }

  /* 在透传模式下，数据直接透传 */
  if (g_transparent_mode == 1 && g_esp32_state == ESP32_STATE_TCP_TRANSPARENT) {
    HandleTransparentData(array, len);
    return;
  }

  /* 解析AT命令类型 */
  AT_CMD_TYPE cmd_type = ParseATCommand(array, len);
  SEGGER_RTT_printf(0,"cmd_type = %d\n", cmd_type);
  /* 根据命令类型分发处理 */
  switch (cmd_type) {
    case AT_CMD_TEST:
      /* AT测试命令，直接转发 */
      SendToESP32C3(array, len);
      break;

    case AT_CMD_GMR:
      /* 版本信息查询，直接转发 */
      SendToESP32C3(array, len);
      break;

    // case AT_CMD_WIFI_MODE:
    //   /* WiFi模式设置，直接转发 */
    //   SendToESP32C3(array, len);
    //   break;

    case AT_CMD_WIFI_SCAN:
      /* 列出可用AP，直接转发 */
      SendToESP32C3(array, len);
      break;

    case AT_CMD_WIFI_CUR_AP:
      /* 查询当前连接的AP，直接转发 */
      SendToESP32C3(array, len);
      break;

    case AT_CMD_WIFI_CONNECT:
      HandleWiFiConnect(array, len);
      break;

    case AT_CMD_WIFI_DISCONNECT:
      HandleWiFiDisconnect(array, len);
      break;

    case AT_CMD_WIFI_STATUS:
      HandleWiFiStatus(array, len);
      break;

    case AT_CMD_NET_CONFIG:
      HandleNetConfig(array, len);
      break;

    case AT_CMD_NET_DHCP:
      HandleNetDHCP(array, len);
      break;

    // case AT_CMD_BLE_INIT:
    //   /* BLE初始化角色，直接转发 */
    //   SendToESP32C3(array, len);
    //   break;

    case AT_CMD_BLE_SCAN_START:
      HandleBLEScanStart(array, len);
      break;

    case AT_CMD_BLE_SCAN_STOP:
      HandleBLEScanStop(array, len);
      break;

    case AT_CMD_TCP_CONNECT:
      HandleTCPConnect(array, len);
      break;

    case AT_CMD_TCP_SEND:
      HandleTCPSend(array, len);
      break;

    case AT_CMD_TCP_CLOSE:
      HandleTCPClose(array, len);
      break;

    case AT_CMD_TCP_STATUS:
      HandleTCPStatus(array, len);
      break;

    case AT_CMD_TCP_TRANSPARENT:
      HandleTCPTransparent(array, len);
      break;

    case AT_CMD_TCP_TRANSMIT:
      /* 透传数据 */
      HandleTransparentData(array, len);
      break;

    case AT_CMD_UNKNOWN:
    default:
      /* 未知命令，直接转发到ESP32C3 */
      SendToESP32C3(array, len);
      break;
  }
}
