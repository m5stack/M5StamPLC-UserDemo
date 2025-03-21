/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "button.h"
#include "../../../../../hal/hal.h"

using namespace SYSTEM::INPUTS;

void Button::update()
{
    m5::Button_Class::setRawState(HAL::Millis(), HAL::GetButton(_button));
}

static Button* _button_a = nullptr;
Button* Button::Back()
{
    // Lazy loading
    if (_button_a == nullptr) {
        _button_a = new Button(Gamepad::BTN_A);
    }
    return _button_a;
}

static Button* _button_b = nullptr;
Button* Button::Next()
{
    // Lazy loading
    if (_button_b == nullptr) {
        _button_b = new Button(Gamepad::BTN_B);
    }
    return _button_b;
}

static Button* _button_c = nullptr;
Button* Button::Ok()
{
    // Lazy loading
    if (_button_c == nullptr) {
        _button_c = new Button(Gamepad::BTN_C);
    }
    return _button_c;
}

void Button::Update()
{
    Back()->update();
    Next()->update();
    Ok()->update();
}
