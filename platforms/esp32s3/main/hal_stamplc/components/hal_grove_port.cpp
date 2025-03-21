/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <M5Unified.h>

uint8_t _scan_device(m5::I2C_Class& i2cClass)
{
    uint8_t ret = 0;

    spdlog::info("start scan with scl: {} sda: {}", i2cClass.getSCL(), i2cClass.getSDA());
    bool scan_list[120];
    i2cClass.scanID(scan_list);
    uint8_t device_num = 0;
    for (int i = 8; i < 0x78; i++) {
        if (scan_list[i]) {
            device_num++;
            spdlog::info("get 0x{0:x}", i);
            ret = i;
        }
    }
    spdlog::info("device num: {}", device_num);

    return ret;
}

void HAL_StamPLC::_grove_port_test()
{
    spdlog::info("grove port test");

    while (1) {
        spdlog::info("-----------------");
        m5::Ex_I2C.release();
        m5::Ex_I2C.begin(I2C_NUM_1, HAL_PIN_GROVE_RED_SDA, HAL_PIN_GROVE_RED_SCL);
        auto d111 = _scan_device(m5::Ex_I2C);
        delay(200);

        spdlog::info("------");

        m5::Ex_I2C.release();
        m5::Ex_I2C.begin(I2C_NUM_1, HAL_PIN_GROVE_BLUE_SDA, HAL_PIN_GROVE_BLUE_SCL);
        auto d222 = _scan_device(m5::Ex_I2C);
        delay(200);

        feedTheDog();

        spdlog::info("get 0x{:02x}, 0x{:02x}", d111, d222);
        delay(800);
    }
}
