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

/**
 * @brief 初始化LCD与触摸屏控制引脚
 */
void gpio_init_touch_lcd(void) 
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_GPIOH);

    // LCD控制引脚：推挽输出
	gpio_mode_set(LCD_PWR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_PWR_PIN);
    gpio_output_options_set(LCD_PWR_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_PWR_PIN);
	
	gpio_mode_set(LCD_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_EN_PIN);
    gpio_output_options_set(LCD_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_EN_PIN);
	
    gpio_mode_set(LCD_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_CS_PIN);
    gpio_output_options_set(LCD_CS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_CS_PIN);
    
    gpio_mode_set(LCD_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_RST_PIN);
    gpio_output_options_set(LCD_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_RST_PIN);
    
    gpio_mode_set(LCD_BL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_BL_PIN);
    gpio_output_options_set(LCD_BL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_BL_PIN);
	//I2C接口设置为开漏
	gpio_mode_set(LCD_SCK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LCD_SCK_PIN);
    gpio_output_options_set(LCD_SCK_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, LCD_SCK_PIN);
	
	gpio_mode_set(LCD_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LCD_SDA_PIN);
    gpio_output_options_set(LCD_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, LCD_SDA_PIN);
	
	gpio_mode_set(LCD_B2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_B2_PIN);
    gpio_output_options_set(LCD_B2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_B2_PIN);
	
	gpio_mode_set(LCD_R2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_R2_PIN);
    gpio_output_options_set(LCD_R2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_R2_PIN);

    // 触摸屏控制引脚：INT（下拉输入）、RST（推挽输出）
    gpio_mode_set(TOUCH_INT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, TOUCH_INT_PIN);
    
    gpio_mode_set(TOUCH_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TOUCH_RST_PIN);
    gpio_output_options_set(TOUCH_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, TOUCH_RST_PIN);
	
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    // 初始化电平
	gpio_bit_set(LCD_PWR_PORT, LCD_PWR_PIN);  // 打开LCD背光电源
	gpio_bit_set(LCD_EN_PORT, LCD_EN_PIN);  // 打开LCD电源
    gpio_bit_set(LCD_CS_PORT, LCD_CS_PIN);    // LCD片选默认高（未选中）
    gpio_bit_set(LCD_BL_PORT, LCD_BL_PIN);    // 开启背光
    gpio_bit_reset(LCD_RST_PORT, LCD_RST_PIN); // 拉低复位LCD
    gpio_bit_reset(TOUCH_RST_PORT, TOUCH_RST_PIN); // 拉低复位触摸屏
	gpio_bit_reset(LCD_B2_PORT, LCD_B2_PIN); // 拉低B2
	gpio_bit_reset(LCD_R2_PORT, LCD_R2_PIN); // 拉低R2
    LOS_TaskDelay(20);
    gpio_bit_set(LCD_RST_PORT, LCD_RST_PIN);  // 释放LCD复位
    gpio_bit_set(TOUCH_RST_PORT, TOUCH_RST_PIN); // 释放触摸屏复位
	gpio_bit_reset(TOUCH_INT_PORT, TOUCH_INT_PIN); // 拉低触摸屏INT
    LOS_TaskDelay(50);
	TXD_EN(1);
}

VOID GpioInit(VOID)
{
    gpio_init_touch_lcd();
}