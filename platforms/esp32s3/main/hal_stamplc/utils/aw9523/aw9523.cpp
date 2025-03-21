/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "aw9523.h"

#define AW9523_DEFAULT_ADDR 0x58  ///< The default I2C address for our breakout

#define AW9523_LED_MODE 0x99  ///< Special pinMode() macro for constant current

#define AW9523_REG_CHIPID     0x10  ///< Register for hardcode chip ID
#define AW9523_REG_SOFTRESET  0x7F  ///< Register for soft resetting
#define AW9523_REG_INPUT0     0x00  ///< Register for reading input values
#define AW9523_REG_OUTPUT0    0x02  ///< Register for writing output values
#define AW9523_REG_CONFIG0    0x04  ///< Register for configuring direction
#define AW9523_REG_INTENABLE0 0x06  ///< Register for enabling interrupt
#define AW9523_REG_GCR        0x11  ///< Register for general configuration
#define AW9523_REG_LEDMODE    0x12  ///< Register for configuring const current

bool AW9523_Class::begin()
{
    auto id = readRegister8(0x10);
    spdlog::info("device id {}", id);
    if (id != 0x23) {
        return false;
    }
    return true;
}

/*!
 *    @brief  Perform a soft reset over I2C
 *    @return True I2C reset command was acknowledged
 */
bool AW9523_Class::reset(void)
{
    return writeRegister8(AW9523_REG_SOFTRESET, 0);
}

/*!
 *    @brief  Sets output value (1 == high) for all 16 GPIO
 *    @param  pins 16-bits of binary output settings
 *    @return True I2C write command was acknowledged
 */
bool AW9523_Class::outputGPIO(uint16_t pins)
{
    if (!writeRegister8(AW9523_REG_OUTPUT0, pins & 0xFF)) {
        return false;
    }
    if (!writeRegister8(AW9523_REG_OUTPUT0 + 1, pins >> 8)) {
        return false;
    }
    return true;
}

/*!
 *    @brief  Reads input value (1 == high) for all 16 GPIO
 *    @return 16-bits of binary input (0 == low & 1 == high)
 */
uint16_t AW9523_Class::inputGPIO(void)
{
    return ((uint16_t)readRegister8(AW9523_REG_INPUT0 + 1) << 8) | (uint16_t)readRegister8(AW9523_REG_INPUT0);
}

/*!
 *    @brief  Sets interrupt watch for all 16 GPIO, 1 == enabled
 *    @param  pins 16-bits of pins we may want INT enabled
 *    @return True I2C write command was acknowledged
 */
bool AW9523_Class::interruptEnableGPIO(uint16_t pins)
{
    if (!writeRegister8(AW9523_REG_INTENABLE0, ~(pins & 0xFF))) {
        return false;
    }
    if (!writeRegister8(AW9523_REG_INTENABLE0 + 1, ~(pins >> 8))) {
        return false;
    }
    return true;
}

/*!
 *    @brief  Sets direction for all 16 GPIO, 1 == output, 0 == input
 *    @param  pins 16-bits of pins we want to set direction
 *    @return True I2C write command was acknowledged
 */
bool AW9523_Class::configureDirection(uint16_t pins)
{
    if (!writeRegister8(AW9523_REG_CONFIG0, ~(pins & 0xFF))) {
        return false;
    }
    if (!writeRegister8(AW9523_REG_CONFIG0 + 1, ~(pins >> 8))) {
        return false;
    }
    return true;
}

/*!
 *    @brief  Sets digital output for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  val True for high value, False for low value
 */
void AW9523_Class::digitalWrite(uint8_t pin, bool val)
{
    uint8_t reg = AW9523_REG_OUTPUT0 + (pin / 8);
    if (val) {
        bitOn(reg, 0x01 << (pin % 8));
    } else {
        bitOff(reg, 0x01 << (pin % 8));
    }
}

/*!
 *    @brief  Reads digital input for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @returns True for high value read, False for low value read
 */
bool AW9523_Class::digitalRead(uint8_t pin)
{
    uint8_t reg = AW9523_REG_INPUT0 + (pin / 8);
    auto data   = readRegister8(reg);
    return (data & (1 << (pin % 8))) != 0;
}

/*!
 *    @brief  Sets interrupt enable for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  en True to enable Interrupt detect, False for ignore
 */
void AW9523_Class::enableInterrupt(uint8_t pin, bool en)
{
    uint8_t reg = AW9523_REG_INTENABLE0 + (pin / 8);
    if (!en) {
        bitOn(reg, 0x01 << (pin % 8));
    } else {
        bitOff(reg, 0x01 << (pin % 8));
    }
}

/*!
 *    @brief  Sets pin mode / direction for one pin
 *    @param  pin GPIO to set, from 0 to 15 inclusive
 *    @param  mode Can be INPUT, OUTPUT for GPIO digital, or AW9523_LED_MODE for
 * constant current LED drive
 */
void AW9523_Class::pinMode(uint8_t pin, uint8_t mode)
{
    if (mode == AW_OUTPUT) {
        uint8_t reg = AW9523_REG_CONFIG0 + (pin / 8);
        bitOff(reg, 0x01 << (pin % 8));
        reg = AW9523_REG_LEDMODE + (pin / 8);
        bitOn(reg, 0x01 << (pin % 8));
    }
    if (mode == AW_INPUT) {
        uint8_t reg = AW9523_REG_CONFIG0 + (pin / 8);
        bitOn(reg, 0x01 << (pin % 8));
        reg = AW9523_REG_LEDMODE + (pin / 8);
        bitOn(reg, 0x01 << (pin % 8));
    }

    if (mode == AW9523_LED_MODE) {
        uint8_t reg = AW9523_REG_CONFIG0 + (pin / 8);
        bitOff(reg, 0x01 << (pin % 8));
        reg = AW9523_REG_LEDMODE + (pin / 8);
        bitOff(reg, 0x01 << (pin % 8));
    }
}

/*!
 *    @brief  Turns on/off open drain output for ALL port 0 pins (GPIO 0-7)
 *    @param  od True to enable open drain, False for push-pull
 *    @return True if I2C write command was acknowledged, otherwise false
 */
bool AW9523_Class::openDrainPort0(bool od)
{
    uint8_t reg = AW9523_REG_GCR;
    if (!od) {
        return bitOn(reg, 0x01 << 4);
    }
    return bitOff(reg, 0x01 << 4);
}
