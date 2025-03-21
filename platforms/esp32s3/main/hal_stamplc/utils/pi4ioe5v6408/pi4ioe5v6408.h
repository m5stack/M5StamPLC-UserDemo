/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <mooncake.h>
#include <M5Unified.h>
#include <driver/gpio.h>

// https://www.diodes.com/assets/Datasheets/PI4IOE5V6408.pdf
class PI4IOE5V6408_Class : public m5::I2C_Device {
public:
    PI4IOE5V6408_Class(std::uint8_t i2c_addr = 0x43, std::uint32_t freq = 400000, m5::I2C_Class* i2c = &m5::In_I2C)
        : I2C_Device(i2c_addr, freq, i2c)
    {
    }

    bool begin();

    // false input, true output
    void setDirection(uint8_t pin, bool direction);

    void enablePull(uint8_t pin, bool enablePull);

    // false down, true up
    void setPullMode(uint8_t pin, bool mode);

    void setHighImpedance(uint8_t pin, bool enable);

    void digitalWrite(uint8_t pin, bool level);

    bool digitalRead(uint8_t pin);

    void resetIrq();

    void disableIrq();

    void enableIrq();
};
