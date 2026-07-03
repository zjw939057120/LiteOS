/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Timer Driver Initialization HeadFile
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MODBUS_H
#define _MODBUS_H

#include "los_typedef.h"
#include "platform.h"
#include "sensor.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct __attribute__((packed)) // 结构体内存紧凑
{
  uint8_t DeVadd;               // 设备地址
  uint8_t Functioncode;         // 功能码
  uint8_t len;                  // 字节数
  uint16_t CO2_Sonsor_data;     // CO2传感器数据
  uint16_t CH2O_Sonsor_data;    // CH2O传感器数据
  uint16_t TVOC_Sonsor_data;    // TVOC传感器数据
  uint16_t PM25_Sonsor_data;    // PM2.5传感器数据
  uint16_t PM100_Sonsor_data;   // PM10传感器数据
  uint16_t TEMP_Sonsor_data;    // TEMP传感器数据
  uint16_t RH_Sonsor_data;      // RH传感器数据
  uint16_t PM10_Sonsor_data;    // PM1.0传感器数据
  uint16_t Sonsor_Type;         // 传感器类型
  uint16_t TEMP_Sonsor_data_1;  // 1 号传感器温度(×10 → 0.1℃)
  uint16_t RH_Sonsor_data_1;    // 1 号传感器湿度(×10 → 0.1%)
  uint16_t TEMP_Sonsor_data_2;  // 2 号传感器温度
  uint16_t RH_Sonsor_data_2;    // 2 号传感器湿度
  uint16_t TEMP_Sonsor_data_3;  // 3 号传感器温度
  uint16_t RH_Sonsor_data_3;    // 3 号传感器湿度
  uint16_t TEMP_Sonsor_data_4;  // 4 号传感器温度
  uint16_t RH_Sonsor_data_4;    // 4 号传感器湿度
  uint16_t TEMP_Sonsor_data_5;  // 5 号传感器温度
  uint16_t RH_Sonsor_data_5;    // 5 号传感器湿度
  uint16_t TEMP_Sonsor_data_6;  // 6 号传感器温度
  uint16_t RH_Sonsor_data_6;    // 6 号传感器湿度
  uint16_t TEMP_Sonsor_data_7;  // 7 号传感器温度
  uint16_t RH_Sonsor_data_7;    // 7 号传感器湿度
  uint16_t TEMP_Sonsor_data_8;  // 8 号传感器温度
  uint16_t RH_Sonsor_data_8;    // 8 号传感器湿度
  uint16_t TEMP_Sonsor_data_9;  // 9 号传感器温度
  uint16_t RH_Sonsor_data_9;    // 9 号传感器湿度
  uint16_t TEMP_Sonsor_data_10; // 10 号传感器温度
  uint16_t RH_Sonsor_data_10;   // 10 号传感器湿度
  uint16_t wifi_status; // wifi状态位
  uint16_t wifi_rssi;   // wifi信号强度
  uint16_t crc_sum;     // CRC校验和
} Sonsor_meter;

extern Sonsor_meter g_sonsor_meter;
// RS485串口通讯

// esp32c3串口通讯

typedef struct {
  // SWM34SMEU6-QFN80串口通讯
  uint8_t address;   // 设备地址
  uint8_t func_code; // 功能码
  uint16_t reg_addr;   // 寄存器地址
  uint16_t reg_number; // 寄存器数量
  uint16_t crc_sum;    // CRC校验码
  uint32_t uart;       // 串口外设
} Modbus;

extern Modbus g_modbus_485;
extern Modbus g_modbus;

void ResetModbus(Modbus *modbus);
void DecodeModbusData(const uint8_t *array, Modbus *modbus);

void ModbusHandle(const uint8_t *array, Modbus *modbus);
void handleModbusData(const Modbus *modbus);
void handleModbusDataByFuncCode00(const Modbus *modbus);
void handleModbusDataByFuncCode01(const Modbus *modbus);
void handleModbusDataByFuncCode02(const Modbus *modbus);
void handleModbusDataByFuncCode03(const Modbus *modbus);
void handleModbusDataByFuncCode04(const Modbus *modbus);
void handleModbusDataByFuncCode05(const Modbus *modbus);
void handleModbusDataByFuncCode06(const Modbus *modbus);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _MODBUS_H */
