/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <hal/hal.h>
#include "hal_config.h"

class HAL_StamPLC : public HAL {
public:
    std::string type() override
    {
        return "StamPLC";
    }

    inline void init() override
    {
        _watch_dog_init();
        _modbus_init();
        _can_init();
        _fs_init();
        _speaker_init();
        _disp_init();
        _sd_card_init();
        _i2c_init();
        _rtc_init();
        _io_expander_a_init();
        _io_expander_b_init();
        _data.display->setBrightness(255);
        _gamepad_init();
        _status_light_init();
        setStatusLight(0, 0, 1);
        _thermal_sense_init();
        _power_monitor_init();
        // Disable all interrupts
        _io_expander_a_disable_irq();
        _io_expander_b_disable_irq();
        ezdata_init();
    }

    /* -------------------------------------------------------------------------- */
    /*                             Public api override                            */
    /* -------------------------------------------------------------------------- */
    void feedTheDog() override;
    void reboot() override;
    bool getButton(Gamepad::GamepadButton_t button) override;
    void setStatusLight(const uint8_t& r, const uint8_t& g, const uint8_t& b) override;
    void setStatusLightHex(const std::string& hexColor) override;
    std::string getStatusLightHex() override;
    void beep(float frequency, uint32_t duration) override;
    void beepStop() override;
    void popFatalError(std::string msg) override;

    void loadSystemConfig() override;
    void saveSystemConfig() override;
    void factoryReset(OnLogPageRenderCallback_t onLogPageRender) override;

    bool getPlcInput(const uint8_t& channel) override;
    bool getPlcRelayState(const uint8_t& channel) override;
    void setPlcRelayState(const uint8_t& channel, const bool& state) override;
    void setPlcAllRelayState(const uint8_t& state) override;

    void updateTemp() override;
    void updatePowerMonitor() override;

    void createLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType) override;
    void destroyLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType) override;

    bool nvsSet(const char* key, const int32_t& value) override;
    int32_t nvsGet(const char* key) override;

    bool isSdCardAvailable(bool printStatus = true) override;

    bool startTimerRelay(TimerRelay::TimerRelayConfig_t config) override;
    bool isTimerRelayRunning() override;
    bool stopTimerRelay() override;

    bool startTriggerRelay(TriggerRelay::Config_t config) override;
    bool isTriggerRelayRunning() override;
    bool stopTriggerRelay() override;

    void setSystemTime(tm dateTime) override;

    bool isWifiConnected() override;
    bool isEzdataConnected() override;
    void startWifiConfigApServer(std::function<void(const std::string&)> onEvent) override;
    std::string getEzdataStatusUrl() override;
    bool clearEzdataMonitoringUser() override;
    bool isEadataTokenValid() override;
    std::string getWifiCurrentStatus() override;

private:
    void _watch_dog_init();
    void _disp_init();
    void _disp_test();
    void _i2c_init();
    void _grove_port_test();
    void _gamepad_init();
    void _status_light_init();
    void _speaker_init();
    void _sd_card_init();

    void _485_init();
    void _485_test();
    void _modbus_init();

    void _can_init();
    void _can_test();

    void _thermal_sense_init();
    void _thermal_sense_intr_test();

    void _power_monitor_init();
    void _power_monitor_intr_test();

    void _rtc_init();
    void _adjust_sys_time();
    void ezdata_init();

    // FS
    void _fs_init();
    std::vector<std::string> _ls(const std::string& path);
    void _config_check_valid();
    std::string _create_config_json();
    void _backup_config_file();
    void _log_out_system_config();

    // NVS
    void _nvs_init();
    void _update_boot_count();
    bool _nvs_reset();
    std::string _nvs_get_string(const char* nameSpace, const char* key);

    // IO expander a
    void _io_expander_a_init();
    void _io_expander_a_intr_test();
    void _io_expander_a_disable_irq();

    // IO expander b
    void _io_expander_b_init();
    void _io_expander_b_test();
    void _io_expander_b_intr_test();
    void _io_expander_b_disable_irq();

    // Log monitor
    void _create_i2c_lm_daemon();
    void _destroy_i2c_lm_daemon();
    void _create_uart_lm_daemon();
    void _destroy_uart_lm_daemon();
    void _create_can_lm_daemon();
    void _destroy_can_lm_daemon();
};
