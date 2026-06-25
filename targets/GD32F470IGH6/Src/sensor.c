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

void DecodeSensorDataTVOC(const uint8_t *array, Sensor *sensor) {
  // TVOC(ug/m3) = Data[8]*256+Data[9]
  sensor->TVOC = toolkit_uint16_little(array + 8);
  // 传感器类型
  sensor->TYPE |= TVOC_Sensor;
}

void DecodeSensorDataCH2O(const uint8_t *array, Sensor *sensor) {
  // CH2O = (Data[2]*256+Data[3]),PPM= PPB/1000
  sensor->CH2O = toolkit_uint16_little(array + 2);
  // 传感器类型
  sensor->TYPE |= HH2O_Sensor;
}

void DecodeSensorDataCO2(const uint8_t *array, Sensor *sensor) {
  // CO2 = (Data[3]*256+Data[4])
  sensor->CO2 = toolkit_uint16_little(array + 3);
  // 传感器类型
  sensor->TYPE |= CO2_Sensor;
}

void DecodeSensorDataPM10(const uint8_t *array, Sensor *sensor) {
  // PM1.0 GRIMM = DF1*256^3 + DF2*256^2 + DF3*256^1 + DF4
  // 传感器类型
  sensor->PM10 = toolkit_uint32_little(array + 3);
}

void DecodeSensorDataPM25(const uint8_t *array, Sensor *sensor) {
  // PM2.5 GRIMM = DF5*256^3 + DF6*256^2 + DF7*256^1 + DF8
  sensor->PM25 = toolkit_uint32_little(array + 7);
  // 传感器类型
  sensor->TYPE |= PM25_Sensor;
}

void DecodeSensorDataPM100(const uint8_t *array, Sensor *sensor) {
  // PM10 GRIMM = DF9*256^3 + DF10*256^2 + DF11*256^1 + DF12
  sensor->PM100 = toolkit_uint32_little(array + 11);
  // 传感器类型
  sensor->TYPE |= PM100_Sensor;
}
