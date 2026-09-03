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

#include "toolkit.h"

void SEGGER_RTT_printf_hex(const uint8_t *array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    SEGGER_RTT_printf(0, "%02X ", array[i]);
    // 每32字节换行
    if ((i + 1) % 32 == 0) {
      SEGGER_RTT_printf(0, "\n");
    }
  }
  SEGGER_RTT_printf(0, "\n");
}

void SEGGER_RTT_printf_dec(const uint8_t *array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    SEGGER_RTT_printf(0, "%d ", array[i]);
    // 每32字节换行
    if ((i + 1) % 32 == 0) {
      SEGGER_RTT_printf(0, "\n");
    }
  }
  SEGGER_RTT_printf(0, "\n");
}

void SEGGER_RTT_printf_string(const uint8_t *array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    SEGGER_RTT_printf(0, "%c", array[i]);
  }
  SEGGER_RTT_printf(0, "\n");
}

uint16_t swap_uint16_array(const uint8_t *array) {
  return (array[0] << 8) | array[1];
}

uint32_t swap_uint32_array(const uint8_t *array) {
  return (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | array[3];
}
uint16_t swap_uint16(uint16_t val){
  return (val << 8) | (val >> 8);
}
uint32_t swap_uint32(uint32_t val){
  return (val << 24) | (val << 8) | (val >> 8) | (val >> 24);
}