/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include "../utils/lm75b/lm75b.h"

static LM75B_Class* _thermal_sense = nullptr;

void HAL_StamPLC::_thermal_sense_init()
{
    spdlog::info("thermal sense init");

    _thermal_sense = new LM75B_Class;
    if (!_thermal_sense->begin()) {
        delete _thermal_sense;
        _thermal_sense = nullptr;
        spdlog::warn("init failed");
    } else {
        spdlog::info("ok");
    }
}

void HAL_StamPLC::_thermal_sense_intr_test()
{
    spdlog::info("thermal intr test");

    gpio_reset_pin((gpio_num_t)HAL_PIN_I2C_INTER_IRQ);
    gpio_set_direction((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_PULLUP_ONLY);

    _thermal_sense->alertTemp(30);

    while (1) {
        feedTheDog();
        delay(100);

        spdlog::info("iqr {}, t {:.3f}", gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ), _thermal_sense->temp());

        if (gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ) == 0) {
            spdlog::info("hit");
            delay(100);
        }
    }
}

void HAL_StamPLC::updateTemp()
{
    if (_thermal_sense == nullptr) {
        return;
    }
    _data.temp = _thermal_sense->temp();
}
