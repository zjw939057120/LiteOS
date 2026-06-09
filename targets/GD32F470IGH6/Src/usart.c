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
#include "queue.h"

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t USART0_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART1_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART2_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART3_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART4_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART5_RX_BUF[USART_REC_LEN] = {0};
uint8_t USART6_RX_BUF[USART_REC_LEN] = {0};

/* 发送缓冲 */
uint8_t USART0_TX_BUF[] = {0xFF, 0x61, 0x02, 0x01, 0x9C};
uint8_t USART1_TX_BUF[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
uint8_t USART2_TX_BUF[] = {0x11, 0x01, 0x01, 0xED};
uint8_t USART3_TX_BUF[] = {0x11, 0x02, 0x0B, 0x07, 0xDB};
uint8_t USART4_TX_BUF[USART_REC_LEN] = {0};
uint8_t USART5_TX_BUF[USART_REC_LEN] = {0};
uint8_t USART6_TX_BUF[USART_REC_LEN] = {0};

uint8_t USART0_RX_CNT = 0;			//接收的字节数
uint8_t USART1_RX_CNT = 0;			//接收的字节数
uint8_t USART2_RX_CNT = 0;			//接收的字节数
uint8_t USART3_RX_CNT = 0;			//接收的字节数
uint8_t USART4_RX_CNT = 0;			//接收的字节数
uint8_t USART5_RX_CNT = 0;			//接收的字节数
uint8_t USART6_RX_CNT = 0;			//接收的字节数

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t USART0_RX_STA = 0;
uint16_t USART1_RX_STA = 0;
uint16_t USART2_RX_STA = 0;
uint16_t USART3_RX_STA = 0;
uint16_t USART4_RX_STA = 0;
uint16_t USART5_RX_STA = 0;
uint16_t USART6_RX_STA = 0;

/***********************************************************
//Dim:auchCRCHI[]                                           
//CRC计算定值数组                                           
***********************************************************/
const unsigned char auchCRCHI[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};
/***********************************************************
//Dim:auchCRCLO[]                                        
//CRC计算定值数组  
***********************************************************/
const unsigned char auchCRCLO[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};
/***********************************************************
//Function:Crc_Cal()                                       
//CRC计算函数                                              
***********************************************************/
uint16_t Crc_Cal(uint8_t *Data_Buff,uint16_t Data_Length)
{
	unsigned char CRCHI=0xff;
	unsigned char CRCLW=0xff;
	unsigned int CRCINDEX,i;
	i=0;
	while(i<Data_Length)
	{
		CRCINDEX=CRCHI^Data_Buff[i];
		CRCHI=CRCLW^auchCRCHI[CRCINDEX];
		CRCLW=auchCRCLO[CRCINDEX];  
		i++;
	}
	return (CRCHI<<8)+CRCLW;	                               
}
// 交换16位数据的字节序
uint16_t swap_bytes_16(uint16_t value) 
{
    return (value >> 8) | (value << 8);
}

// 交换32位数据的字节序
uint32_t swap_bytes_32(uint32_t value) 
{
    return ((value >> 24) & 0xFF) |
           ((value >> 8) & 0xFF00) |
           ((value << 8) & 0xFF0000) |
           ((value << 24) & 0xFF000000);
}
// 32位数据转换为浮点型数据
float int32_to_float(uint32_t value) {
    union {
        uint32_t i;
        float    f;
    } converter;
    
    converter.i = value;
    return converter.f;
}
/**
 * @brief       串口0中断服务函数
 * @param       无
 * @retval      无
 */
void USART0_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART0_RX_BUF[USART0_RX_CNT] = usart_data_receive(USART0);
        // SEGGER_RTT_printf(0, "USART0_RX_BUF[%d] = 0x%02X\n", USART0_RX_CNT, USART0_RX_BUF[USART0_RX_CNT]);
        USART0_RX_CNT >= USART_REC_LEN ? USART0_RX_CNT = 0 : USART0_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart0,USART0_RX_BUF, USART0_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART0_RX_CNT, ret);
        USART0_RX_CNT = 0;                                                          /* 标记帧接收完成 */
        (void)USART_STAT0(USART0); 
        (void)USART_DATA(USART0);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口1中断服务函数
 * @param       无
 * @retval      无
 */
