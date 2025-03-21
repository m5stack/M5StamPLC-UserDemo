/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "pi4ioe5v6408.h"

bool PI4IOE5V6408_Class::begin()
{
    auto id = readRegister8(0x01);
    spdlog::info("device id {}", id);
    if (id == 0) {
        return false;
    }
    return true;
}

// false input, true output
void PI4IOE5V6408_Class::setDirection(uint8_t pin, bool direction)
{
    // Input, set 0
    if (!direction) {
        auto data = readRegister8(0x03);
        data &= ~(1 << pin);
        writeRegister8(0x03, data);
    }
    // Output, set 1
    else {
        auto data = readRegister8(0x03);
        data |= (1 << pin);
        writeRegister8(0x03, data);
    }
}

void PI4IOE5V6408_Class::enablePull(uint8_t pin, bool enablePull)
{
    if (enablePull) {
        auto data = readRegister8(0x0B);
        data |= (1 << pin);
        writeRegister8(0x0B, data);
    } else {
        auto data = readRegister8(0x0B);
        data &= ~(1 << pin);
        writeRegister8(0x0B, data);
    }
}

// false down, true up
void PI4IOE5V6408_Class::setPullMode(uint8_t pin, bool mode)
{
    if (mode) {
        auto data = readRegister8(0x0D);
        data |= (1 << pin);
        writeRegister8(0x0D, data);
    } else {
        auto data = readRegister8(0x0D);
        data &= ~(1 << pin);
        writeRegister8(0x0D, data);
    }
}

void PI4IOE5V6408_Class::setHighImpedance(uint8_t pin, bool enable)
{
    auto data = readRegister8(0x07);

    if (enable) {
        data |= (1 << pin);
    } else {
        data &= ~(1 << pin);
    }

    writeRegister8(0x07, data);
}

void PI4IOE5V6408_Class::digitalWrite(uint8_t pin, bool level)
{
    auto data = readRegister8(0x05);
    // spdlog::info("data: {}", data);

    if (level) {
        data |= (1 << pin);
    } else {
        data &= ~(1 << pin);
    }

    writeRegister8(0x05, data);
}

bool PI4IOE5V6408_Class::digitalRead(uint8_t pin)
{
    auto data = readRegister8(0x0F);
    return (data & (1 << pin)) != 0;
}

void PI4IOE5V6408_Class::resetIrq()
{
    readRegister8(0x13);
}

void PI4IOE5V6408_Class::disableIrq()
{
    writeRegister8(0x11, 0B11111111);
}

void PI4IOE5V6408_Class::enableIrq()
{
    writeRegister8(0x11, 0x0);
}
