#include "lm75b.h"

bool LM75B_Class::begin()
{
    bool result = _i2c->start(_addr, false, _freq) && _i2c->stop();
    return result;
}

LM75B_Class::PowerMode LM75B_Class::powerMode()
{
    // Read the 8-bit register value
    char value = read8(REG_CONF);

    // Return the status of the SHUTDOWN bit
    if (value & (1 << 0)) {
        return POWER_SHUTDOWN;
    } else {
        return POWER_NORMAL;
    }
}

void LM75B_Class::powerMode(PowerMode mode)
{
    // Read the current 8-bit register value
    char value = read8(REG_CONF);

    // Set or clear the SHUTDOWN bit
    if (mode == POWER_SHUTDOWN) {
        value |= (1 << 0);
    } else {
        value &= ~(1 << 0);
    }

    // Write the value back out
    write8(REG_CONF, value);
}

LM75B_Class::OSMode LM75B_Class::osMode()
{
    // Read the 8-bit register value
    char value = read8(REG_CONF);

    // Return the status of the OS_COMP_INT bit
    if (value & (1 << 1)) {
        return OS_INTERRUPT;
    } else {
        return OS_COMPARATOR;
    }
}

void LM75B_Class::osMode(OSMode mode)
{
    // Read the current 8-bit register value
    char value = read8(REG_CONF);

    // Set or clear the OS_COMP_INT bit
    if (mode == OS_INTERRUPT) {
        value |= (1 << 1);
    } else {
        value &= ~(1 << 1);
    }

    // Write the value back out
    write8(REG_CONF, value);
}

LM75B_Class::OSPolarity LM75B_Class::osPolarity()
{
    // Read the 8-bit register value
    char value = read8(REG_CONF);

    // Return the status of the OS_POL bit
    if (value & (1 << 2)) {
        return OS_ACTIVE_HIGH;
    } else {
        return OS_ACTIVE_LOW;
    }
}

void LM75B_Class::osPolarity(OSPolarity polarity)
{
    // Read the current 8-bit register value
    char value = read8(REG_CONF);

    // Set or clear the OS_POL bit
    if (polarity == OS_ACTIVE_HIGH) {
        value |= (1 << 2);
    } else {
        value &= ~(1 << 2);
    }

    // Write the value back out
    write8(REG_CONF, value);
}

LM75B_Class::OSFaultQueue LM75B_Class::osFaultQueue()
{
    // Read the 8-bit register value
    char value = read8(REG_CONF);

    // Return the status of the OS_F_QUE bits
    if ((value & (1 << 3)) && (value & (1 << 4))) {
        return OS_FAULT_QUEUE_6;
    } else if (!(value & (1 << 3)) && (value & (1 << 4))) {
        return OS_FAULT_QUEUE_4;
    } else if ((value & (1 << 3)) && !(value & (1 << 4))) {
        return OS_FAULT_QUEUE_2;
    } else {
        return OS_FAULT_QUEUE_1;
    }
}

void LM75B_Class::osFaultQueue(OSFaultQueue queue)
{
    // Read the current 8-bit register value
    char value = read8(REG_CONF);

    // Clear the old OS_F_QUE bits
    value &= ~(3 << 3);

    // Set the new OS_F_QUE bits
    if (queue == OS_FAULT_QUEUE_2) {
        value |= (1 << 3);
    } else if (queue == OS_FAULT_QUEUE_4) {
        value |= (2 << 3);
    } else if (queue == OS_FAULT_QUEUE_6) {
        value |= (3 << 3);
    }

    // Write the value back out
    write8(REG_CONF, value);
}

float LM75B_Class::alertTemp()
{
    // Use the 9-bit helper to read the TOS register
    return readAlertTempHelper(REG_TOS);
}

void LM75B_Class::alertTemp(float temp)
{
    // Use the 9-bit helper to write to the TOS register
    return writeAlertTempHelper(REG_TOS, temp);
}

float LM75B_Class::alertHyst()
{
    // Use the 9-bit helper to read the THYST register
    return readAlertTempHelper(REG_THYST);
}

void LM75B_Class::alertHyst(float temp)
{
    // Use the 9-bit helper to write to the THYST register
    return writeAlertTempHelper(REG_THYST, temp);
}

float LM75B_Class::temp()
{
    // Signed return value
    short value;

    // Read the 11-bit raw temperature value
    value = read16(REG_TEMP) >> 5;

    // Sign extend negative numbers
    if (value & (1 << 10)) {
        value |= 0xFC00;
    }

    // Return the temperature in °C
    return value * 0.125;
}

char LM75B_Class::read8(char reg)
{
    return readRegister8(reg);
}

void LM75B_Class::write8(char reg, char data)
{
    writeRegister8(reg, data);
}

unsigned short LM75B_Class::read16(char reg)
{
    // Create a temporary buffer
    uint8_t buff[2];

    // Read the 16-bit register
    readRegister(reg, buff, 2);

    // Return the combined 16-bit value
    return (buff[0] << 8) | buff[1];
}

void LM75B_Class::write16(char reg, unsigned short data)
{
    // Create a temporary buffer
    uint8_t buff[2];

    // Load the register address and 16-bit data
    buff[0] = data >> 8;
    buff[1] = data;

    // Write the data
    writeRegister(reg, buff, 2);
}

float LM75B_Class::readAlertTempHelper(char reg)
{
    // Signed return value
    short value;

    // Read the 9-bit raw temperature value
    value = read16(reg) >> 7;

    // Sign extend negative numbers
    if (value & (1 << 8)) {
        value |= 0xFF00;
    }

    // Return the temperature in °C
    return value * 0.5;
}

void LM75B_Class::writeAlertTempHelper(char reg, float temp)
{
    // Range limit temp
    if (temp < -55.0) {
        temp = -55.0;
    } else if (temp > 125.0) {
        temp = 125.0;
    }

    // Extract and shift the signed integer
    short value = temp * 2;
    value <<= 7;

    // Send the new value
    write16(reg, value);
}
