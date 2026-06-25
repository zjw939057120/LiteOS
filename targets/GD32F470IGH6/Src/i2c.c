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

#include "i2c.h"
#include "los_task.h"
#include "platform.h"
#include "gd32f4xx.h"
#include "sensor.h"

#define I2C1_TIMEOUT  0xFFFF

static UINT8 I2c1WaitFlag(i2c_flag_enum flag)
{
    UINT32 timeout = I2C1_TIMEOUT;
    while (RESET == i2c_flag_get(I2C1_PERIPH, flag)) {
        if (--timeout == 0) {
            return 1;
        }
    }
    return 0;
}

static UINT8 I2c1WaitFlagClear(i2c_flag_enum flag)
{
    UINT32 timeout = I2C1_TIMEOUT;
    while (SET == i2c_flag_get(I2C1_PERIPH, flag)) {
        if (--timeout == 0) {
            return 1;
        }
    }
    return 0;
}

static void I2c1GpioInit(void)
{
    rcu_periph_clock_enable(I2C1_GPIO_CLK);
    rcu_periph_clock_enable(I2C1_CLK);

    /* PH4 = I2C1_SCL, PH5 = I2C1_SDA */
    gpio_af_set(I2C1_SCL_PORT, I2C1_GPIO_AF, I2C1_SCL_PIN);
    gpio_af_set(I2C1_SDA_PORT, I2C1_GPIO_AF, I2C1_SDA_PIN);

    gpio_mode_set(I2C1_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SCL_PIN);
    gpio_output_options_set(I2C1_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C1_SCL_PIN);

    gpio_mode_set(I2C1_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SDA_PIN);
    gpio_output_options_set(I2C1_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C1_SDA_PIN);
}

static void I2c1PeriphInit(void)
{
    i2c_disable(I2C1_PERIPH);

    i2c_clock_config(I2C1_PERIPH, 100000, I2C_DTCY_2);
    i2c_mode_addr_config(I2C1_PERIPH, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
    i2c_ack_config(I2C1_PERIPH, I2C_ACK_ENABLE);

    i2c_enable(I2C1_PERIPH);
}

void I2cInit(void)
{
    I2c1GpioInit();
    I2c1PeriphInit();
}

void I2c1Write(UINT8 devAddr, UINT8 *data, UINT8 len)
{
    UINT32 i;

    while (i2c_flag_get(I2C1_PERIPH, I2C_FLAG_I2CBSY));

    i2c_start_on_bus(I2C1_PERIPH);
    I2c1WaitFlag(I2C_FLAG_SBSEND);

    i2c_master_addressing(I2C1_PERIPH, devAddr << 1, I2C_TRANSMITTER);
    I2c1WaitFlag(I2C_FLAG_ADDSEND);
    i2c_flag_clear(I2C1_PERIPH, I2C_FLAG_ADDSEND);

    for (i = 0; i < len; i++) {
        I2c1WaitFlagClear(I2C_FLAG_TBE);
        i2c_data_transmit(I2C1_PERIPH, data[i]);
        I2c1WaitFlag(I2C_FLAG_BTC);
    }

    i2c_stop_on_bus(I2C1_PERIPH);
    I2c1WaitFlagClear(I2C_FLAG_STPDET);
}

void I2c1Read(UINT8 devAddr, UINT8 *buf, UINT8 len)
{
    UINT32 i;

    while (i2c_flag_get(I2C1_PERIPH, I2C_FLAG_I2CBSY));

    i2c_start_on_bus(I2C1_PERIPH);
    I2c1WaitFlag(I2C_FLAG_SBSEND);

    i2c_master_addressing(I2C1_PERIPH, devAddr << 1, I2C_RECEIVER);
    I2c1WaitFlag(I2C_FLAG_ADDSEND);

    if (len == 1) {
        i2c_ack_config(I2C1_PERIPH, I2C_ACK_DISABLE);
        i2c_flag_clear(I2C1_PERIPH, I2C_FLAG_ADDSEND);
        i2c_stop_on_bus(I2C1_PERIPH);
        I2c1WaitFlag(I2C_FLAG_RBNE);
        buf[0] = i2c_data_receive(I2C1_PERIPH);
        I2c1WaitFlagClear(I2C_FLAG_STPDET);
        i2c_ack_config(I2C1_PERIPH, I2C_ACK_ENABLE);
    } else {
        i2c_flag_clear(I2C1_PERIPH, I2C_FLAG_ADDSEND);

        for (i = 0; i < len; i++) {
            if (i == len - 2) {
                i2c_ack_config(I2C1_PERIPH, I2C_ACK_DISABLE);
            }
            if (i == len - 1) {
                i2c_stop_on_bus(I2C1_PERIPH);
            }
            I2c1WaitFlag(I2C_FLAG_RBNE);
            buf[i] = i2c_data_receive(I2C1_PERIPH);
        }
        I2c1WaitFlagClear(I2C_FLAG_STPDET);
        i2c_ack_config(I2C1_PERIPH, I2C_ACK_ENABLE);
    }
}

void Aht30Init(void)
{
    UINT8 cmd[3] = {AHT30_CMD_INIT, 0x08, 0x00};
    I2c1Write(AHT30_I2C_ADDR, cmd, 3);
    LOS_TaskDelay(10);
}

UINT8 Aht30Read(float *temperature, float *humidity)
{
    UINT8 cmd[4] = {AHT30_CMD_TRIGGER, 0x33, 0x00};
    UINT8 buf[6];
    UINT32 rawHum = 0, rawTemp = 0;
    // UINT8 i;

    I2c1Write(AHT30_I2C_ADDR, cmd, 3);
    LOS_TaskDelay(20);

    I2c1Read(AHT30_I2C_ADDR, buf, 6);

    if ((buf[0] & 0x68) != 0x08) {
        return 1;
    }

    rawHum = ((UINT32)buf[1] << 12) | ((UINT32)buf[2] << 4) | ((UINT32)buf[3] >> 4);
    rawTemp = (((UINT32)buf[3] & 0x0F) << 16) | ((UINT32)buf[4] << 8) | (UINT32)buf[5];

    *humidity = (float)rawHum / 1048576.0f * 100.0f;
    *temperature = (float)rawTemp / 1048576.0f * 200.0f - 50.0f;
    
    if (*temperature >= 20.0f) {
      *temperature = *temperature - 10.0f;
    }
    //放大10倍，方便后续计算
    *humidity = *humidity * 10;
    *temperature = *temperature * 10;
    g_sensor.RH = *humidity;
    g_sensor.TEMP = *temperature;
    //传感器类型
    g_sensor.TYPE |= SHT_Sensor;
    g_sensor.TYPE |= HMT_Sensor;

    return 0;
}
