/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <smooth_ui_toolkit.h>

/* -------------------------------------------------------------------------- */
/*                               Gamepad button                               */
/* -------------------------------------------------------------------------- */
namespace Gamepad {
enum GamepadButton_t {
    BTN_START = 0,
    BTN_SELECT,
    BTN_UP,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_DOWN,
    BTN_X,
    BTN_Y,
    BTN_A,
    BTN_B,
    BTN_C,
    BTN_LEFT_STICK,
    GAMEPAD_BUTTON_NUM,
};
}  // namespace Gamepad

/* -------------------------------------------------------------------------- */
/*                                     IMU                                    */
/* -------------------------------------------------------------------------- */
namespace IMU {
struct ImuData_t {
    float accelX;
    float accelY;
    float accelZ;
};
}  // namespace IMU

/* -------------------------------------------------------------------------- */
/*                                  Touchpad                                  */
/* -------------------------------------------------------------------------- */
namespace Touch {
/**
 * @brief Touch point
 *
 */
struct Point_t {
    int x;
    int y;
};
}  // namespace Touch

/* -------------------------------------------------------------------------- */
/*                                 Locale code                                */
/* -------------------------------------------------------------------------- */
enum LocaleCode_t {
    locale_code_en = 0,
    locale_code_cn,
    locale_code_jp,
};

/* -------------------------------------------------------------------------- */
/*                                System config                               */
/* -------------------------------------------------------------------------- */
namespace Config {
// Default config
struct SystemConfig_t {
    uint8_t brightness         = 255;
    uint8_t modbusSlaveAddress = 1;
    int8_t ntpTimeZone         = 8;
    bool beepOn                = true;
    std::string wifiSsid;
    std::string wifiPassword;
    LocaleCode_t localeCode = locale_code_en;
};
}  // namespace Config

/* -------------------------------------------------------------------------- */
/*                                  OTA info                                  */
/* -------------------------------------------------------------------------- */
namespace OtaUpgrade {
struct OtaInfo_t {
    bool upgradeAvailable = false;
    bool getInfoFailed    = false;
    std::string latestVersion;
    std::string firmwareUrl;
};
};  // namespace OtaUpgrade

/* -------------------------------------------------------------------------- */
/*                                 Log monitor                                */
/* -------------------------------------------------------------------------- */
namespace LogMonitor {
enum LogMonitorType_t {
    I2C = 0,
    UART,
    Modbus,
    CAN,
    SENS,
};
};

/* -------------------------------------------------------------------------- */
/*                                 Timer relay                                */
/* -------------------------------------------------------------------------- */
namespace TimerRelay {
struct TimerRelayConfig_t {
    uint8_t relayId      = 0;
    uint32_t onInterval  = 1000;
    uint32_t offInterval = 1000;
};
}  // namespace TimerRelay

/* -------------------------------------------------------------------------- */
/*                                    MISC                                    */
/* -------------------------------------------------------------------------- */
typedef std::function<void(const std::string& log, bool pushScreen, bool clear)> OnLogPageRenderCallback_t;

/* -------------------------------------------------------------------------- */
/*                                  NVS keys                                  */
/* -------------------------------------------------------------------------- */
#define NVS_KEY_APP_HISTORY "app_history"
#define NVS_KEY_BOOT_COUNT  "boot_count"

/* -------------------------------------------------------------------------- */
/*                                   SD card                                  */
/* -------------------------------------------------------------------------- */
#define SD_CARD_MOUNT_POINT "/sdcard"
