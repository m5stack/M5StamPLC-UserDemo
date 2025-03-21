/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include "../utils/pi4ioe5v6408/pi4ioe5v6408.h"

// #define __ENABLE_TEST

static PI4IOE5V6408_Class* _io_expander_a = nullptr;
PI4IOE5V6408_Class* __get_io_expander_a()
{
    return _io_expander_a;
}

void HAL_StamPLC::_io_expander_a_init()
{
    spdlog::info("io expander a init");

    // Init
    assert(_io_expander_a == nullptr);
    _io_expander_a = new PI4IOE5V6408_Class;
    if (!_io_expander_a->begin()) {
        delete _io_expander_a;
        _io_expander_a = nullptr;
        popFatalError("io expander init failed");
    } else {
        spdlog::info("ok");
        _io_expander_a->resetIrq();
    }

/* -------------------------------------------------------------------------- */
/*                                    Test                                    */
/* -------------------------------------------------------------------------- */
#ifdef __ENABLE_TEST
    _io_expander_a->setDirection(7, true);
    _io_expander_a->setPullMode(7, false);
    _io_expander_a->setHighImpedance(7, false);

    _io_expander_a->setDirection(4, true);
    _io_expander_a->setPullMode(4, false);
    _io_expander_a->setHighImpedance(4, false);
    _io_expander_a->setDirection(5, true);
    _io_expander_a->setPullMode(5, false);
    _io_expander_a->setHighImpedance(5, false);
    _io_expander_a->setDirection(6, true);
    _io_expander_a->setPullMode(6, false);
    _io_expander_a->setHighImpedance(6, false);

    while (1) {
        spdlog::info("111");
        _io_expander_a->digitalWrite(7, true);
        _io_expander_a->digitalWrite(4, true);
        _io_expander_a->digitalWrite(5, true);
        _io_expander_a->digitalWrite(6, true);
        delay(500);

        spdlog::info("000");
        _io_expander_a->digitalWrite(7, false);
        _io_expander_a->digitalWrite(4, false);
        _io_expander_a->digitalWrite(5, false);
        _io_expander_a->digitalWrite(6, false);
        delay(500);
    }
#endif
}

void HAL_StamPLC::_io_expander_a_disable_irq()
{
    if (_io_expander_a == nullptr) {
        spdlog::error("ioex a is null");
        return;
    }
    _io_expander_a->disableIrq();
}

// Active-low interrupt output. Connect to VDD(I2C_bus) through a pull-up resistor.
// 0,1,2 input
void HAL_StamPLC::_io_expander_a_intr_test()
{
    spdlog::info("ioex a intr test");

    gpio_reset_pin((gpio_num_t)HAL_PIN_I2C_INTER_IRQ);
    gpio_set_direction((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_PULLUP_ONLY);

    _io_expander_a->resetIrq();

    while (1) {
        feedTheDog();
        delay(50);

        spdlog::info("iqr {}", gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ));

        if (gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ) == 0) {
            spdlog::info("hit");
            _io_expander_a->resetIrq();
            delay(100);
        }
    }
}
