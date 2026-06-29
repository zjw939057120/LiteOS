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

/* AT响应前缀宏定义 */
#define AT_RES_PREFIX_CWLAP     "+CWLAP:"      /* WiFi扫描结果 */
#define AT_RES_PREFIX_CWJAP     "+CWJAP:"      /* 当前连接的AP */
#define AT_RES_PREFIX_CWSTATE   "+CWSTATE:"    /* WiFi状态 */
#define AT_RES_PREFIX_BLESCAN   "+BLESCAN:"    /* 蓝牙扫描结果 */
#define AT_RES_PREFIX_CIPSTATUS "+CIPSTATUS:"  /* TCP状态 */
#define AT_RES_PREFIX_GMR       "+GMR"         /* 版本信息 */

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

/* WiFi状态 */
typedef enum {
  ESP32_WIFI_IDLE = 0,
  ESP32_WIFI_CONNECTING,
  ESP32_WIFI_CONNECTED,
} ESP32_WIFI_STATE;

/* TCP状态 */
typedef enum {
  ESP32_TCP_IDLE = 0,
  ESP32_TCP_CONNECTING,
  ESP32_TCP_CONNECTED,
  ESP32_TCP_TRANSPARENT,
} ESP32_TCP_STATE;

/* BLE状态 */
typedef enum {
  ESP32_BLE_IDLE = 0,
  ESP32_BLE_SCANNING,
} ESP32_BLE_STATE;

/* AT命令结构体 */
typedef struct {
  AT_CMD_TYPE type;
  uint8_t cmd_buf[256];
  uint32_t cmd_len;
} AT_CMD;

/* ESP32C3状态 */
static volatile ESP32_WIFI_STATE g_wifi_state = ESP32_WIFI_IDLE;
static volatile ESP32_TCP_STATE g_tcp_state = ESP32_TCP_IDLE;
static volatile ESP32_BLE_STATE g_ble_state = ESP32_BLE_IDLE;

/* 透传模式标志 */
static volatile uint8_t g_transparent_mode = 0;

/* TCP连接ID */
static volatile uint8_t g_tcp_link_id = 0;



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

/* 网络配置 - WiFi连接 */
static void HandleWiFiConnect(const uint8_t *cmd, uint32_t len)
{
  /* 转发AT+CWJAP命令到ESP32C3 */
  /* 格式: AT+CWJAP="ssid","password" */
  SendToESP32C3(cmd, len);
  g_wifi_state = ESP32_WIFI_CONNECTING;
}

/* 网络配置 - WiFi断开 */
static void HandleWiFiDisconnect(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
  g_wifi_state = ESP32_WIFI_IDLE;
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
  g_ble_state = ESP32_BLE_SCANNING;
}

/* 蓝牙扫描 - 停止 */
static void HandleBLEScanStop(const uint8_t *cmd, uint32_t len)
{
  SendToESP32C3(cmd, len);
  g_ble_state = ESP32_BLE_IDLE;
}

/* TCP连接 */
static void HandleTCPConnect(const uint8_t *cmd, uint32_t len)
{
  /* 格式: AT+CIPSTART=<link_id>,"TCP","ip",<port> */
  SendToESP32C3(cmd, len);
  g_tcp_state = ESP32_TCP_CONNECTING;
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
  g_wifi_state = ESP32_WIFI_CONNECTED;
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
  if (StrFind(cmd, "=1") >= 0) {
    g_transparent_mode = 1;
    g_tcp_state = ESP32_TCP_TRANSPARENT;
  } else {
    g_transparent_mode = 0;
  }
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
  } else {
    SendResponseToUSART6(res, len);
    SEGGER_RTT_printf(0, "res = %s\n", res);
    return;
  }
  if (res == NULL || len == 0) {
    return;
  } else if (res[0] == 0x00 || len == 1) {
    return;
  } else if (StrStartsWith(res, "\r\nOK") || StrStartsWith(res, "\r\nERROR") ||
             StrStartsWith(res, "\r\nSEND OK") ||
             StrStartsWith(res, "\r\nSEND FAIL") ||
             StrStartsWith(res, "\r\nbusy p") ||
             StrStartsWith(res, "ATE0")) {
    /* 过滤AT响应状态行和回显命令 */
    SEGGER_RTT_printf(0, "filter res = %s", res);
    return;
  } else if (StrStartsWith(res, "\r\nready")) {
    /* 收到ready，每次都重新初始化 */
    // 断开与 AP 的连接
    SendToESP32C3((const uint8_t *)"AT+CWQAP\r\n", 10);
    LOS_TaskDelay(300);
    // Bluetooth LE 初始化
    SendToESP32C3((const uint8_t *)"AT+BLEINIT=1\r\n", 14);
    LOS_TaskDelay(300);
    // 设置 Bluetooth扫描参数
    SendToESP32C3((const uint8_t *)"AT+BLESCANPARAM=1,0,0,100,99\r\n", 30);
    LOS_TaskDelay(300);
    // 关闭AT回显
    SendToESP32C3((const uint8_t *)"ATE0\r\n", 6);
    LOS_TaskDelay(300);
    // 设置 Wi-Fi 模式
    SendToESP32C3((const uint8_t *)"AT+CWMODE=1\r\n", 13);
    LOS_TaskDelay(300);
    // // 设置 AT+CWLAP 命令扫描结果的属性
    SendToESP32C3((const uint8_t *)"AT+CWLAPOPT=,15\r\n", 17);
    LOS_TaskDelay(300);
    //连接至上次 Wi-Fi 配置中的 AP
    SendToESP32C3((const uint8_t *)"AT+CWJAP\r\n", 10);
    LOS_TaskDelay(300);
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

  /* 在透传模式下，数据直接转发到USART6 */
  else if (g_transparent_mode == 1 && g_tcp_state == ESP32_TCP_TRANSPARENT) {
    /* 检查是否是退出透传的特定序列 (+++) */
    if (len == 3 && res[0] == '+' && res[1] == '+' && res[2] == '+') {
      g_transparent_mode = 0;
      g_tcp_state = ESP32_TCP_CONNECTED;
      SendResponseToUSART6(res, len);
      return;
    }
    /* 透传数据直接转发 */
    SendResponseToUSART6(res, len);
    return;
  }

  /* 检查连接状态变化 */
  else if (StrFind(res, "WIFI GOT IP") >= 0) {
    g_wifi_state = ESP32_WIFI_CONNECTED;
    /* 查询WiFi状态 */
    SendToESP32C3((const uint8_t *)"AT+CWSTATE?\r\n", 13);
  } else if (StrFind(res, "WIFI DISCONNECTED") >= 0) {
    g_wifi_state = ESP32_WIFI_IDLE;
    g_tcp_state = ESP32_TCP_IDLE;
    g_transparent_mode = 0;
    /* 查询WiFi状态 */
    SendToESP32C3((const uint8_t *)"AT+CWSTATE?\r\n", 13);
  } else if (StrFind(res, "CONNECT") >= 0) {
    if (g_tcp_state == ESP32_TCP_CONNECTING) {
      g_tcp_state = ESP32_TCP_CONNECTED;
    }
  } else if (StrFind(res, "CLOSED") >= 0) {
    g_tcp_state = ESP32_TCP_IDLE;
    g_transparent_mode = 0;
  } else if (StrFind(res, "+BLESCANDONE") >= 0) {
    g_ble_state = ESP32_BLE_IDLE;
  }

  /* 转发响应到USART6 */
  SendResponseToUSART6(res, len);
}

