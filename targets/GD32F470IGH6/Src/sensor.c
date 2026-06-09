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

Sensor g_sensor = {0};
Modbus g_modbus_485 = {0, 0, 0, 0, 0, UART4};
Modbus g_modbus = {0, 0, 0, 0, 0, UART6};

void ResetSensorTVOC(Sensor *sensor) { sensor->TVOC = 0; }
void ResetSensorPPB(Sensor *sensor) { sensor->PPB = 0; }
void ResetSensorCO2(Sensor *sensor) { sensor->CO2 = 0; }
void ResetSensorPM10(Sensor *sensor) { sensor->PM10 = 0; }
void ResetSensorPM25(Sensor *sensor) { sensor->PM25 = 0; }
void ResetSensorPM100(Sensor *sensor) { sensor->PM100 = 0; }
void ResetModbus(Modbus *modbus) {
  //   modbus->address = 0;
  modbus->func_code = 0;
  modbus->reg_addr = 0;
  modbus->reg_number = 0;
  modbus->crc_sum = 0;
}

void DecodeSensorDataTVOC(const uint8_t *array, Sensor *sensor) {
  // TVOC(ug/m3) = Data[8]*256+Data[9]
  sensor->TVOC = bl_t_uint16(array + 8);
}
void DecodeSensorDataPPB(const uint8_t *array, Sensor *sensor) {
  // PPB = (Data[2]*256+Data[3]),PPM= PPB/1000
  sensor->PPB = bl_t_uint16(array + 2);
}
void DecodeSensorDataCO2(const uint8_t *array, Sensor *sensor) {
  // CO2 = (Data[3]*256+Data[4])
  sensor->CO2 = bl_t_uint16(array + 3);
}
void DecodeSensorDataPM10(const uint8_t *array, Sensor *sensor) {
  // PM1.0 GRIMM = DF1*256^3 + DF2*256^2 + DF3*256^1 + DF4
  sensor->PM10 = bl_t_uint32(array + 3);
}
void DecodeSensorDataPM25(const uint8_t *array, Sensor *sensor) {
  // PM2.5 GRIMM = DF5*256^3 + DF6*256^2 + DF7*256^1 + DF8
  sensor->PM25 = bl_t_uint32(array + 7);
}
void DecodeSensorDataPM100(const uint8_t *array, Sensor *sensor) {
  // PM100 GRIMM = DF9*256^3 + DF10*256^2 + DF11*256^1 + DF12
  sensor->PM100 = bl_t_uint32(array + 11);
}
void DecodeModbusData(const uint8_t *array, Modbus *modbus) {
  modbus->address = array[0];
  modbus->func_code = array[1];
  modbus->reg_addr = bl_t_uint16(array + 2);
  modbus->reg_number = bl_t_uint16(array + 4);
  modbus->crc_sum = bl_t_uint16(array + 6);
}

void handleModbusData(const Modbus *modbus) {
  switch (modbus->func_code) {
  case 0x00:
    handleModbusDataByFuncCode00(modbus);
    break;
  case 0x01:
    handleModbusDataByFuncCode01(modbus);
    break;
  case 0x02:
    handleModbusDataByFuncCode02(modbus);
    break;
  case 0x03:
    handleModbusDataByFuncCode03(modbus);
    break;
  case 0x04:
    handleModbusDataByFuncCode04(modbus);
    break;
  case 0x05:
    handleModbusDataByFuncCode05(modbus);
    break;
  case 0x06:
    handleModbusDataByFuncCode06(modbus);
    break;
  default:
    break;
  }
}
void handleModbusDataByFuncCode00(const Modbus *modbus) {
  // 读取寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode01(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode02(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode03(const Modbus *modbus) {
  // 读取寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode04(const Modbus *modbus) { 
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode05(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}
void handleModbusDataByFuncCode06(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n",__func__, modbus->func_code);
}