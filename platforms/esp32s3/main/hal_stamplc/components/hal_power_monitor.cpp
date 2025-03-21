/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <cstdint>
#include <cstdio>
#include "../utils/INA226/src/INA226.h"

// Refs:
// https://github.com/jarzebski/Arduino-INA226/blob/dev/examples/INA226_simple/INA226_simple.ino

static INA226* _ina226 = nullptr;

void HAL_StamPLC::_power_monitor_init()
{
    spdlog::info("power monitor init");

    _ina226 = new INA226;
    if (!_ina226->begin()) {
        delete _ina226;
        _ina226 = nullptr;
        spdlog::warn("init failed");
    } else {
        spdlog::info("ok");

        // 28.4 Hz
        _ina226->configure(INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US,
                           INA226_MODE_SHUNT_BUS_CONT);
        _ina226->calibrate(0.01, 8.192);
    }
}

void HAL_StamPLC::_power_monitor_intr_test()
{
    spdlog::info("pm intr test");

    gpio_reset_pin((gpio_num_t)HAL_PIN_I2C_INTER_IRQ);
    gpio_set_direction((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_PULLUP_ONLY);

    _ina226->enableBusOvertLimitAlert();
    _ina226->setBusVoltageLimit(9);

    while (1) {
        feedTheDog();
        delay(50);

        spdlog::info("iqr {}", gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ));

        if (gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ) == 0) {
            spdlog::info("hit");
            delay(100);
        }
    }
}

void HAL_StamPLC::updatePowerMonitor()
{
    if (_ina226 == nullptr) {
        return;
    }
    _data.power_voltage           = _ina226->readBusVoltage();
    _data.expander_output_current = _ina226->readShuntCurrent();
}
