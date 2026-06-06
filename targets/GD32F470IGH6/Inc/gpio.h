/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Gpio Init HeadFile
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

#ifndef _GPIO_H
#define _GPIO_H

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// -------------------------- LCD(ST7701S) 引脚定义 --------------------------
// 控制引脚（GPIO配置为推挽输出）
#define LCD_EN_PORT    GPIOH
#define LCD_EN_PIN     GPIO_PIN_3
#define LCD_PWR_PORT   GPIOA
#define LCD_PWR_PIN    GPIO_PIN_7
#define LCD_CS_PORT    GPIOF
#define LCD_CS_PIN     GPIO_PIN_11
#define LCD_RST_PORT   GPIOC
#define LCD_RST_PIN    GPIO_PIN_0
#define LCD_BL_PORT    GPIOA
#define LCD_BL_PIN     GPIO_PIN_4
#define LCD_SCK_PORT   GPIOH
#define LCD_SCK_PIN    GPIO_PIN_7
#define LCD_SDA_PORT   GPIOC
#define LCD_SDA_PIN    GPIO_PIN_9
#define LCD_B2_PORT    GPIOD
#define LCD_B2_PIN     GPIO_PIN_6
#define LCD_R2_PORT    GPIOH
#define LCD_R2_PIN     GPIO_PIN_8

#define TXD_EN(x)   do{ x ? \
                      gpio_bit_write(GPIOE, GPIO_PIN_2, SET) : \
                      gpio_bit_write(GPIOE, GPIO_PIN_2, RESET); \
                  }while(0)

// TLI接口引脚（硬件自动映射，无需手动配置GPIO）
#define LCD_TLI_PERIPH TLI
#define LCD_TLI_CLK    RCU_TLI

// -------------------------- 触摸屏(FT6336U) 引脚定义 --------------------------
// I2C接口（原理图TP_SDA/TP_SCL对应I2C0）
#define TOUCH_I2C_PERIPH I2C0
#define TOUCH_I2C_CLK    RCU_I2C0
#define TOUCH_SDA_PORT   GPIOB
#define TOUCH_SDA_PIN    GPIO_PIN_7
#define TOUCH_SCL_PORT   GPIOB
#define TOUCH_SCL_PIN    GPIO_PIN_6
#define TOUCH_I2C_ADDR   0x38 << 1  // FT6336U默认I2C地址

// 中断与复位引脚
#define TOUCH_INT_PORT   GPIOB
#define TOUCH_INT_PIN    GPIO_PIN_5
#define TOUCH_RST_PORT   GPIOB
#define TOUCH_RST_PIN    GPIO_PIN_10

#define TOUCH_INT    gpio_input_bit_get(GPIOB,GPIO_PIN_5)

void gpio_init_touch_lcd(void);

VOID GpioInit(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GPIO_H */
