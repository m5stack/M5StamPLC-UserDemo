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

PI4IOE5V6408_Class* __get_io_expander_a();

void HAL_StamPLC::_gamepad_init()
{
    spdlog::info("gamepad init");

    __get_io_expander_a()->setDirection(0, false);
    __get_io_expander_a()->setPullMode(0, true);
    __get_io_expander_a()->setHighImpedance(0, false);

    __get_io_expander_a()->setDirection(1, false);
    __get_io_expander_a()->setPullMode(1, true);
    __get_io_expander_a()->setHighImpedance(1, false);

    __get_io_expander_a()->setDirection(2, false);
    __get_io_expander_a()->setPullMode(2, true);
    __get_io_expander_a()->setHighImpedance(2, false);

/* -------------------------------------------------------------------------- */
/*                                    Test                                    */
/* -------------------------------------------------------------------------- */
#ifdef __ENABLE_TEST
    while (1) {
        spdlog::info("{} {} {}", getButton(Gamepad::BTN_A), getButton(Gamepad::BTN_B), getButton(Gamepad::BTN_C));

        _data.canvas->fillScreen(TFT_BLACK);
        _data.canvas->setCursor(0, 0);
        _data.canvas->printf("%d %d %d\n", getButton(Gamepad::BTN_A), getButton(Gamepad::BTN_B),
                             getButton(Gamepad::BTN_C));
        _data.canvas->pushSprite(0, 0);

        delay(50);
    }
#endif
}

static bool _button_state_a = false;
static bool _button_state_b = false;
static bool _button_state_c = false;

bool HAL_StamPLC::getButton(Gamepad::GamepadButton_t button)
{
    if (button == Gamepad::BTN_A) {
        if (!__get_io_expander_a()->digitalRead(2)) {
            // If just pressed
            if (!_button_state_a) {
                _button_state_a = true;
                beep(600, 20);
            }
            return true;
        }
        // If just released
        if (_button_state_a) {
            _button_state_a = false;
            beep(800, 20);
        }
    }

    if (button == Gamepad::BTN_B) {
        if (!__get_io_expander_a()->digitalRead(1)) {
            // If just pressed
            if (!_button_state_b) {
                _button_state_b = true;
                beep(600, 20);
            }
            return true;
        }
        // If just released
        if (_button_state_b) {
            _button_state_b = false;
            beep(800, 20);
        }
    }

    if (button == Gamepad::BTN_C) {
        if (!__get_io_expander_a()->digitalRead(0)) {
            // If just pressed
            if (!_button_state_c) {
                _button_state_c = true;
                beep(600, 20);
            }
            return true;
        }
        // If just released
        if (_button_state_c) {
            _button_state_c = false;
            beep(800, 20);
        }
    }

    return false;
}

void HAL_StamPLC::_status_light_init()
{
    spdlog::info("status light init");

    __get_io_expander_a()->setDirection(4, true);
    __get_io_expander_a()->setPullMode(4, false);
    __get_io_expander_a()->setHighImpedance(4, true);

    __get_io_expander_a()->setDirection(5, true);
    __get_io_expander_a()->setPullMode(5, false);
    __get_io_expander_a()->setHighImpedance(5, true);

    __get_io_expander_a()->setDirection(6, true);
    __get_io_expander_a()->setPullMode(6, false);
    __get_io_expander_a()->setHighImpedance(6, true);
}

static uint8_t _status_light[3] = {0, 0, 0};
void HAL_StamPLC::setStatusLight(const uint8_t& r, const uint8_t& g, const uint8_t& b)
{
    if (r == 0) {
        __get_io_expander_a()->setHighImpedance(6, true);
        _status_light[0] = 0;
    } else {
        __get_io_expander_a()->setHighImpedance(6, false);
        __get_io_expander_a()->digitalWrite(6, false);
        _status_light[0] = 255;
    }

    if (g == 0) {
        __get_io_expander_a()->setHighImpedance(5, true);
        _status_light[1] = 0;
    } else {
        __get_io_expander_a()->setHighImpedance(5, false);
        __get_io_expander_a()->digitalWrite(5, false);
        _status_light[1] = 255;
    }

    if (b == 0) {
        __get_io_expander_a()->setHighImpedance(4, true);
        _status_light[2] = 0;
    } else {
        __get_io_expander_a()->setHighImpedance(4, false);
        __get_io_expander_a()->digitalWrite(4, false);
        _status_light[2] = 255;
    }
}

void HAL_StamPLC::setStatusLightHex(const std::string& hexColor)
{
    uint8_t r, g, b;
    std::sscanf(hexColor.c_str(), "#%2hhx%2hhx%2hhx", &r, &g, &b);
    setStatusLight(r, g, b);
}

std::string HAL_StamPLC::getStatusLightHex()
{
    return fmt::format("#{:02x}{:02x}{:02x}", _status_light[0], _status_light[1], _status_light[2]);
}
