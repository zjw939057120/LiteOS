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

 #include "modbus.h"
#include "sensor.h"
#include "usart.h"
#include <string.h>

Modbus g_modbus_485 = {0, 0, 0, 0, 0, UART4};
Modbus g_modbus = {0, 0, 0, 0, 0, UART6};

void ResetModbus(Modbus *modbus) {
  //   modbus->address = 0;
  modbus->func_code = 0;
  modbus->reg_addr = 0;
  modbus->reg_number = 0;
  modbus->crc_sum = 0;
}

void DecodeModbusData(const uint8_t *array, Modbus *modbus) {
  modbus->address = array[0];
  modbus->func_code = array[1];
  modbus->reg_addr = toolkit_uint16_little(array + 2);
  modbus->reg_number = toolkit_uint16_little(array + 4);
  modbus->crc_sum = toolkit_uint16_little(array + 6);
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
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode01(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode02(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}

// 0x01, 0x03, 0x12, 0x08, 0x2E, 0x00, 0x0D, 0x00,
// 0xFD, 0x00, 0x05, 0x00, 0x05, 0x00, 0xE7, 0x01,
// 0x6D, 0x00, 0x05, 0x00, 0x7F, 0xD7, 0x9B
void handleModbusDataByFuncCode03(const Modbus *modbus) {
  Sonsor_meter Sonsor_meter = {0};
  Sonsor_meter.DeVadd = modbus->address;
  Sonsor_meter.Functioncode = modbus->func_code;
  Sonsor_meter.len = modbus->reg_number * 2;
  Sonsor_meter.CO2_Sonsor_data = toolkit_swap_uint16(g_sensor.CO2);
  Sonsor_meter.CH2O_Sonsor_data = toolkit_swap_uint16(g_sensor.CH2O);
  Sonsor_meter.TVOC_Sonsor_data = toolkit_swap_uint16(g_sensor.TVOC);
  Sonsor_meter.PM25_Sonsor_data = toolkit_swap_uint32(g_sensor.PM25);
  Sonsor_meter.PM100_Sonsor_data = toolkit_swap_uint32(g_sensor.PM100);
  g_sensor.TEMP = 0;
  g_sensor.RH = 0;
  Sonsor_meter.TEMP_Sonsor_data = toolkit_swap_uint16(g_sensor.TEMP);
  Sonsor_meter.RH_Sonsor_data = toolkit_swap_uint16(g_sensor.RH);
  Sonsor_meter.PM10_Sonsor_data = toolkit_swap_uint32(g_sensor.PM10);
  Sonsor_meter.Sonsor_Type = toolkit_swap_uint16(0x7F);
  Sonsor_meter.crc_sum = toolkit_swap_uint16(
      Crc_Cal((uint8_t *)&Sonsor_meter,
              sizeof(Sonsor_meter) - sizeof(Sonsor_meter.crc_sum)));
  // SEGGER_RTT_printf_hex((uint8_t *)&Sonsor_meter, sizeof(Sonsor_meter));
  Seria_SendArray(modbus->uart, (uint8_t *)&Sonsor_meter, sizeof(Sonsor_meter));
}
void handleModbusDataByFuncCode04(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode05(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode06(const Modbus *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}