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

Modbus g_modbus_hmi = {0, 0, 0, 0, 0, true};
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
  modbus->reg_addr = swap_uint16_array(array + 2);
  modbus->reg_number = swap_uint16_array(array + 4);
  modbus->crc_sum = swap_uint16_array(array + 6);
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

  // 头部固定 3 字节: DeVadd + Functioncode + len
  const uint16_t head_size = 3;
  // CRC固定 2 字节
  const uint16_t crc_size = 2;
  // 数据区可读寄存器数: sizeof(Sonsor_meter) 减去头部与 crc_sum
  const uint16_t data_regs = (sizeof(Sonsor_meter) - head_size - crc_size) / 2;

void handleModbusDataByFuncCode03(const Modbus *modbus) {
  // 边界校验：限定 reg_addr / reg_number 不越界
  uint16_t start = modbus->reg_addr;
  uint16_t count = modbus->reg_number;
  if (start >= data_regs) {
    start = 0;
    count = data_regs;
  }
  if (count == 0 || count > data_regs - start) {
    count = data_regs - start;
  }
  g_sonsor_meter.DeVadd = modbus->address;
  g_sonsor_meter.Functioncode = modbus->func_code;
  g_sonsor_meter.len = count * 2;
  // 准备全部寄存器值(按结构体顺序)
  uint16_t regs[data_regs];
  regs[0]  = swap_uint16(g_sensor.CO2);
  regs[1]  = swap_uint16(g_sensor.CH2O);
  regs[2]  = swap_uint16(g_sensor.TVOC);
  regs[3]  = swap_uint32(g_sensor.PM25);
  regs[4]  = swap_uint32(g_sensor.PM100);
  regs[5]  = swap_uint16(g_sensor.TEMP);
  regs[6]  = swap_uint16(g_sensor.RH);
  regs[7]  = swap_uint32(g_sensor.PM10);
  regs[8]  = swap_uint16(g_sensor.TYPE);
  regs[9]  = swap_uint16(g_ble_sensor_data.temp[0]);
  regs[10] = swap_uint16(g_ble_sensor_data.humi[0]);
  regs[11] = swap_uint16(g_ble_sensor_data.temp[1]);
  regs[12] = swap_uint16(g_ble_sensor_data.humi[1]);
  regs[13] = swap_uint16(g_ble_sensor_data.temp[2]);
  regs[14] = swap_uint16(g_ble_sensor_data.humi[2]);
  regs[15] = swap_uint16(g_ble_sensor_data.temp[3]);
  regs[16] = swap_uint16(g_ble_sensor_data.humi[3]);
  regs[17] = swap_uint16(g_ble_sensor_data.temp[4]);
  regs[18] = swap_uint16(g_ble_sensor_data.humi[4]);
  regs[19] = swap_uint16(g_ble_sensor_data.temp[5]);
  regs[20] = swap_uint16(g_ble_sensor_data.humi[5]);
  regs[21] = swap_uint16(g_ble_sensor_data.temp[6]);
  regs[22] = swap_uint16(g_ble_sensor_data.humi[6]);
  regs[23] = swap_uint16(g_ble_sensor_data.temp[7]);
  regs[24] = swap_uint16(g_ble_sensor_data.humi[7]);
  regs[25] = swap_uint16(g_ble_sensor_data.temp[8]);
  regs[26] = swap_uint16(g_ble_sensor_data.humi[8]);
  regs[27] = swap_uint16(g_ble_sensor_data.temp[9]);
  regs[28] = swap_uint16(g_ble_sensor_data.humi[9]);
  regs[29] = swap_uint16(g_ble_sensor_data.wifi_status);
  regs[30] = swap_uint16(abs(g_ble_sensor_data.wifi_rssi));
  // 根据 reg_addr 写入起始位置，连续写入 count 个寄存器
  uint16_t *data_ptr = (uint16_t *)((uint8_t *)&g_sonsor_meter + head_size);
  for (uint16_t i = 0; i < count; i++) {
    data_ptr[i] = regs[start + i];
  }
  // 响应帧: DeVadd + Functioncode + len + 数据区 + crc_sum
  uint8_t *array = (uint8_t *)&g_sonsor_meter;
  // CRC 只覆盖头部 + 数据区，不包含 crc_sum 自身
  g_sonsor_meter.crc_sum = swap_uint16(
      Crc_Cal(array, head_size + g_sonsor_meter.len));
  // 写入校验和(尾部 2 字节)
  *(uint16_t *)(array + head_size + g_sonsor_meter.len) = g_sonsor_meter.crc_sum;
  // 发送Modbus数据包
  //  if(!modbus->is_hmi)
  //  SEGGER_RTT_printf_hex(array, head_size + g_sonsor_meter.len + 2);
  sendModbusData(array, head_size + g_sonsor_meter.len + 2, modbus->is_hmi);
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