void USART1_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART1_RX_BUF[USART1_RX_CNT] = usart_data_receive(USART1);
        // SEGGER_RTT_printf(0, "USART1_RX_BUF[%d] = 0x%02X\n", USART1_RX_CNT, USART1_RX_BUF[USART1_RX_CNT]);
        USART1_RX_CNT >= USART_REC_LEN ? USART1_RX_CNT = 0 : USART1_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart1,USART1_RX_BUF, USART1_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART1_RX_CNT, ret);
        USART1_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_STAT0(USART1); 
        (void)USART_DATA(USART1);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口2中断服务函数
 * @param       无
 * @retval      无
 */
void USART2_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART2_RX_BUF[USART2_RX_CNT] = usart_data_receive(USART2);
        // SEGGER_RTT_printf(0, "USART2_RX_BUF[%d] = 0x%02X\n", USART2_RX_CNT, USART2_RX_BUF[USART2_RX_CNT]);
        USART2_RX_CNT >= USART_REC_LEN ? USART2_RX_CNT = 0 : USART2_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart2,USART2_RX_BUF, USART2_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART2_RX_CNT, ret);
        USART2_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_STAT0(USART2); 
        (void)USART_DATA(USART2);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口3中断服务函数
 * @param       无
 * @retval      无
 */
