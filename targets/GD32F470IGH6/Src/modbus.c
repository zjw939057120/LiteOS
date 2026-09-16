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

ModbusReq g_modbus_hmi = {0, 0, 0, 0, 0, true};
ModbusReq g_modbus = {0, 0, 0, 0, 0, false};
Sonsor_meter g_sonsor_meter = {0};

void ResetModbus(ModbusReq *modbus) {
  // modbus->address = g_modbus_address;
  modbus->func_code = 0;
  modbus->reg_addr = 0;
  modbus->reg_number = 0;
  modbus->crc_sum = 0;
}

bool DecodeModbusData(const uint8_t *array, uint32_t len, ModbusReq *modbus) {
  if (array[0] != g_uart_config.addr) {
    // 地址不匹配
    return false;
  }else if (len < 8) {
    // 数据长度不足
    return false;
  }
  modbus->address = array[0];
  modbus->func_code = array[1];
  modbus->reg_addr = swap_uint16_array(array + 2);
  modbus->reg_number = swap_uint16_array(array + 4);
  modbus->crc_sum = swap_uint16_array(array + 6);
  return true;
}

void ModbusHandle(const uint8_t *array, uint32_t len, ModbusReq *modbus){
  // 解码Modbus数据
  if (!DecodeModbusData(array, len, modbus)) {
    return;
   }
  handleModbusData(modbus);
}
void handleModbusData(const ModbusReq *modbus) {
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
void handleModbusDataByFuncCode00(const ModbusReq *modbus) {
  // 读取寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode01(const ModbusReq *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode02(const ModbusReq *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}

//所有寄存器数量
const uint16_t all_regs_count = sizeof(Sonsor_meter) / 2;

  // 读取寄存器数据
uint16_t getModbusRegister(uint16_t addr) {
  switch (addr) {
  case 0:
    return g_sensor.CO2;
  case 1:
    return g_sensor.CH2O;
  case 2:
    return g_sensor.TVOC;
  case 3:
    return g_sensor.PM25;
  case 4:
    return g_sensor.PM100;
  case 5:
    return g_sensor.TEMP;
  case 6:
    return g_sensor.RH;
  case 7:
    return g_sensor.PM10;
  case 8:
    return g_sensor.TYPE;
  case 9:
    return g_ble_sensor_data.temp[0];
  case 10:
    return g_ble_sensor_data.humi[0];
  case 11:
    return g_ble_sensor_data.temp[1];
  case 12:
    return g_ble_sensor_data.humi[1];
  case 13:
    return g_ble_sensor_data.temp[2];
  case 14:
    return g_ble_sensor_data.humi[2];
  case 15:
    return g_ble_sensor_data.temp[3];
  case 16:
    return g_ble_sensor_data.humi[3];
  case 17:
    return g_ble_sensor_data.temp[4];
  case 18:
    return g_ble_sensor_data.humi[4];
  case 19:
    return g_ble_sensor_data.temp[5];
  case 20:
    return g_ble_sensor_data.humi[5];
  case 21:
    return g_ble_sensor_data.temp[6];
  case 22:
    return g_ble_sensor_data.humi[6];
  case 23:
    return g_ble_sensor_data.temp[7];
  case 24:
    return g_ble_sensor_data.humi[7];
  case 25:
    return g_ble_sensor_data.temp[8];
  case 26:
    return g_ble_sensor_data.humi[8];
  case 27:
    return g_ble_sensor_data.temp[9];
  case 28:
    return g_ble_sensor_data.humi[9];
  case 29:
    return g_ble_sensor_data.wifi_status;
  case 30:
    return abs(g_ble_sensor_data.wifi_rssi);
  case 31:
    return g_sonsor_meter.screen_version;
  case 32:
    return g_sonsor_meter.system_version;
  case 33:
    return g_sonsor_meter.network_version;
  default:
    return 0;
  }
}


void handleModbusDataByFuncCode03(const ModbusReq *modbus) {
  uint16_t startAddr = modbus->reg_addr;// 起始地址
  uint16_t quantity = modbus->reg_number;// 读取寄存器数量

  if (startAddr > all_regs_count) {
    // 超出范围
    return;
  }else if (startAddr + quantity > all_regs_count) {
    // 超出范围
    return;
  }

  uint8_t response[256];
  int responseLen = 0;

  // 构建响应
  response[0] = modbus->address;// 地址
  response[1] = modbus->func_code;// 功能码
  response[2] = quantity * 2; // 字节数
  for (int i = 0; i < quantity; i++) {
     uint16_t value = getModbusRegister(startAddr + i);
     response[3 + i * 2] = (value >> 8) & 0xFF;
     response[4 + i * 2] = value & 0xFF;
    }
    responseLen = 3 + quantity * 2;
    // CRC校验
    uint16_t crc = Crc_Cal(response, responseLen);
    response[responseLen] = crc >> 8;
    response[responseLen + 1] = crc & 0xFF;
    responseLen += 2;
  // if(!modbus->is_hmi)
  //   SEGGER_RTT_printf_hex(response, responseLen);
  sendModbusData(response, responseLen, modbus->is_hmi);
}
void handleModbusDataByFuncCode04(const ModbusReq *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode05(const ModbusReq *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void handleModbusDataByFuncCode06(const ModbusReq *modbus) {
  // 写入寄存器数据
  SEGGER_RTT_printf(0, "%s %d\n", __func__, modbus->func_code);
}
void sendModbusData(const uint8_t *Array, uint16_t Length, bool is_hmi) {
  // 如果是HMI请求，发送到UART6
  if (is_hmi) {
    Seria_SendArray(UART6, Array, Length);
    return;
  }
  // 如果不是HMI请求，发送到UART4
  rs485_en(true);
  LOS_TaskDelay(5);
  Seria_SendArray(UART4, Array, Length);
  LOS_TaskDelay(5);
  rs485_en(false);
  Seria_SendArray(UART4, Array, Length);
}
void rs485_en(bool enable) {
  gpio_bit_write(RS485_EN_PORT, RS485_EN_PIN, enable ? SET : RESET);
}

