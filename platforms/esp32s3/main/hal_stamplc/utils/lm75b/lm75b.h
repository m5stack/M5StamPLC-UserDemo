/* LM75B Driver Library
 * Copyright (c) 2013 Neil Thiessen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <M5Unified.h>
#include <driver/gpio.h>

// https://www.nxp.com/docs/en/data-sheet/LM75B.pdf
// https://github.com/ARMmbed/LM75B/
class LM75B_Class : public m5::I2C_Device {
public:
    /** Represents the different I2C address possibilities for the LM75B
     */
    enum Address {
        ADDRESS_0 = (0x48 << 1), /**< A[2:0] pins = 000 */
        ADDRESS_1 = (0x49 << 1), /**< A[2:0] pins = 001 */
        ADDRESS_2 = (0x4A << 1), /**< A[2:0] pins = 010 */
        ADDRESS_3 = (0x4B << 1), /**< A[2:0] pins = 011 */
        ADDRESS_4 = (0x4C << 1), /**< A[2:0] pins = 100 */
        ADDRESS_5 = (0x4D << 1), /**< A[2:0] pins = 101 */
        ADDRESS_6 = (0x4E << 1), /**< A[2:0] pins = 110 */
        ADDRESS_7 = (0x4F << 1)  /**< A[2:0] pins = 111 */
    };

    /** Represents the power mode of the LM75B
     */
    enum PowerMode {
        POWER_NORMAL,  /**< Chip is enabled and samples every 100ms */
        POWER_SHUTDOWN /**< Chip is in low-power shutdown mode */
    };

    /** Represents OS pin mode of the LM75B
     */
    enum OSMode {
        OS_COMPARATOR, /**< OS is asserted when the temperature reaches the alert threshold, and de-asserted when the
                          temperature drops below the alert hysteresis threshold */
        OS_INTERRUPT   /**< OS is asserted when the temperature reaches the alert threshold, or drops below the alert
                          hysteresis   threshold, and only de-asserted when a register has been read */
    };

    /** Represents OS pin polarity of the LM75B
     */
    enum OSPolarity {
        OS_ACTIVE_LOW, /**< OS is a logic low when asserted, and a logic high when de-asserted */
        OS_ACTIVE_HIGH /**< OS is a logic high when asserted, and a logic low when de-asserted */
    };

    /** Represents OS pin fault queue length of the LM75B
     */
    enum OSFaultQueue {
        OS_FAULT_QUEUE_1, /**< OS is asserted after 1 fault */
        OS_FAULT_QUEUE_2, /**< OS is asserted after 2 consecutive faults */
        OS_FAULT_QUEUE_4, /**< OS is asserted after 4 consecutive faults */
        OS_FAULT_QUEUE_6  /**< OS is asserted after 6 consecutive faults */
    };

    LM75B_Class(std::uint8_t i2c_addr = 0x48, std::uint32_t freq = 400000, m5::I2C_Class* i2c = &m5::In_I2C)
        : I2C_Device(i2c_addr, freq, i2c)
    {
    }

    bool begin();

    /** Get the current power mode of the LM75B
     *
     * @returns The current power mode as a PowerMode enum.
     */
    PowerMode powerMode();

    /** Set the power mode of the LM75B
     *
     * @param mode The new power mode as a PowerMode enum.
     */
    void powerMode(PowerMode mode);

    /** Get the current OS pin mode of the LM75B
     *
     * @returns The current OS pin mode as an OSMode enum.
     */
    OSMode osMode();

    /** Set the OS pin mode of the LM75B
     *
     * @param mode The new OS pin mode as an OSMode enum.
     */
    void osMode(OSMode mode);

    /** Get the current OS pin polarity of the LM75B
     *
     * @returns The current OS pin polarity as an OSPolarity enum.
     */
    OSPolarity osPolarity();

    /** Set the OS pin polarity of the LM75B
     *
     * @param polarity The new OS pin polarity as an OSPolarity enum.
     */
    void osPolarity(OSPolarity polarity);

    /** Get the current OS pin fault queue length of the LM75B
     *
     * @returns The current OS pin fault queue length as an OSFaultQueue enum.
     */
    OSFaultQueue osFaultQueue();

    /** Set the OS pin fault queue length of the LM75B
     *
     * @param queue The new OS pin fault queue length as an OSFaultQueue enum.
     */
    void osFaultQueue(OSFaultQueue queue);

    /** Get the current alert temperature threshold of the LM75B
     *
     * @returns The current alert temperature threshold in °C.
     */
    float alertTemp();

    /** Set the alert temperature threshold of the LM75B
     *
     * @param temp The new alert temperature threshold in °C.
     */
    void alertTemp(float temp);

    /** Get the current alert temperature hysteresis threshold of the LM75B
     *
     * @returns The current alert temperature hysteresis threshold in °C.
     */
    float alertHyst();

    /** Set the alert temperature hysteresis threshold of the LM75B
     *
     * @param temp The new alert temperature hysteresis threshold in °C.
     */
    void alertHyst(float temp);

    /** Get the current temperature measurement of the LM75B
     *
     * @returns The current temperature measurement in °C.
     */
    float temp();

private:
    // I2C register addresses
    enum Register { REG_TEMP = 0x00, REG_CONF = 0x01, REG_THYST = 0x02, REG_TOS = 0x03 };

    // Internal functions
    char read8(char reg);
    void write8(char reg, char data);
    unsigned short read16(char reg);
    void write16(char reg, unsigned short data);
    float readAlertTempHelper(char reg);
    void writeAlertTempHelper(char reg, float temp);
};
