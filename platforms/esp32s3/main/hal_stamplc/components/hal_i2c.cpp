/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <utility/I2C_Class.hpp>
#include <shared/shared.h>
#include <Arduino.h>
#include "../utils/daemon_control/daemon_control.h"

void HAL_StamPLC::_i2c_init()
{
    spdlog::info("i2c bus init");

    /* -------------------------------- Internal -------------------------------- */
    spdlog::info("internal bus:");
    m5::In_I2C.begin(I2C_NUM_0, HAL_PIN_I2C_INTER_SDA, HAL_PIN_I2C_INTER_SCL);

    // Scan
    spdlog::info("start scan:");
    bool scan_list[120];
    m5::In_I2C.scanID(scan_list);
    uint8_t device_num = 0;
    for (int i = 8; i < 0x78; i++) {
        if (scan_list[i]) {
            device_num++;
            spdlog::info("get 0x{0:x}", i);
        }
    }
    spdlog::info("device num: {}", device_num);
}

/* -------------------------------------------------------------------------- */
/*                                 Log monitor                                */
/* -------------------------------------------------------------------------- */
void HAL_StamPLC::createLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
{
    if (monitorType == LogMonitor::I2C) {
        _create_i2c_lm_daemon();
    } else if (monitorType == LogMonitor::UART) {
        _create_uart_lm_daemon();
    } else if (monitorType == LogMonitor::Modbus) {
        SharedData::BorrowData();
        SharedData::Console().setModbusLoggingEnable(true);
        SharedData::Console().clear();
        SharedData::Console().log("> Listening PORT-RS485\n> Baudrate: 115200 ");
        SharedData::ReturnData();
    } else if (monitorType == LogMonitor::CAN) {
        _create_can_lm_daemon();
    }
}

void HAL_StamPLC::destroyLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
{
    if (monitorType == LogMonitor::I2C) {
        _destroy_i2c_lm_daemon();
    } else if (monitorType == LogMonitor::UART) {
        _destroy_uart_lm_daemon();
    } else if (monitorType == LogMonitor::Modbus) {
        SharedData::BorrowData();
        SharedData::Console().setModbusLoggingEnable(false);
        SharedData::ReturnData();
    } else if (monitorType == LogMonitor::CAN) {
        _destroy_can_lm_daemon();
    }
}

/* -------------------------------------------------------------------------- */
/*                                I2C lm daemon                               */
/* -------------------------------------------------------------------------- */
static DaemonControl_t* _daemon_control = nullptr;

static void _i2c_lm_daemon(void* param)
{
    SharedData::BorrowData();
    SharedData::Console().clear();
    SharedData::Console().log("> Scanning PORT-A for I2C Device");
    SharedData::Console().log("> Waiting .. ");
    SharedData::ReturnData();

    spdlog::info("start i2c lm daemon");
    spdlog::info("start external bus:");
    m5::Ex_I2C.begin(I2C_NUM_1, HAL_PIN_GROVE_RED_SDA, HAL_PIN_GROVE_RED_SCL);

    uint8_t time_count      = 233;
    uint8_t last_found_addr = 0;
    while (1) {
        vTaskDelay(pdTICKS_TO_MS(100));

        if (_daemon_control->CheckKillSignal()) {
            break;
        }

        time_count++;
        if (time_count > 1000 / 100) {
            time_count = 0;

            // Scan
            SharedData::BorrowData();

            bool scan_list[120];
            m5::Ex_I2C.scanID(scan_list);
            uint8_t device_num     = 0;
            uint8_t new_found_addr = 0;

            for (int i = 8; i < 0x78; i++) {
                if (scan_list[i]) {
                    device_num++;
                    new_found_addr = i;
                    break;
                }
            }
            if (device_num != 0 && new_found_addr != last_found_addr) {
                last_found_addr = new_found_addr;
                SharedData::Console().log("> Found device at >>> 0x{:02X}", last_found_addr);
            } else if (device_num == 0 && last_found_addr != 0) {
                SharedData::Console().log("> Device 0x{:02X} unplugged", last_found_addr);
                last_found_addr = 0;
            }

            SharedData::ReturnData();
        }
    }

    m5::Ex_I2C.release();
    _daemon_control->DaemonGone();
    vTaskDelete(NULL);
}

void HAL_StamPLC::_create_i2c_lm_daemon()
{
    if (_daemon_control != nullptr) {
        spdlog::warn("i2c lm daemon already exist");
        return;
    }

    _daemon_control = new DaemonControl_t;
    xTaskCreate(_i2c_lm_daemon, "i2clm", 4000, NULL, 10, NULL);
}

void HAL_StamPLC::_destroy_i2c_lm_daemon()
{
    if (_daemon_control == nullptr) {
        spdlog::warn("i2c lm daemon not exist");
        return;
    }

    _daemon_control->SendKillSignalAndWait();
    delay(20);

    delete _daemon_control;
    _daemon_control = nullptr;
}

/* -------------------------------------------------------------------------- */
/*                               UART lm daemon                               */
/* -------------------------------------------------------------------------- */
HardwareSerial* _serial = nullptr;

static void _uart_lm_daemon(void* param)
{
    spdlog::info("start uart lm daemon");

    SharedData::BorrowData();
    SharedData::Console().clear();
    SharedData::Console().log("> Waiting uart msg from PORT-C\n> Baudrate: 115200 ");
    SharedData::ReturnData();

    _serial = new HardwareSerial(2);
    _serial->begin(115200, SERIAL_8N1, HAL_PIN_GROVE_BLUE_SCL, HAL_PIN_GROVE_BLUE_SDA);

    uint8_t time_count = 0;
    while (1) {
        vTaskDelay(pdTICKS_TO_MS(100));

        if (_daemon_control->CheckKillSignal()) {
            break;
        }

        // Check input
        if (_serial->available()) {
            SharedData::BorrowData();
            SharedData::Console().setAutoNewLine(false);

            SharedData::Console().log("\n> Get msg: ");
            while (_serial->available()) {
                SharedData::Console().put(_serial->read());
            }
            SharedData::Console().setAutoNewLine(true);

            SharedData::ReturnData();
        }

        // Send output
        time_count++;
        if (time_count > 1000 / 100) {
            time_count = 0;
            spdlog::info("send uart msg");
            _serial->println("StamPLC: Hi~");
        }
    }

    _serial->end();
    _daemon_control->DaemonGone();
    vTaskDelete(NULL);
}

void HAL_StamPLC::_create_uart_lm_daemon()
{
    if (_daemon_control != nullptr) {
        spdlog::warn("uart lm daemon already exist");
        return;
    }

    _daemon_control = new DaemonControl_t;
    xTaskCreate(_uart_lm_daemon, "uartlm", 4000, NULL, 10, NULL);
}

void HAL_StamPLC::_destroy_uart_lm_daemon()
{
    if (_daemon_control == nullptr) {
        spdlog::warn("uart lm daemon not exist");
        return;
    }

    _daemon_control->SendKillSignalAndWait();
    delay(20);

    delete _daemon_control;
    _daemon_control = nullptr;
}
