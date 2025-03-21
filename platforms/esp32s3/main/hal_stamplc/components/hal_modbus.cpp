/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
// https://github.com/espressif/esp-idf/blob/v5.1.4/examples/protocols/modbus/serial/mb_slave
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <shared/shared.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <mbcontroller.h>                          // for mbcontroller defines and api
#include "../utils/modbus_params/modbus_params.h"  // for modbus parameters structures
#include <esp_log.h>
#include <sdkconfig.h>
#include <thread>
#include <mutex>
#include <Arduino.h>

#define MB_PORT_NUM (CONFIG_MB_UART_PORT_NUM)  // Number of UART port used for Modbus connection
// #define MB_SLAVE_ADDR (CONFIG_MB_SLAVE_ADDR)      // The address of device in Modbus network
#define MB_DEV_SPEED (CONFIG_MB_UART_BAUD_RATE)  // The communication speed of the UART

// Note: Some pins on target chip cannot be assigned for UART communication.
// Please refer to documentation for selected board and target to configure pins using Kconfig.

// Defines below are used to define register start address for each type of Modbus registers
#define HOLD_OFFSET(field)          ((uint16_t)(offsetof(holding_reg_params_t, field) >> 1))
#define INPUT_OFFSET(field)         ((uint16_t)(offsetof(input_reg_params_t, field) >> 1))
#define MB_REG_DISCRETE_INPUT_START (0x0000)
#define MB_REG_COILS_START          (0x0000)
#define MB_REG_INPUT_START_AREA0    (INPUT_OFFSET(input_data0))  // register offset input area 0
#define MB_REG_INPUT_START_AREA1    (INPUT_OFFSET(input_data4))  // register offset input area 1
#define MB_REG_HOLDING_START_AREA0  (HOLD_OFFSET(holding_data0))
#define MB_REG_HOLDING_START_AREA1  (HOLD_OFFSET(holding_data4))

#define MB_PAR_INFO_GET_TOUT (10)  // Timeout for get parameter info
#define MB_CHAN_DATA_MAX_VAL (6)
#define MB_CHAN_DATA_OFFSET  (0.2f)
#define MB_READ_MASK         (MB_EVENT_INPUT_REG_RD | MB_EVENT_HOLDING_REG_RD | MB_EVENT_DISCRETE_RD | MB_EVENT_COILS_RD)
#define MB_WRITE_MASK        (MB_EVENT_HOLDING_REG_WR | MB_EVENT_COILS_WR)
#define MB_READ_WRITE_MASK   (MB_READ_MASK | MB_WRITE_MASK)

static const char* TAG = "mb";

static portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;

void _float32_2_uint16(float value, uint16_t& high, uint16_t& low)
{
    // 将 float 转换为 uint32_t
    uint32_t temp;
    std::memcpy(&temp, &value, sizeof(float));

    // 提取高 16 位和低 16 位
    high = (temp >> 16) & 0xFFFF;
    low  = temp & 0xFFFF;
}

// Set register values into known state
static void setup_reg_data(void)
{
    coil_reg_params.coils_port0 = 0x00;
    coil_reg_params.coils_port1 = 0x00;

    input_reg_params.input_data0 = 0;
    input_reg_params.input_data1 = 0;
    input_reg_params.input_data2 = 0;
    input_reg_params.input_data3 = 0;
    input_reg_params.input_data4 = 0;
    input_reg_params.input_data5 = 0;
    input_reg_params.input_data6 = 0;
    input_reg_params.input_data7 = 0;

    _float32_2_uint16(0.0f, input_reg_params.input_temp_0, input_reg_params.input_temp_1);
    _float32_2_uint16(0.0f, input_reg_params.input_voltage_0, input_reg_params.input_voltage_1);
    _float32_2_uint16(0.0f, input_reg_params.input_current_0, input_reg_params.input_current_1);
}

