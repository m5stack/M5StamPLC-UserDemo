/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include "../utils/aw9523/aw9523.h"

static AW9523_Class* _io_expander_b = nullptr;

static std::vector<int> _in_pin_list  = {4, 5, 6, 7, 12, 13, 14, 15};
static std::vector<int> _out_pin_list = {0, 1, 2, 3};

void HAL_StamPLC::_io_expander_b_init()
{
    spdlog::info("io expander b init");

    _io_expander_b = new AW9523_Class;
    if (!_io_expander_b->begin()) {
        delete _io_expander_b;
        _io_expander_b = nullptr;
        spdlog::warn("init failed");
    } else {
        _io_expander_b->configureDirection(0x0);  // all inputs!
        _io_expander_b->openDrainPort0(false);    // push pull default
        _io_expander_b->interruptEnableGPIO(0);   // no interrupt

        for (const auto& i : _out_pin_list) {
            _io_expander_b->pinMode(i, AW9523_Class::AW_OUTPUT);
            _io_expander_b->digitalWrite(i, false);
        }

        for (const auto& i : _in_pin_list) {
            _io_expander_b->pinMode(i, AW9523_Class::AW_INPUT);
        }
    }

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // _io_expander_b_test();
}

void HAL_StamPLC::_io_expander_b_test()
{
    while (1) {
        feedTheDog();

        for (const auto& i : _out_pin_list) {
            spdlog::info("get {} {}", i, _io_expander_b->digitalRead(i));
            _io_expander_b->digitalWrite(i, !_io_expander_b->digitalRead(i));
            delay(100);
        }

        delay(1000);
    }
}

void HAL_StamPLC::_io_expander_b_disable_irq()
{
    if (_io_expander_b == nullptr) {
        spdlog::error("ioex b is null");
        return;
    }
    _io_expander_b->disableIrq();
}

void HAL_StamPLC::_io_expander_b_intr_test()
{
    spdlog::info("ioex b intr test");

    gpio_reset_pin((gpio_num_t)HAL_PIN_I2C_INTER_IRQ);
    gpio_set_direction((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_I2C_INTER_IRQ, GPIO_PULLUP_ONLY);

    for (const auto& i : _in_pin_list) {
        _io_expander_b->enableInterrupt(i, true);
    }

    while (1) {
        feedTheDog();
        delay(50);

        spdlog::info("iqr {}", gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ));

        if (gpio_get_level((gpio_num_t)HAL_PIN_I2C_INTER_IRQ) == 0) {
            spdlog::info("hit");
            _io_expander_b->resetIrq();
            delay(100);
        }
    }
}

bool HAL_StamPLC::getPlcInput(const uint8_t& channel)
{
    if (_io_expander_b == nullptr) {
        return false;
    }
    return _io_expander_b->digitalRead(_in_pin_list[channel]);
}

bool HAL_StamPLC::getPlcRelayState(const uint8_t& channel)
{
    if (_io_expander_b == nullptr) {
        return false;
    }
    return _io_expander_b->digitalRead(_out_pin_list[channel]);
}

void HAL_StamPLC::setPlcRelayState(const uint8_t& channel, const bool& state)
{
    if (_io_expander_b == nullptr) {
        return;
    }
    _io_expander_b->digitalWrite(_out_pin_list[channel], state);
}

void HAL_StamPLC::setPlcAllRelayState(const uint8_t& state)
{
    if (_io_expander_b == nullptr) {
        return;
    }
    for (int i = 0; i < _out_pin_list.size(); i++) {
        _io_expander_b->digitalWrite(_out_pin_list[i], (state & (1 << i)));
    }
}