/* 处理透传数据发送 */
static void HandleTransparentData(const uint8_t *data, uint32_t len)
{
  if (g_transparent_mode == 1 && g_tcp_state == ESP32_TCP_TRANSPARENT) {
    /* 透传数据直接发送到ESP32C3 */
    SendToESP32C3(data, len);
  }
}

/* AT命令请求处理入口 */
void ATRequestHandle(const uint8_t *req, uint32_t len)
{
  if (req == NULL || len == 0) {
    return;
  } else {
    SendToESP32C3(req, len);
    SEGGER_RTT_printf(0, "req = %s\n", req);
    return;
  }

  /* 在透传模式下，数据直接透传 */
  if (g_transparent_mode == 1 && g_tcp_state == ESP32_TCP_TRANSPARENT) {
    HandleTransparentData(req, len);
    return;
  }

  /* 解析AT命令类型 */
  AT_CMD_TYPE cmd_type = ParseATCommand(req, len);
  // 过滤未知AT命令
  if (cmd_type == AT_CMD_UNKNOWN) {
    SEGGER_RTT_printf(0, "filter req = %s\n", req);
    return;
  } else {
    SEGGER_RTT_printf(0, "req = %s, cmd_type = %d\n", req, cmd_type);
  }

  /* WiFi扫描开始，重置计数 */
  if (cmd_type == AT_CMD_WIFI_SCAN) {
    g_wifi_scan_count = 0;
  }

  /* 根据命令类型分发处理 */
  switch (cmd_type) {
    case AT_CMD_TEST:
      /* AT测试命令，直接转发 */
      SendToESP32C3(req, len);
      break;

    case AT_CMD_GMR:
      /* 版本信息查询，直接转发 */
      SendToESP32C3(req, len);
      break;

    // case AT_CMD_WIFI_MODE:
    //   /* WiFi模式设置，直接转发 */
    //   SendToESP32C3(req, len);
    //   break;

    case AT_CMD_WIFI_SCAN:
      /* 列出可用AP，直接转发 */
      SendToESP32C3(req, len);
      break;

    case AT_CMD_WIFI_CUR_AP:
      /* 查询当前连接的AP，直接转发 */
      SendToESP32C3(req, len);
      break;

    case AT_CMD_WIFI_CONNECT:
      HandleWiFiConnect(req, len);
      break;

    case AT_CMD_WIFI_DISCONNECT:
      HandleWiFiDisconnect(req, len);
      break;

    case AT_CMD_WIFI_STATUS:
      HandleWiFiStatus(req, len);
      break;

    case AT_CMD_NET_CONFIG:
      HandleNetConfig(req, len);
      break;

    case AT_CMD_NET_DHCP:
      HandleNetDHCP(req, len);
      break;

    // case AT_CMD_BLE_INIT:
    //   /* BLE初始化角色，直接转发 */
    //   SendToESP32C3(req, len);
    //   break;

    case AT_CMD_BLE_SCAN_START:
      HandleBLEScanStart(req, len);
      break;

    case AT_CMD_BLE_SCAN_STOP:
      HandleBLEScanStop(req, len);
      break;

    case AT_CMD_TCP_CONNECT:
      HandleTCPConnect(req, len);
      break;

    case AT_CMD_TCP_SEND:
      HandleTCPSend(req, len);
      break;

    case AT_CMD_TCP_CLOSE:
      HandleTCPClose(req, len);
      break;

    case AT_CMD_TCP_STATUS:
      HandleTCPStatus(req, len);
      break;

    case AT_CMD_TCP_TRANSPARENT:
      HandleTCPTransparent(req, len);
      break;

    case AT_CMD_TCP_TRANSMIT:
      /* 透传数据 */
      HandleTransparentData(req, len);
      break;

    case AT_CMD_UNKNOWN:
    default:
      /* 未知命令，直接转发到ESP32C3 */
      SendToESP32C3(req, len);
      break;
  }
}
