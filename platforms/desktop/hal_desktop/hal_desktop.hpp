/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <mooncake.h>
#include <M5GFX.h>
#include "LGFX_AutoDetect_sdl.hpp"
#include "spdlog/spdlog.h"
#include <hal/hal.h>
#include <cmath>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <shared/shared.h>

class HAL_Desktop : public HAL {
public:
    std::string type() override
    {
        return "Desktop";
    }

    HAL_Desktop(int screenWidth = 240, int screenHeight = 240)
    {
        _screenWidth  = screenWidth;
        _screenHeight = screenHeight;
    }

    void init() override
    {
        // Display
        _data.display = new LGFX(_screenWidth, _screenHeight);
        _data.display->init();

        // Canvas
        _data.canvas = new LGFX_SpriteFx(_data.display);
        _data.canvas->createSprite(_data.display->width(), _data.display->height());

        // Some pc window pop up slower?
        lgfx::delay(1500);

        // // Add key mapping
        auto panel = (lgfx::Panel_sdl*)_data.display->getPanel();
        panel->addKeyCodeMapping(SDLK_q, 0);
        panel->addKeyCodeMapping(SDLK_w, 1);
        panel->addKeyCodeMapping(SDLK_e, 2);
    }

    void canvasUpdate() override
    {
        GetCanvas()->pushSprite(0, 0);
    }

    bool getButton(Gamepad::GamepadButton_t button) override
    {
        if (button == Gamepad::BTN_A) {
            return !lgfx::gpio_in(0);
        } else if (button == Gamepad::BTN_B) {
            return !lgfx::gpio_in(1);
        } else if (button == Gamepad::BTN_C) {
            return !lgfx::gpio_in(2);
        }
        return false;
    }

    float generateRandomNumber(float v1, float v2)
    {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> distribution(v1, v2);
        return distribution(generator);
    }

    /* -------------------------------------------------------------------------- */
    /*                                 Plc relate                                 */
    /* -------------------------------------------------------------------------- */
    std::mutex _plc_daemon_mutex;
    std::array<bool, 5> _plc_relay_state;
    std::array<bool, 9> _plc_input_state;

    void _start_plc_test_daemon()
    {
        std::thread plc_fake_data([this]() {
            while (1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                int random_index               = generateRandomNumber(0, 8);
                _plc_input_state[random_index] = !_plc_input_state[random_index];

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                random_index                   = generateRandomNumber(0, 5);
                _plc_relay_state[random_index] = !_plc_relay_state[random_index];
            }
        });
        plc_fake_data.detach();
    }

    bool getPlcInput(const uint8_t& channel) override
    {
        bool ret = false;
        _plc_daemon_mutex.lock();
        ret = _plc_input_state[channel];
        _plc_daemon_mutex.unlock();
        return ret;
    }

    bool getPlcRelayState(const uint8_t& channel) override
    {
        bool ret = false;
        _plc_daemon_mutex.lock();
        ret = _plc_relay_state[channel];
        _plc_daemon_mutex.unlock();
        return ret;
    }

    bool time_to_go = false;

    void createLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType) override
    {
        /* ----------------------------------- i2c ---------------------------------- */
        if (monitorType == LogMonitor::I2C) {
            std::thread daemon([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));

                time_to_go     = false;
                int time_count = 114514;

                SharedData::BorrowData();
                SharedData::Console().clear();
                SharedData::Console().log("> Scanning PORT-A for I2C Device");
                SharedData::Console().log("> Waiting .. ");
                SharedData::ReturnData();

                while (1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    time_count++;

                    if (time_count > 1000 / 100) {
                        time_count = 0;
                    }

                    // Check quit
                    _plc_daemon_mutex.lock();
                    if (time_to_go) {
                        break;
                    }
                    _plc_daemon_mutex.unlock();
                }
                _plc_daemon_mutex.unlock();
                spdlog::info("i2c bye");
            });
            daemon.detach();
        }

        /* ---------------------------------- uart ---------------------------------- */
        else if (monitorType == LogMonitor::UART) {
            std::thread daemon([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));

                time_to_go     = false;
                int time_count = 0;

                SharedData::BorrowData();
                SharedData::Console().clear();
                SharedData::Console().log("> Waiting uart msg from PORT-C\n> Baudrate: 115200 ");
                SharedData::ReturnData();

                while (1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    time_count++;

                    if (time_count > 1000 / 100) {
                        time_count = 0;

                        SharedData::BorrowData();
                        SharedData::Console().log("> Get msg: {}", "hi :)");
                        SharedData::ReturnData();
                    }

                    // Check quit
                    _plc_daemon_mutex.lock();
                    if (time_to_go) {
                        break;
                    }
                    _plc_daemon_mutex.unlock();
                }
                _plc_daemon_mutex.unlock();
                spdlog::info("uart bye");
            });
            daemon.detach();
        }

        /* --------------------------------- modbus --------------------------------- */
        else if (monitorType == LogMonitor::Modbus) {
            std::thread daemon([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                time_to_go     = false;
                int time_count = 0;

                SharedData::BorrowData();
                SharedData::Console().clear();
                SharedData::Console().log("> Listening PORT-RS485\n> Baudrate: 115200 ");
                SharedData::ReturnData();

                while (1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    time_count++;

                    if (time_count > 2000 / 100) {
                        time_count = 0;

                        SharedData::BorrowData();
                        SharedData::Console().log("> Get msg: H-{} addr: {} size: {}", "W", 0, 8);
                        SharedData::ReturnData();
                    }

                    // Check quit
                    _plc_daemon_mutex.lock();
                    if (time_to_go) {
                        break;
                    }
                    _plc_daemon_mutex.unlock();
                }
                _plc_daemon_mutex.unlock();
                spdlog::info("modbus bye");
            });
            daemon.detach();
        }
    }

    void destroyLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType) override
    {
        _plc_daemon_mutex.lock();
        time_to_go = true;
        _plc_daemon_mutex.unlock();
    }

    bool _is_timer_relay_running = false;

    bool startTimerRelay(TimerRelay::TimerRelayConfig_t config) override
    {
        spdlog::info("try start timer relay with: id: {} on: {} off: {}", config.relayId, config.onInterval,
                     config.offInterval);
        _is_timer_relay_running = true;

        std::thread daemon([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            time_to_go = false;
            bool state = false;
            while (1) {
                state               = !state;
                _plc_relay_state[0] = state;
                spdlog::info("??? {}", state);

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                // Check quit
                _plc_daemon_mutex.lock();
                if (time_to_go) {
                    break;
                }
                _plc_daemon_mutex.unlock();
            }
            _plc_daemon_mutex.unlock();
        });
        daemon.detach();

        return false;
    }

    bool isTimerRelayRunning() override
    {
        return _is_timer_relay_running;
    }

    bool stopTimerRelay() override
    {
        _plc_daemon_mutex.lock();
        time_to_go = true;
        _plc_daemon_mutex.unlock();

        spdlog::info("try stop timer relay");
        _is_timer_relay_running = false;
        return false;
    }

    bool _is_trigger_relay_running = false;
    bool startTriggerRelay(TriggerRelay::Config_t config) override
    {
        _is_trigger_relay_running = true;
        return true;
    }

    bool isTriggerRelayRunning() override
    {
        return _is_trigger_relay_running;
    }

    bool stopTriggerRelay() override
    {
        _is_trigger_relay_running = false;
        return true;
    }

private:
    int _screenWidth;
    int _screenHeight;
};