static void _handle_update_plc_inputs()
{
    input_reg_params.input_data0 = HAL::GetPlcInput(0);
    input_reg_params.input_data1 = HAL::GetPlcInput(1);
    input_reg_params.input_data2 = HAL::GetPlcInput(2);
    input_reg_params.input_data3 = HAL::GetPlcInput(3);
    input_reg_params.input_data4 = HAL::GetPlcInput(4);
    input_reg_params.input_data5 = HAL::GetPlcInput(5);
    input_reg_params.input_data6 = HAL::GetPlcInput(6);
    input_reg_params.input_data7 = HAL::GetPlcInput(7);
}

static void _handle_update_temp()
{
    _float32_2_uint16(HAL::GetTemp(), input_reg_params.input_temp_0, input_reg_params.input_temp_1);
}

static void _handle_update_voltage()
{
    _float32_2_uint16(HAL::GetPowerVoltage(), input_reg_params.input_voltage_0, input_reg_params.input_voltage_1);
}

static void _handle_update_current()
{
    _float32_2_uint16(HAL::GetExpanderOutputCurrent(), input_reg_params.input_current_0,
                      input_reg_params.input_current_1);
}

static void _handle_update_coils()
{
    HAL::SetPlcAllRelayState(coil_reg_params.coils_port0);
}

static void _modbus_daemon(void* param)
{
    // Wait until hal init done
    while (!HAL::Check()) vTaskDelay(pdMS_TO_TICKS(200));

    spdlog::info("start modbus daemon");

    mb_param_info_t reg_info;  // keeps the Modbus registers access information

    // The cycle below will be terminated when parameter holdingRegParams.dataChan0
    // incremented each access cycle reaches the CHAN_DATA_MAX_VAL value.
    // for (; holding_reg_params.holding_data0 < MB_CHAN_DATA_MAX_VAL;)
    while (1) {
        // Check for read/write events of Modbus master for certain events
        (void)mbc_slave_check_event((mb_event_group_t)MB_READ_WRITE_MASK);
        ESP_ERROR_CHECK_WITHOUT_ABORT(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
        const char* rw_str = (reg_info.type & MB_READ_MASK) ? "R" : "W";
        // Filter events and process them accordingly
        if (reg_info.type & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD)) {
            SharedData::BorrowData();
            if (SharedData::Console().isModbusLoggingEnable()) {
                SharedData::Console().log("> Get msg: H-{} addr: {} size: {}", rw_str, (unsigned)reg_info.mb_offset,
                                          (unsigned)reg_info.size);
            } else {
                spdlog::info("Modbus H-{} addr:{} size:{}", rw_str, (unsigned)reg_info.mb_offset,
                             (unsigned)reg_info.size);
            }
            SharedData::ReturnData();
        } else if (reg_info.type & MB_EVENT_INPUT_REG_RD) {
            SharedData::BorrowData();
            if (SharedData::Console().isModbusLoggingEnable()) {
                SharedData::Console().log("> Get msg: I-{} addr: {} size: {}", rw_str, (unsigned)reg_info.mb_offset,
                                          (unsigned)reg_info.size);
            } else {
                spdlog::info("Modbus I-{} addr:{} size:{}", rw_str, (unsigned)reg_info.mb_offset,
                             (unsigned)reg_info.size);
            }
            SharedData::ReturnData();

            portENTER_CRITICAL(&param_lock);
            // _handle_update_inputs();

            if (reg_info.address == (uint8_t*)&input_reg_params.input_temp_0) {
                _handle_update_temp();
            } else if (reg_info.address == (uint8_t*)&input_reg_params.input_voltage_0) {
                _handle_update_voltage();
            } else if (reg_info.address == (uint8_t*)&input_reg_params.input_current_0) {
                _handle_update_current();
            } else {
                _handle_update_plc_inputs();
            }

            portEXIT_CRITICAL(&param_lock);
        } else if (reg_info.type & MB_EVENT_DISCRETE_RD) {
            SharedData::BorrowData();
            if (SharedData::Console().isModbusLoggingEnable()) {
                SharedData::Console().log("> Get msg: D-{} addr: {} size: {}", rw_str, (unsigned)reg_info.mb_offset,
                                          (unsigned)reg_info.size);
            } else {
                spdlog::info("Modbus D-{} addr:{} size:{}", rw_str, (unsigned)reg_info.mb_offset,
                             (unsigned)reg_info.size);
            }
            SharedData::ReturnData();
        } else if (reg_info.type & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR)) {
            SharedData::BorrowData();
            if (SharedData::Console().isModbusLoggingEnable()) {
                SharedData::Console().log("> Get msg: C-{} addr: {} size: {}", rw_str, (unsigned)reg_info.mb_offset,
                                          (unsigned)reg_info.size);
            } else {
                spdlog::info("Modbus C-{} addr:{} size:{}", rw_str, (unsigned)reg_info.mb_offset,
                             (unsigned)reg_info.size);
            }
            SharedData::ReturnData();

            // portENTER_CRITICAL(&param_lock);
            _handle_update_coils();
            // portEXIT_CRITICAL(&param_lock);
        }
    }

    vTaskDelete(NULL);
}

