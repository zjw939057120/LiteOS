/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usart Init HeadFile
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

#ifndef _USART_H
#define _USART_H

#include "los_typedef.h"
#include "uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if LOSCFG_DEBUG_VERSION
#define USART_DEFAULT_BOUND 115200U

VOID Usart0Init(UINT32 bound);

extern UartControllerOps g_genericUart;
#endif

#define USART_REC_LEN               200         /* 定义最大接收字节数 200 */
#define USART_EN_RX                 0           /* 使能（1）/禁止（0）串口0接收 */

extern uint8_t  USART0_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART1_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART2_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART3_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART4_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART5_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART6_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 

extern uint8_t  USART0_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART1_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART2_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART3_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART4_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART5_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART6_TX_BUF[USART_REC_LEN];    /* 发送缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint16_t USART0_RX_STA;         		      /* 接收状态标记 */	
extern uint16_t USART1_RX_STA;         		      /* 接收状态标记 */
extern uint16_t USART2_RX_STA;         		      /* 接收状态标记 */
extern uint16_t USART3_RX_STA;         		      /* 接收状态标记 */
extern uint16_t USART4_RX_STA;         		      /* 接收状态标记 */
extern uint16_t USART5_RX_STA;         		      /* 接收状态标记 */
extern uint16_t USART6_RX_STA;         		      /* 接收状态标记 */

extern uint8_t USART0_RX_CNT;				      /*串口0接收的字节数*/	
extern uint8_t USART1_RX_CNT;				      /*串口1接收的字节数*/	
extern uint8_t USART2_RX_CNT;				      /*串口2接收的字节数*/
extern uint8_t USART3_RX_CNT;				      /*串口3接收的字节数*/	
extern uint8_t USART4_RX_CNT;				      /*串口4接收的字节数*/
extern uint8_t USART5_RX_CNT;				      /*串口5接收的字节数*/	
extern uint8_t USART6_RX_CNT;				      /*串口6接收的字节数*/

void usart0_init(uint32_t bound);                /* 串口初始化函数 */
void usart1_init(uint32_t bound);                /* 串口初始化函数 */
void usart2_init(uint32_t bound);                /* 串口初始化函数 */
void usart3_init(uint32_t bound);                /* 串口初始化函数 */
void usart4_init(uint32_t bound);                /* 串口初始化函数 */
void usart5_init(uint32_t bound);                /* 串口初始化函数 */
void usart6_init(uint32_t bound);                /* 串口初始化函数 */

void USART0_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void USART4_IRQHandler(void);
void USART5_IRQHandler(void);
void USART6_IRQHandler(void);

void Seria_SendByte(uint32_t USARTx,uint8_t Byte);
void Seria_SendArray(uint32_t USARTx,uint8_t *Array, uint16_t Length);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _USART_H */
