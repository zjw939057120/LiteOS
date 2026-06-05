/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usart Init Implementation
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

#include "usart.h"
#include "los_hwi.h"
#include "platform.h"
#include "gd32f4xx.h"

#if LOSCFG_DEBUG_VERSION 
VOID Usart0Init(UINT32 bound)
{    
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, bound);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

VOID UsartInit(VOID) {
    Usart0Init(USART_DEFAULT_BOUND);
}

VOID UsartWrite(const CHAR c)
{
    while (usart_flag_get(USART0, USART_FLAG_TBE) == 0) {}
    usart_data_transmit(USART0, c);
}

UINT8 UsartRead(VOID)
{
    UINT8 ch = 0;
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) == RESET) {
        return ch;
    }
    ch = (UINT8)(usart_data_receive(USART0) & 0xFF);
    return ch;
}

STATIC VOID UartHandler(VOID)
{
    (VOID)uart_getc();
}

INT32 UsartHwi(VOID)
{
    nvic_irq_enable(USART0_IRQn, 0, 0);
    usart_flag_clear(USART0, USART_INT_RBNE);
    LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 0, 0, UartHandler, NULL);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    return LOS_OK;
}

UartControllerOps g_genericUart = {
    .uartInit = UsartInit,
    .uartWriteChar = UsartWrite,
    .uartReadChar = UsartRead,
    .uartHwiCreate = UsartHwi
};
#endif