void UART3_IRQHandler(void)
{
    if (usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART3_RX_BUF[USART3_RX_CNT] = usart_data_receive(UART3);
        // SEGGER_RTT_printf(0, "UART3_RX_BUF[%d] = 0x%02X\n", USART3_RX_CNT, USART3_RX_BUF[USART3_RX_CNT]);
        USART3_RX_CNT >= USART_REC_LEN ? USART3_RX_CNT = 0 : USART3_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart3,USART3_RX_BUF, USART3_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART3_RX_CNT, ret);
        USART3_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_DATA(UART3);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口4中断服务函数
 * @param       无
 * @retval      无
 */
void UART4_IRQHandler(void)
{
    if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART4_RX_BUF[USART4_RX_CNT] = usart_data_receive(UART4);
        // SEGGER_RTT_printf(0, "UART4_RX_BUF[%d] = 0x%02X\n", USART4_RX_CNT, USART4_RX_BUF[USART4_RX_CNT]);
        USART4_RX_CNT >= USART_REC_LEN ? USART4_RX_CNT = 0 : USART4_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart4,USART4_RX_BUF, USART4_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART4_RX_CNT, ret);
        USART4_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_DATA(UART4);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口5中断服务函数
 * @param       无
 * @retval      无
 */
void USART5_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART5, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART5_RX_BUF[USART5_RX_CNT] = usart_data_receive(USART5);
        // SEGGER_RTT_printf(0, "USART5_RX_BUF[%d] = 0x%02X\n", USART5_RX_CNT, USART5_RX_BUF[USART5_RX_CNT]);
        USART5_RX_CNT >= USART_REC_LEN ? USART5_RX_CNT = 0 : USART5_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(USART5, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart5,USART5_RX_BUF, USART5_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART5_RX_CNT, ret);
        USART5_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_STAT0(USART5); 
        (void)USART_DATA(USART5);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口6中断服务函数
 * @param       无
 * @retval      无
 */
void UART6_IRQHandler(void)
{
    if (usart_interrupt_flag_get(UART6, USART_INT_FLAG_RBNE) != RESET)         /* UART接收中断 */
    {
        USART6_RX_BUF[USART6_RX_CNT] = usart_data_receive(UART6);
        // SEGGER_RTT_printf(0, "UART6_RX_BUF[%d] = 0x%02X\n", USART6_RX_CNT, USART6_RX_BUF[USART6_RX_CNT]);
        USART6_RX_CNT >= USART_REC_LEN ? USART6_RX_CNT = 0 : USART6_RX_CNT++;
    }
    else if (usart_interrupt_flag_get(UART6, USART_INT_FLAG_IDLE) != RESET)        /* UART总线空闲中断 */
    {
         QueueSend(g_queueId_uart6,USART6_RX_BUF, USART6_RX_CNT);
        // SEGGER_RTT_printf(0, "USART_INT_FLAG_IDLE %d, ret = %d\n",USART6_RX_CNT, ret);
        USART6_RX_CNT = 0;                                                          /* 标记帧接收完成 */    
        (void)USART_STAT0(UART6); 
        (void)USART_DATA(UART6);                                                   /* 先读USART_STAT0，再读USART_DATA清除该标志位 */ 
    } 
}
/**
 * @brief       串口0初始化函数,空气质量传感器MS-VOC-V4
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(USART0, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	// usart_interrupt_enable(USART0, USART_INT_IDLE);       /* 使能空闲线中断 */
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(USART0_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart0Hwi();
#endif
    usart_enable(USART0);	                                /* 使能串口 */
}
/**
 * @brief       串口1初始化函数,甲醛传感器SC11-CH2O
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(USART1, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	// usart_interrupt_enable(USART1, USART_INT_IDLE);       /* 使能空闲线中断 */
    /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(USART1_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart1Hwi();
#endif
    usart_enable(USART1);	                                /* 使能串口 */
}
/**
 * @brief       串口2初始化函数,红外二氧化碳传感器CM1106
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(USART2, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	// usart_interrupt_enable(USART2, USART_INT_IDLE);       /* 使能空闲线中断 */	
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(USART2_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart2Hwi();
#endif
    usart_enable(USART2);	                                /* 使能串口 */
}
/**
 * @brief       串口3初始化函数,激光粉尘传感器PM2012SE
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(UART3, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	// usart_interrupt_enable(UART3, USART_INT_IDLE);       /* 使能空闲线中断 */    
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(UART3_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart3Hwi();
#endif
    usart_enable(UART3);	                                /* 使能串口 */
}
/**
 * @brief       串口4初始化函数,RS485串口通讯
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(UART4, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(UART4, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	// usart_interrupt_enable(UART4, USART_INT_IDLE);       /* 使能空闲线中断 */	
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(UART4_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart4Hwi();
#endif
    usart_enable(UART4);	                                /* 使能串口 */
}
/**
 * @brief       串口5初始化函数,esp32c3串口通讯
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(USART5, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */ 
	// usart_interrupt_enable(USART5, USART_INT_IDLE);       /* 使能空闲线中断 */	
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(USART5_IRQn, 3, 3);                   /* 抢占优先级3，子优先级3 */
    Usart5Hwi();
#endif
    usart_enable(USART5);	                                /* 使能串口 */
}
/**
 * @brief       串口6初始化函数,SWM34SMEU6-QFN80串口通讯
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
#if USART_EN_RX  /* 如果使能了接收 */
    usart_receive_config(UART6, USART_RECEIVE_ENABLE);   /* 使能接收 */
    // usart_interrupt_enable(UART6, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */
	// usart_interrupt_enable(UART6, USART_INT_IDLE);       /* 使能空闲线中断 */    
    // /* 配置NVIC，并设置中断优先级 */
    // nvic_irq_enable(UART6_IRQn, 0, 0);                   /* 抢占优先级0，子优先级0 */
    Usart6Hwi();
#endif
    usart_enable(UART6);	                                /* 使能串口 */
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

