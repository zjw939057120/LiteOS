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
#include "usart.h"
#include "gpio.h"
#include "esp32c3.h"

// Modbus地址
UartConfig_t g_uart_config = {9600, 8, 1, 0, 0};

Modbus g_modbus_485 = {0, 0, 0, 0, 0, true};
Modbus g_modbus = {0, 0, 0, 0, 0, false};
Sonsor_meter g_sonsor_meter = {0};

void ResetModbus(Modbus *modbus) {
  // modbus->address = g_modbus_address;
  modbus->func_code = 0;
  modbus->reg_addr = 0;
  modbus->reg_number = 0;
  modbus->crc_sum = 0;
}

bool DecodeModbusData(const uint8_t *array, uint32_t len, Modbus *modbus) {
  if (array[0] != g_uart_config.addr) {
    // 地址不匹配
    return false;
  }else if (len < 8) {
    // 数据长度不足
    return false;
  }
  modbus->address = array[0];
  modbus->func_code = array[1];
  modbus->reg_addr = toolkit_uint16_little(array + 2);
  modbus->reg_number = toolkit_uint16_little(array + 4);
  modbus->crc_sum = toolkit_uint16_little(array + 6);
  return true;
}

void ModbusHandle(const uint8_t *array, uint32_t len, Modbus *modbus){
  // 解码Modbus数据
  if (!DecodeModbusData(array, len, modbus)) {
    return;
   }
  handleModbusData(modbus);
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
  g_sonsor_meter.DeVadd = modbus->address;
  g_sonsor_meter.Functioncode = modbus->func_code;
  g_sonsor_meter.len = modbus->reg_number * 2;
  g_sonsor_meter.CO2_Sonsor_data = toolkit_swap_uint16(g_sensor.CO2);
  g_sonsor_meter.CH2O_Sonsor_data = toolkit_swap_uint16(g_sensor.CH2O);
  g_sonsor_meter.TVOC_Sonsor_data = toolkit_swap_uint16(g_sensor.TVOC);
  g_sonsor_meter.PM25_Sonsor_data = toolkit_swap_uint32(g_sensor.PM25);
  g_sonsor_meter.PM100_Sonsor_data = toolkit_swap_uint32(g_sensor.PM100);
  g_sonsor_meter.TEMP_Sonsor_data = toolkit_swap_uint16(g_sensor.TEMP);
  g_sonsor_meter.RH_Sonsor_data = toolkit_swap_uint16(g_sensor.RH);
  g_sonsor_meter.PM10_Sonsor_data = toolkit_swap_uint32(g_sensor.PM10);
  g_sonsor_meter.Sonsor_Type = toolkit_swap_uint16(g_sensor.TYPE);
  g_sonsor_meter.TEMP_Sonsor_data_1 = toolkit_swap_uint16(g_ble_sensor_data.temp[0]);
  g_sonsor_meter.RH_Sonsor_data_1 = toolkit_swap_uint16(g_ble_sensor_data.humi[0]);
  g_sonsor_meter.TEMP_Sonsor_data_2 = toolkit_swap_uint16(g_ble_sensor_data.temp[1]);
  g_sonsor_meter.RH_Sonsor_data_2 = toolkit_swap_uint16(g_ble_sensor_data.humi[1]);
  g_sonsor_meter.TEMP_Sonsor_data_3 = toolkit_swap_uint16(g_ble_sensor_data.temp[2]);
  g_sonsor_meter.RH_Sonsor_data_3 = toolkit_swap_uint16(g_ble_sensor_data.humi[2]);
  g_sonsor_meter.TEMP_Sonsor_data_4 = toolkit_swap_uint16(g_ble_sensor_data.temp[3]);
  g_sonsor_meter.RH_Sonsor_data_4 = toolkit_swap_uint16(g_ble_sensor_data.humi[3]);
  g_sonsor_meter.TEMP_Sonsor_data_5 = toolkit_swap_uint16(g_ble_sensor_data.temp[4]);
  g_sonsor_meter.RH_Sonsor_data_5 = toolkit_swap_uint16(g_ble_sensor_data.humi[4]);
  g_sonsor_meter.TEMP_Sonsor_data_6 = toolkit_swap_uint16(g_ble_sensor_data.temp[5]);
  g_sonsor_meter.RH_Sonsor_data_6 = toolkit_swap_uint16(g_ble_sensor_data.humi[5]);
  g_sonsor_meter.TEMP_Sonsor_data_7 = toolkit_swap_uint16(g_ble_sensor_data.temp[6]);
  g_sonsor_meter.RH_Sonsor_data_7 = toolkit_swap_uint16(g_ble_sensor_data.humi[6]);
  g_sonsor_meter.TEMP_Sonsor_data_8 = toolkit_swap_uint16(g_ble_sensor_data.temp[7]);
  g_sonsor_meter.RH_Sonsor_data_8 = toolkit_swap_uint16(g_ble_sensor_data.humi[7]);
  g_sonsor_meter.TEMP_Sonsor_data_9 = toolkit_swap_uint16(g_ble_sensor_data.temp[8]);
  g_sonsor_meter.RH_Sonsor_data_9 = toolkit_swap_uint16(g_ble_sensor_data.humi[8]);
  g_sonsor_meter.TEMP_Sonsor_data_10 = toolkit_swap_uint16(g_ble_sensor_data.temp[9]);
  g_sonsor_meter.RH_Sonsor_data_10 = toolkit_swap_uint16(g_ble_sensor_data.humi[9]);
  g_sonsor_meter.wifi_status = toolkit_swap_uint16(g_ble_sensor_data.wifi_status);
  g_sonsor_meter.wifi_rssi = toolkit_swap_uint16(abs(g_ble_sensor_data.wifi_rssi));
  g_sonsor_meter.crc_sum = toolkit_swap_uint16(
      Crc_Cal((uint8_t *)&g_sonsor_meter,
              sizeof(g_sonsor_meter) - sizeof(g_sonsor_meter.crc_sum)));
  // SEGGER_RTT_printf_hex((uint8_t *)&g_sonsor_meter, sizeof(g_sonsor_meter));
  sendModbusData(modbus->is_485, (uint8_t *)&g_sonsor_meter, sizeof(g_sonsor_meter));
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
void sendModbusData(bool is_485, const uint8_t *Array, uint16_t Length) {
  if (is_485) {
    rs485_en(true);
    LOS_TaskDelay(5);
    Seria_SendArray(UART4, Array, Length);
    LOS_TaskDelay(5);
    rs485_en(false);
    return;
  }
  Seria_SendArray(UART6, Array, Length);
}
void rs485_en(bool enable) {
  gpio_bit_write(RS485_EN_PORT, RS485_EN_PIN, enable ? SET : RESET);
}

