/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "rx8130.h"

// RX-8130 Register definitions
#define RX8130_REG_SEC   0x10
#define RX8130_REG_MIN   0x11
#define RX8130_REG_HOUR  0x12
#define RX8130_REG_WDAY  0x13
#define RX8130_REG_MDAY  0x14
#define RX8130_REG_MONTH 0x15
#define RX8130_REG_YEAR  0x16

#define RX8130_REG_ALMIN   0x17
#define RX8130_REG_ALHOUR  0x18
#define RX8130_REG_ALWDAY  0x19
#define RX8130_REG_TCOUNT0 0x1A
#define RX8130_REG_TCOUNT1 0x1B
#define RX8130_REG_EXT     0x1C
#define RX8130_REG_FLAG    0x1D
#define RX8130_REG_CTRL0   0x1E
#define RX8130_REG_CTRL1   0x1F

#define RX8130_REG_END 0x23

// Extension Register (1Ch) bit positions
#define RX8130_BIT_EXT_TSEL (7 << 0)
#define RX8130_BIT_EXT_WADA (1 << 3)
#define RX8130_BIT_EXT_TE   (1 << 4)
#define RX8130_BIT_EXT_USEL (1 << 5)
#define RX8130_BIT_EXT_FSEL (3 << 6)

// Flag Register (1Dh) bit positions
#define RX8130_BIT_FLAG_VLF (1 << 1)
#define RX8130_BIT_FLAG_AF  (1 << 3)
#define RX8130_BIT_FLAG_TF  (1 << 4)
#define RX8130_BIT_FLAG_UF  (1 << 5)

// Control 0 Register (1Еh) bit positions
#define RX8130_BIT_CTRL_TSTP (1 << 2)
#define RX8130_BIT_CTRL_AIE  (1 << 3)
#define RX8130_BIT_CTRL_TIE  (1 << 4)
#define RX8130_BIT_CTRL_UIE  (1 << 5)
#define RX8130_BIT_CTRL_STOP (1 << 6)
#define RX8130_BIT_CTRL_TEST (1 << 7)

static uint8_t bcd2dec(uint8_t val)
{
    return (val >> 4) * 10 + (val & 0x0f);
}

static uint8_t dec2bcd(uint8_t val)
{
    return ((val / 10) << 4) + (val % 10);
}

bool RX8130_Class::begin()
{
    bool result = _i2c->start(_addr, false, _freq) && _i2c->stop();
    return result;
}

void RX8130_Class::setTime(struct tm *time)
{
    uint8_t rbuf = 0;

    time->tm_year -= 100;

    // set STOP bit before changing clock/calendar
    rbuf = readRegister8(RX8130_REG_CTRL0);
    rbuf = rbuf | RX8130_BIT_CTRL_STOP;
    writeRegister8(RX8130_REG_CTRL0, rbuf);

    uint8_t date[7] = {dec2bcd(time->tm_sec),       dec2bcd(time->tm_min),  dec2bcd(time->tm_hour),
                       dec2bcd(time->tm_wday),      dec2bcd(time->tm_mday), dec2bcd(time->tm_mon),
                       dec2bcd(time->tm_year % 100)};

    writeRegister(RX8130_REG_SEC, date, 7);

    // clear STOP bit after changing clock/calendar
    rbuf = readRegister8(RX8130_REG_CTRL0);
    rbuf = rbuf & ~RX8130_BIT_CTRL_STOP;
    writeRegister8(RX8130_REG_CTRL0, rbuf);
}

void RX8130_Class::getTime(struct tm *time)
{
    uint8_t date[7];
    readRegister(RX8130_REG_SEC, date, 7);

    time->tm_sec  = bcd2dec(date[RX8130_REG_SEC - 0x10] & 0x7f);
    time->tm_min  = bcd2dec(date[RX8130_REG_MIN - 0x10] & 0x7f);
    time->tm_hour = bcd2dec(date[RX8130_REG_HOUR - 0x10] & 0x3f);  // only 24-hour clock
    time->tm_mday = bcd2dec(date[RX8130_REG_MDAY - 0x10] & 0x3f);
    time->tm_mon  = bcd2dec(date[RX8130_REG_MONTH - 0x10] & 0x1f);
    time->tm_year = bcd2dec(date[RX8130_REG_YEAR - 0x10]);
    time->tm_wday = bcd2dec(date[RX8130_REG_WDAY - 0x10] & 0x7f);

    time->tm_year += 100;
}

void RX8130_Class::clearIrqFlags()
{
    writeRegister8(RX8130_REG_FLAG, 0);
}

void RX8130_Class::disableIrq()
{
    writeRegister8(RX8130_REG_CTRL0, 0);
}
