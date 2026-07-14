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
#include "toolkit.h"
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

//传感器类型
#define CO2_Sensor   0x01
#define HH2O_Sensor  0x02
#define TVOC_Sensor  0x04
#define PM25_Sensor  0x08
#define PM100_Sensor 0x10
#define SHT_Sensor   0x20
#define HMT_Sensor   0x40

// 传感器数据头
enum SensorDataHeader {
  TVOC_Sensor_Data_Header = 0xFF,
  CH2O_Sensor_Data_Header = 0xFF,
  CO2_Sensor_Data_Header = 0x16,
  PM_Sensor_Data_Header = 0x16,
};

// 传感器数据长度
enum SensorDataLength {
  TVOC_Sensor_Data_Length = 13,
  CH2O_Sensor_Data_Length = 9,
  CO2_Sensor_Data_Length = 8,
  PM_Sensor_Data_Length = 56,
};

typedef struct {
  // 红外二氧化碳传感器CM1106S
  uint16_t CO2; // CO2
  // 甲醛传感器SC11-CH2O
  uint16_t CH2O; // CH2O
  // 空气质量传感器MS-VOC-V4
  uint16_t TVOC; // TVOC
  // 激光粉尘传感器PM2012SE
  uint16_t PM25;  // PM2.5 GRIMM
  uint16_t PM100; // PM10 GRIMM
  // 温度传感器
  uint16_t TEMP;
  // 湿度传感器
  uint16_t RH;
  // 激光粉尘传感器PM2012SE
  uint16_t PM10;  // PM1.0 GRIMM
  // 传感器类型
  uint8_t TYPE;
} Sensor;

extern Sensor g_sensor;

void DecodeSensorDataTVOC(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataCH2O(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataCO2(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM10(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM25(const uint8_t *array, Sensor *sensor);
void DecodeSensorDataPM100(const uint8_t *array, Sensor *sensor);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _SENSOR_H */