/**
 * @brief       串口0初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart0_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOA);     /* 使能GPIOA时钟 */
    rcu_periph_clock_enable(RCU_USART0);    /* 使能串口时钟 */

    /* 设置USART0_Tx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);

    /* 设置USART0_Rx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

    /* USART0_Tx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* USART0_Rx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* 配置USART的参数 */
    usart_deinit(USART0);                                 /* 复位USART0 */
    usart_baudrate_set(USART0, bound);                    /* 设置波特率 */
    usart_stop_bit_set(USART0, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(USART0, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(USART0, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(USART0, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(USART0, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	usart_interrupt_enable(USART0, USART_INT_IDLE);       /* 使能空闲线中断 */
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(USART0_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(USART0);	                                /* 使能串口 */
}
/**
 * @brief       串口1初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart1_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOA);     /* 使能GPIOA时钟 */
    rcu_periph_clock_enable(RCU_USART1);    /* 使能串口时钟 */

    /* 设置USART1_Tx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);

    /* 设置USART1_Rx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);

    /* USART1_Tx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* USART1_Rx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* 配置USART的参数 */
    usart_deinit(USART1);                                 /* 复位USART1 */
    usart_baudrate_set(USART1, bound);                    /* 设置波特率 */
    usart_stop_bit_set(USART1, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(USART1, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(USART1, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(USART1, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(USART1, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	usart_interrupt_enable(USART1, USART_INT_IDLE);       /* 使能空闲线中断 */
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(USART1_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(USART1);	                                /* 使能串口 */
}
/**
 * @brief       串口2初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart2_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOC);     /* 使能GPIOC时钟 */
    rcu_periph_clock_enable(RCU_USART2);    /* 使能串口时钟 */

    /* 设置USART2_Tx的复用功能选择 */
    gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_10);

    /* 设置USART2_Rx的复用功能选择 */
    gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_5);

    /* USART2_Tx的模式设置 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* USART2_Rx的模式设置 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    /* 配置USART的参数 */
    usart_deinit(USART2);                                 /* 复位USART2 */
    usart_baudrate_set(USART2, bound);                    /* 设置波特率 */
    usart_stop_bit_set(USART2, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(USART2, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(USART2, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(USART2, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(USART2, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	usart_interrupt_enable(USART2, USART_INT_IDLE);       /* 使能空闲线中断 */	
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(USART2_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(USART2);	                                /* 使能串口 */
}
/**
 * @brief       串口3初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart3_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOA);     /* 使能GPIOA时钟 */
    rcu_periph_clock_enable(RCU_UART3);    /* 使能串口时钟 */

    /* 设置UART3_Tx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_0);

    /* 设置UART3_Rx的复用功能选择 */
    gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_1);

    /* UART3_Tx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    /* UART3_Rx的模式设置 */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

    /* 配置UART的参数 */
    usart_deinit(UART3);                                 /* 复位UART3 */
    usart_baudrate_set(UART3, bound);                    /* 设置波特率 */
    usart_stop_bit_set(UART3, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(UART3, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(UART3, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(UART3, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(UART3, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	usart_interrupt_enable(UART3, USART_INT_IDLE);       /* 使能空闲线中断 */    
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(UART3_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(UART3);	                                /* 使能串口 */
}
/**
 * @brief       串口4初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart4_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOD);     /* 使能GPIOD时钟 */
	rcu_periph_clock_enable(RCU_GPIOC);     /* 使能GPIOC时钟 */
    rcu_periph_clock_enable(RCU_UART4);    /* 使能串口时钟 */

    /* 设置UART4_Tx的复用功能选择 */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_12);

    /* 设置UART4_Rx的复用功能选择 */
    gpio_af_set(GPIOD, GPIO_AF_8, GPIO_PIN_2);

    /* UART4_Tx的模式设置 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

    /* UART4_Rx的模式设置 */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* 配置UART的参数 */
    usart_deinit(UART4);                                 /* 复位UART4 */
    usart_baudrate_set(UART4, bound);                    /* 设置波特率 */
    usart_stop_bit_set(UART4, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(UART4, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(UART4, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(UART4, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(UART4, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(UART4, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	usart_interrupt_enable(UART4, USART_INT_IDLE);       /* 使能空闲线中断 */	
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(UART4_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(UART4);	                                /* 使能串口 */
}
/**
 * @brief       串口5初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart5_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
	rcu_periph_clock_enable(RCU_GPIOC);     /* 使能GPIOC时钟 */
    rcu_periph_clock_enable(RCU_USART5);    /* 使能串口时钟 */

    /* 设置USART5_Tx的复用功能选择 */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_7);

    /* 设置USART5_Rx的复用功能选择 */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_6);

    /* USART5_Tx的模式设置 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* USART5_Rx的模式设置 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* 配置USART的参数 */
    usart_deinit(USART5);                                 /* 复位USART0 */
    usart_baudrate_set(USART5, bound);                    /* 设置波特率 */
    usart_stop_bit_set(USART5, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(USART5, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(USART5, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(USART5, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(USART5, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	usart_interrupt_enable(USART5, USART_INT_IDLE);       /* 使能空闲线中断 */	
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(USART5_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */

    usart_enable(USART5);	                                /* 使能串口 */
}
/**
 * @brief       串口6初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart6_init(uint32_t bound)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(RCU_GPIOF);     /* 使能GPIOF时钟 */
    rcu_periph_clock_enable(RCU_UART6);    /* 使能串口时钟 */

    /* 设置UART6_Tx的复用功能选择 */
    gpio_af_set(GPIOF, GPIO_AF_8, GPIO_PIN_7);

    /* 设置UART6_Rx的复用功能选择 */
    gpio_af_set(GPIOF, GPIO_AF_8, GPIO_PIN_6);

    /* UART6_Tx的模式设置 */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* UART6_Rx的模式设置 */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* 配置UART的参数 */
    usart_deinit(UART6);                                 /* 复位UART6 */
    usart_baudrate_set(UART6, bound);                    /* 设置波特率 */
    usart_stop_bit_set(UART6, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(UART6, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(UART6, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(UART6, USART_TRANSMIT_ENABLE); /* 使能发送 */

    usart_receive_config(UART6, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(UART6, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	usart_interrupt_enable(UART6, USART_INT_IDLE);       /* 使能空闲线中断 */    
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(UART6_IRQn, 0, 0);                   /* 抢占优先级0，子优先级0 */

    usart_enable(UART6);	                                /* 使能串口 */
}

void USART0_IRQHandler(void){
    if((RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(USART0);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART1_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(USART1);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART2_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(USART2);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART3_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(UART3);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART4_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(UART4);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART5_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(USART5);
        printf("Received byte: 0x%02X\n", byte);
    }
}
void USART6_IRQHandler(void)
{
    if((RESET != usart_interrupt_flag_get(UART6, USART_INT_FLAG_RBNE)) &&
       (RESET != usart_interrupt_flag_get(UART6, USART_INT_FLAG_IDLE)))
    {
        uint8_t byte = usart_data_receive(UART6);
        printf("Received byte: 0x%02X\n", byte);
    }
}

//串口发送单字节函数
void Seria_SendByte(uint32_t USARTx,uint8_t Byte)
{
	usart_data_transmit(USARTx, Byte); 	
	while (usart_flag_get(USARTx, USART_FLAG_TC) == RESET); 
}
//串口发送一组数据函数
void Seria_SendArray(uint32_t USARTx,uint8_t *Array, uint16_t Length) 
{ 	
	uint16_t i; 	
	for (i = 0; i < Length; i ++) 	
	{ 		
		Seria_SendByte(USARTx,Array[i]); 	
	} 
}
