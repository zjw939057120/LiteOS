/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Timer Driver Initialization Implementation
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

#include "sensor.h"
#include "usart.h"
#include <string.h>


Sensor g_sensor = {0};

// TVOC校准
// 第一步：扣除零点漂移
// x' = x - 0.87

// // 第二步：根据数值范围，选择对应的公式
// IF (x' <= 0.39) THEN  // 对应标准值 0.00 - 4.00 区间
//     y = (4.00 - 0.00) / 0.39 * x' + 0.00
//     // 简化公式：y = 10.2564 * x'

// ELSE IF (x' <= 0.83) THEN  // 对应标准值 4.00 - 10.00 区间
//     y = (10.00 - 4.00) / (0.83 - 0.39) * (x' - 0.39) + 4.00
//     // 简化公式：y = 13.6364 * (x' - 0.39) + 4.00

// ELSE IF (x' <= 1.24) THEN  // 对应标准值 10.00 - 16.00 区间
//     y = (16.00 - 10.00) / (1.24 - 0.83) * (x' - 0.83) + 10.00
//     // 简化公式：y = 14.6341 * (x' - 0.83) + 10.00
    
// END IF

// 限制条件：如果最终计算结果 y < 0，则令 y = 0
static uint16_t TVOC_calibrate(uint16_t TVOC) {
  // SEGGER_RTT_printf(0, "TVOC = %d\n", TVOC);
  int32_t tmp = (int32_t)TVOC - 870;
  int32_t ret = 0;
 if (tmp <= 390) {
    ret = 102564 * tmp;
  }
  else if (tmp <= 830) {
    ret = 136364 * (tmp - 390) + 400000;
  }
  else if (tmp <= 1240) {
    ret = 146341 * (tmp - 830) + 100000;
  }
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

// CH2O校准,
// 实测值 ≥ 0.74时标准浓度值 = 1.1101 × 实测值 + 0.2152
// 实测值 < 0.74时标准浓度值 = 1.4554 × 实测值 - 0.3530
static uint16_t CH2O_calibrate(uint16_t CH2O) {
  // SEGGER_RTT_printf(0, "CH2O = %d\n", CH2O);
  int32_t ret = 0;
  if (CH2O >= 740) {
    ret = 11101 * (int32_t)CH2O + 2152;
  } else {
    ret = 14554 * (int32_t)CH2O - 3530;
  }
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

// CO2校
// 准标准浓度值 = (仪器实测值 - 400) × 1.1275
static uint16_t CO2_calibrate(uint16_t CO2) {
  // SEGGER_RTT_printf(0, "CO2 = %d\n", CO2);
  int32_t ret = 0;
  ret = ((int32_t)CO2 - 400) * 11275;
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

// PM10校准
// 标准浓度值 = 7.1728 × 实测值 - 27.893
static uint16_t PM10_calibrate(uint16_t PM10) {
  // SEGGER_RTT_printf(0, "PM10 = %d\n", PM10);
  int32_t ret = 0;
  ret = 71728 * (int32_t)PM10 - 278930;
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

// PM25校准
// 标准浓度值 = 7.1728 × 实测值 - 27.893
static uint16_t PM25_calibrate(uint16_t PM25) {
  // SEGGER_RTT_printf(0, "PM25 = %d\n", PM25);
  int32_t ret = 0;
  ret = 71728 * (int32_t)PM25 - 278930;
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

// PM100校准
// 标准浓度值 = 7.1728 × 实测值 - 27.893
static uint16_t PM100_calibrate(uint16_t PM100) {
  // SEGGER_RTT_printf(0, "PM100 = %d\n", PM100);
  int32_t ret = 0;
  ret = 71728 * (int32_t)PM100 - 278930;
  return (uint16_t)(ret < 0 ? 0 : ret/10000);
}

void DecodeSensorDataTVOC(const uint8_t *array, Sensor *sensor) {
  // TVOC(ug/m3) = Data[8]*256+Data[9]
  sensor->TVOC = TVOC_calibrate(swap_uint16_array(array + 8));
  // 传感器类型
  sensor->TYPE |= TVOC_Sensor;
}

void DecodeSensorDataCH2O(const uint8_t *array, Sensor *sensor) {
  // CH2O = (Data[2]*256+Data[3]),PPM= PPB/1000
  sensor->CH2O = CH2O_calibrate(swap_uint16_array(array + 2));
  // 传感器类型
  sensor->TYPE |= HH2O_Sensor;
}

void DecodeSensorDataCO2(const uint8_t *array, Sensor *sensor) {
  // CO2 = (Data[3]*256+Data[4])
  sensor->CO2 = CO2_calibrate(swap_uint16_array(array + 3));
  // 传感器类型
  sensor->TYPE |= CO2_Sensor;
}

void DecodeSensorDataPM10(const uint8_t *array, Sensor *sensor) {
  // PM1.0 GRIMM = DF1*256^3 + DF2*256^2 + DF3*256^1 + DF4
  // 传感器类型
  sensor->PM10 = PM10_calibrate(swap_uint32_array(array + 3));
}

void DecodeSensorDataPM25(const uint8_t *array, Sensor *sensor) {
  // PM2.5 GRIMM = DF5*256^3 + DF6*256^2 + DF7*256^1 + DF8
  sensor->PM25 = PM25_calibrate(swap_uint32_array(array + 7));
  // 传感器类型
  sensor->TYPE |= PM25_Sensor;
}

void DecodeSensorDataPM100(const uint8_t *array, Sensor *sensor) {
  // PM10 GRIMM = DF9*256^3 + DF10*256^2 + DF11*256^1 + DF12
  sensor->PM100 = PM100_calibrate(swap_uint32_array(array + 11));
  // 传感器类型
  sensor->TYPE |= PM100_Sensor;
}