void HAL_StamPLC::_modbus_init()
{
    spdlog::info("modbus init");

    Serial0.end();
    Serial1.end();
    Serial2.end();

    mb_communication_info_t comm_info;       // Modbus communication parameters
    mb_register_area_descriptor_t reg_area;  // Modbus register area descriptor structure

    // Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    void* mbc_slave_handler = NULL;

    ESP_ERROR_CHECK(mbc_slave_init(MB_PORT_SERIAL_SLAVE, &mbc_slave_handler));  // Initialization of Modbus controller

    // Setup communication parameters and start stack
#if CONFIG_MB_COMM_MODE_ASCII
    comm_info.mode = MB_MODE_ASCII,
#elif CONFIG_MB_COMM_MODE_RTU
    comm_info.mode = MB_MODE_RTU,
#endif
    // comm_info.slave_addr = MB_SLAVE_ADDR;
        comm_info.slave_addr = _data.config.modbusSlaveAddress;
    comm_info.port           = MB_PORT_NUM;
    comm_info.baudrate       = MB_DEV_SPEED;
    comm_info.parity         = MB_PARITY_NONE;
    ESP_ERROR_CHECK(mbc_slave_setup((void*)&comm_info));

    reg_area.type         = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA0;
    reg_area.address      = (void*)&input_reg_params.input_data0;
    reg_area.size         = 16;
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    // Temp
    reg_area.type         = MB_PARAM_INPUT;
    reg_area.start_offset = INPUT_OFFSET(input_temp_0);
    reg_area.address      = (void*)&input_reg_params.input_temp_0;
    reg_area.size         = 4;
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    // Voltage
    reg_area.type         = MB_PARAM_INPUT;
    reg_area.start_offset = INPUT_OFFSET(input_voltage_0);
    reg_area.address      = (void*)&input_reg_params.input_voltage_0;
    reg_area.size         = 4;
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    // Current
    reg_area.type         = MB_PARAM_INPUT;
    reg_area.start_offset = INPUT_OFFSET(input_current_0);
    reg_area.address      = (void*)&input_reg_params.input_current_0;
    reg_area.size         = 4;
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    // Initialization of Coils register area
    reg_area.type         = MB_PARAM_COIL;
    reg_area.start_offset = MB_REG_COILS_START;
    reg_area.address      = (void*)&coil_reg_params;
    reg_area.size         = sizeof(coil_reg_params);
    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));

    setup_reg_data();  // Set values into known state

    // Starts of modbus controller and stack
    ESP_ERROR_CHECK(mbc_slave_start());

    // Set UART pin numbers
    ESP_ERROR_CHECK(
        uart_set_pin(MB_PORT_NUM, CONFIG_MB_UART_TXD, CONFIG_MB_UART_RXD, CONFIG_MB_UART_RTS, UART_PIN_NO_CHANGE));

    // Set UART driver mode to Half Duplex
    ESP_ERROR_CHECK(uart_set_mode(MB_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    xTaskCreate(_modbus_daemon, "mb", 4000, NULL, 15, NULL);
}