void Usart0Hwi(void){
    nvic_irq_enable(USART0_IRQn, 0, 0);
    usart_flag_clear(USART0, USART_INT_RBNE);
    usart_flag_clear(USART0, USART_INT_IDLE);
    LOS_HwiCreate((USART0_IRQn + 16), 0, 0, USART0_IRQHandler, NULL);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_interrupt_enable(USART0, USART_INT_IDLE);
}
void Usart1Hwi(void){
    nvic_irq_enable(USART1_IRQn, 0, 0);
    usart_flag_clear(USART1, USART_INT_RBNE);
    usart_flag_clear(USART1, USART_INT_IDLE);
    LOS_HwiCreate((USART1_IRQn + 16), 0, 0, USART1_IRQHandler, NULL);
    usart_interrupt_enable(USART1, USART_INT_RBNE);
    usart_interrupt_enable(USART1, USART_INT_IDLE);
}
void Usart2Hwi(void){
    nvic_irq_enable(USART2_IRQn, 0, 0);
    usart_flag_clear(USART2, USART_INT_RBNE);
    usart_flag_clear(USART2, USART_INT_IDLE);
    LOS_HwiCreate((USART2_IRQn + 16), 0, 0, USART2_IRQHandler, NULL);
    usart_interrupt_enable(USART2, USART_INT_RBNE);
    usart_interrupt_enable(USART2, USART_INT_IDLE);
}
void Usart3Hwi(void){
    nvic_irq_enable(UART3_IRQn, 0, 0);
    usart_flag_clear(UART3, USART_INT_RBNE);
    usart_flag_clear(UART3, USART_INT_IDLE);
    LOS_HwiCreate((UART3_IRQn + 16), 0, 0, UART3_IRQHandler, NULL);
    usart_interrupt_enable(UART3, USART_INT_RBNE);
    usart_interrupt_enable(UART3, USART_INT_IDLE);
}

void Usart4Hwi(void){
    nvic_irq_enable(UART4_IRQn, 0, 0);
    usart_flag_clear(UART4, USART_INT_RBNE);
    usart_flag_clear(UART4, USART_INT_IDLE);
    LOS_HwiCreate((UART4_IRQn + 16), 0, 0, UART4_IRQHandler, NULL);
    usart_interrupt_enable(UART4, USART_INT_RBNE);
    usart_interrupt_enable(UART4, USART_INT_IDLE);
}
void Usart5Hwi(void){
    nvic_irq_enable(USART5_IRQn, 0, 0);
    usart_flag_clear(USART5, USART_INT_RBNE);
    usart_flag_clear(USART5, USART_INT_IDLE);
    LOS_HwiCreate((USART5_IRQn + 16), 0, 0, USART5_IRQHandler, NULL);
    usart_interrupt_enable(USART5, USART_INT_RBNE);
    usart_interrupt_enable(USART5, USART_INT_IDLE);
}
void Usart6Hwi(void){
    nvic_irq_enable(UART6_IRQn, 0, 0);
    usart_flag_clear(UART6, USART_INT_RBNE);
    usart_flag_clear(UART6, USART_INT_IDLE);
    LOS_HwiCreate((UART6_IRQn + 16), 0, 0, UART6_IRQHandler, NULL);
    usart_interrupt_enable(UART6, USART_INT_RBNE);
    usart_interrupt_enable(UART6, USART_INT_IDLE);
}
void Usart0Req(void)
{
	Seria_SendArray(USART0,USART0_TX_BUF,sizeof(USART0_TX_BUF));
}
void Usart1Req(void)
{
	Seria_SendArray(USART1,USART1_TX_BUF,sizeof(USART1_TX_BUF));
}
void Usart2Req(void)
{
	Seria_SendArray(USART2,USART2_TX_BUF,sizeof(USART2_TX_BUF));
}
void Usart3Req(void)
{
	Seria_SendArray(UART3,USART3_TX_BUF,sizeof(USART3_TX_BUF));
}
void Usart4Req(void)
{}
void Usart5Req(void)
{}
void Usart6Req(void)
{}

void UsartInit(void)
{
    usart0_init(9600U);
    usart1_init(9600U);
    usart2_init(9600U);
    usart3_init(9600U);
    usart4_init(9600U);
    usart5_init(9600U);
    usart6_init(115200U);
}

