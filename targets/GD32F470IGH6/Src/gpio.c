/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Gpio Init Implementation
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

#include "gpio.h"
#include "gd32f4xx.h"
#include "los_task_pri.h"

/* GPIO引脚宏定义 */
#define TXC_OE_PORT         GPIOE
#define TXC_OE_PIN          GPIO_PIN_2

#define ESP_EN_PORT         GPIOA
#define ESP_EN_PIN          GPIO_PIN_8

#define RS485_EN_PORT       GPIOC
#define RS485_EN_PIN        GPIO_PIN_13

void gpio_en(void)
{
  /* 使能GPIO时钟 */
  rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_GPIOE);

  /* 配置TXC_OE为输出模式 */
  gpio_mode_set(TXC_OE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TXC_OE_PIN);
  gpio_output_options_set(TXC_OE_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TXC_OE_PIN);

  /* 配置ESP_EN为输出模式 */
  gpio_mode_set(ESP_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ESP_EN_PIN);
  gpio_output_options_set(ESP_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, ESP_EN_PIN);

  /* 配置RS485_EN为输出模式 */
  gpio_mode_set(RS485_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RS485_EN_PIN);
  gpio_output_options_set(RS485_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_EN_PIN);

  /* 拉高TXC_OE和ESP_EN，拉低RS485_EN */
  gpio_bit_set(TXC_OE_PORT, TXC_OE_PIN);
  gpio_bit_set(ESP_EN_PORT, ESP_EN_PIN);
  gpio_bit_reset(RS485_EN_PORT, RS485_EN_PIN);
}

VOID GpioInit(VOID)
{
    gpio_en();
}