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
#include "debug.h" 
#include "queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define USART_REC_LEN DEFAULT_QUEUE_BUF_MAX_LEN /* 定义最大接收字节数 200 */
#define USART_EN_RX 1 /* 使能（1）/禁止（0）串口0接收 */

extern uint8_t  USART0_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART1_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART2_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART3_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART4_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 
extern uint8_t  USART5_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint8_t  USART6_RX_BUF[USART_REC_LEN];    /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */ 

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
extern uint16_t swap_bytes_16(uint16_t value);
extern uint32_t swap_bytes_32(uint32_t value);
extern float int32_to_float(uint32_t value);
uint16_t Crc_Cal(uint8_t * Data_Buff,uint16_t Data_Length);

void usart0_init(uint32_t bound);                /* 串口初始化函数,空气质量传感器MS-VOC-V4 */
void usart1_init(uint32_t bound);                /* 串口初始化函数,甲醛传感器SC11-CH2O */
void usart2_init(uint32_t bound);                /* 串口初始化函数,红外二氧化碳传感器CM1106S */
void usart3_init(uint32_t bound);                /* 串口初始化函数,激光粉尘传感器PM2012 */
void usart4_init(uint32_t bound);                /* 串口初始化函数,RS485串口通讯 */
void usart5_init(uint32_t bound);                /* 串口初始化函数,esp32c3串口通讯 */
void usart6_init(uint32_t bound);                /* 串口初始化函数,SWM34SMEU6-QFN80串口通讯 */
void Seria_SendByte(uint32_t USARTx,uint8_t Byte);
void Seria_SendArray(uint32_t USARTx,uint8_t *Array, uint16_t Length);

void Usart0Hwi(void);
void Usart1Hwi(void);
void Usart2Hwi(void);
void Usart3Hwi(void);
void Usart4Hwi(void);
void Usart5Hwi(void);
void Usart6Hwi(void);

void Usart0Req(void);
void Usart1Req(void);
void Usart2Req(void);
void Usart3Req(void);
void Usart4Req(void);
void Usart5Req(void);
void Usart6Req(void);

void UsartInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _USART_H */
