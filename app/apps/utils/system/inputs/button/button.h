/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "../../../../../hal/hal.h"
#include "m5unified/Button_Class.hpp"

namespace SYSTEM {
namespace INPUTS {
/**
 * @brief Button class with richer apis
 *
 */
class Button : public m5::Button_Class {
public:
    Button(Gamepad::GamepadButton_t button) : _button(button)
    {
    }
    void update();

    /**
     * @brief Button instances
     *
     * @return Button*
     */
    static Button* Back();
    static Button* Next();
    static Button* Ok();

    /**
     * @brief Helper method to update all button instances
     *
     */
    static void Update();

private:
    Gamepad::GamepadButton_t _button;
};

}  // namespace INPUTS
}  // namespace SYSTEM
