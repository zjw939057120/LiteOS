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
#ifndef _SENSOR_H
#define _SENSOR_H

#include "los_typedef.h"
#include "platform.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
  // 空气质量传感器MS-VOC-V4
  uint16_t TVOC; // TVOC

  // 甲醛传感器SC11-CH2O
  uint16_t PPB; // PPB

  // 红外二氧化碳传感器CM1106S
  uint16_t CO2; // CO2

  // 激光粉尘传感器PM2012SE
  uint16_t PM10;  // PM1.0 GRIMM
  uint16_t PM25;  // PM2.5 GRIMM
  uint16_t PM100; // PM100 GRIMM
} Sensor;

// RS485串口通讯

// esp32c3串口通讯

typedef struct {
  // SWM34SMEU6-QFN80串口通讯
  uint8_t address;   // 设备地址
  uint8_t func_code; // 功能码
  // uint8_t len = 0x0;          // 字节数
  uint16_t reg_addr;   // 寄存器地址
  uint16_t reg_number; // 寄存器数量
  uint16_t crc_sum;    // CRC校验码
  uint32_t uart; // 串口外设
} Modbus;

extern Sensor g_sensor;

extern Modbus g_modbus_485;
extern Modbus g_modbus;

void ResetSensorTVOC(Sensor *sensor);
void ResetSensorPPB(Sensor *sensor);
void ResetSensorCO2(Sensor *sensor);
void ResetSensorPM10(Sensor *sensor);
void ResetSensorPM25(Sensor *sensor);
void ResetSensorPM100(Sensor *sensor);
void ResetModbus(Modbus *modbus);

void DecodeSensorDataTVOC(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPPB(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataCO2(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM10(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM25(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM100(const uint8_t *array, Sensor *sensor);
void DecodeModbusData(const uint8_t *array, Modbus *modbus);

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

#endif /* _SENSOR_H */
