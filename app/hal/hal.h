/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <M5GFX.h>
#include "utils/lgfx_fx/lgfx_fx.h"
#include "utils/trigger_relay/trigger_relay.h"
#include "types.h"

/**
 * @brief Provide a dingleton to abstract hardware methods
 * 1) Inherit and override methods to create a specific hal
 * 2) Use HAL::Inject() to inject your hal
 * 3) Use HAL:Get() to get this hal wherever you want
 */
class HAL {
public:
    /**
     * @brief Get HAL instance
     *
     * @return HAL*
     */
    static HAL* Get();

    /**
     * @brief Check if HAL is valid
     *
     * @return true
     * @return false
     */
    static bool Check();

    /**
     * @brief HAL injection, init() will be called here
     *
     * @param hal
     * @return true
     * @return false
     */
    static bool Inject(HAL* hal);

    /**
     * @brief Destroy HAL instance
     *
     */
    static void Destroy();

    /**
     * @brief Base class
     *
     */
    HAL()
    {
    }
    virtual ~HAL()
    {
    }

    static std::string Type()
    {
        return Get()->type();
    }
    virtual std::string type()
    {
        return "Base";
    }

    static std::string CompileDate()
    {
        return Get()->compileDate();
    }
    virtual std::string compileDate()
    {
        return __DATE__;
    }

    virtual void init()
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Display                                  */
    /* -------------------------------------------------------------------------- */
    /**
     * @brief Display device
     *
     * @return LGFX_Device*
     */
    static LGFX_Device* GetDisplay()
    {
        return Get()->_data.display;
    }

    /**
     * @brief Full screen canvas (sprite)
     *
     * @return LGFX_SpriteFx*
     */
    static LGFX_SpriteFx* GetCanvas()
    {
        return Get()->_data.canvas;
    }

    /**
     * @brief Push framebuffer
     *
     */
    static void CanvasUpdate()
    {
        Get()->canvasUpdate();
    }
    virtual void canvasUpdate()
    {
        GetCanvas()->pushSprite(0, 0);
    }

    /**
     * @brief Render fps panel
     *
     */
    static void RenderFpsPanel()
    {
        Get()->renderFpsPanel();
    }
    virtual void renderFpsPanel();

    /**
     * @brief Pop error message and wait reboot
     *
     * @param msg
     */
    static void PopFatalError(std::string msg)
    {
        Get()->popFatalError(msg);
    }
    virtual void popFatalError(std::string msg);

    /**
     * @brief Pop warning message and wait continue
     *
     * @param msg
     */
    static void PopWarning(std::string msg)
    {
        Get()->popWarning(msg);
    }
    virtual void popWarning(std::string msg);

    /**
     * @brief Pop error message and wait continue
     *
     * @param msg
     */
    static void PopError(std::string msg)
    {
        Get()->popError(msg);
    }
    virtual void popError(std::string msg);

    /**
     * @brief Pop success message and wait continue
     *
     * @param msg
     */
    static void PopSuccess(std::string msg, bool showSuccessLabel = true)
    {
        Get()->popSuccess(msg, showSuccessLabel);
    }
    virtual void popSuccess(std::string msg, bool showSuccessLabel = true);

    /* -------------------------------------------------------------------------- */
    /*                                   System                                   */
    /* -------------------------------------------------------------------------- */
    /**
     * @brief Delay(ms)
     *
     * @param milliseconds
     */
    static void Delay(unsigned long milliseconds)
    {
        Get()->delay(milliseconds);
    }
    virtual void delay(unsigned long milliseconds)
    {
        lgfx::delay(milliseconds);
    }

    /**
     * @brief Get the number of milliseconds passed since boot
     *
     * @return unsigned long
     */
    static unsigned long Millis()
    {
        return Get()->millis();
    }
    virtual unsigned long millis()
    {
        return lgfx::millis();
    }

    /**
     * @brief Power off
     *
     */
    static void PowerOff()
    {
        Get()->powerOff();
    }
    virtual void powerOff()
    {
    }

    /**
     * @brief Reboot
     *
     */
    static void Reboot()
    {
        Get()->reboot();
    }
    virtual void reboot()
    {
    }

    /**
     * @brief Set RTC time
     *
     * @param dateTime
     */
    static void SetSystemTime(tm dateTime)
    {
        return Get()->setSystemTime(dateTime);
    }
    virtual void setSystemTime(tm dateTime)
    {
    }

    /**
     * @brief Get local time(wrap of localtime())
     *
     * @return tm*
     */
    static tm* GetLocalTime()
    {
        return Get()->getLocalTime();
    }
    virtual tm* getLocalTime();

    /**
     * @brief Reset system watch dog
     *
     */
    static void FeedTheDog()
    {
        Get()->feedTheDog();
    }
    virtual void feedTheDog()
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                System config                               */
    /* -------------------------------------------------------------------------- */
    /**
     * @brief Load system config from fs
     *
     */
    static void LoadSystemConfig()
    {
        Get()->loadSystemConfig();
    }
    virtual void loadSystemConfig()
    {
    }

    /**
     * @brief Save system config to fs
     *
     */
    static void SaveSystemConfig()
    {
        Get()->saveSystemConfig();
    }
    virtual void saveSystemConfig()
    {
    }

    /**
     * @brief Get system config
     *
     * @return Config::SystemConfig_t&
     */
    static Config::SystemConfig_t& GetSystemConfig()
    {
        return Get()->_data.config;
    }

    /**
     * @brief Set system config
     *
     * @param cfg
     */
    static void SetSystemConfig(Config::SystemConfig_t cfg)
    {
        Get()->_data.config = cfg;
    }

    /**
     * @brief Apply system config to device
     *
     */
    static void ApplySystemConfig()
    {
        Get()->applySystemConfig();
    }
    virtual void applySystemConfig()
    {
    }

    /**
     * @brief Factory reset device
     *
     * @param onLogPageRender
     */
    static void FactoryReset(OnLogPageRenderCallback_t onLogPageRender)
    {
        Get()->factoryReset(onLogPageRender);
    }
    virtual void factoryReset(OnLogPageRenderCallback_t onLogPageRender)
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                     NVS                                    */
    /* -------------------------------------------------------------------------- */
    static bool NvsSet(const char* key, const int32_t& value)
    {
        return Get()->nvsSet(key, value);
    }
    virtual bool nvsSet(const char* key, const int32_t& value)
    {
        return false;
    }

    static int32_t NvsGet(const char* key)
    {
        return Get()->nvsGet(key);
    }
    virtual int32_t nvsGet(const char* key)
    {
        return 0;
    }

    /* -------------------------------------------------------------------------- */
    /*                                   SD card                                  */
    /* -------------------------------------------------------------------------- */
    static bool IsSdCardAvailable(bool printStatus = true)
    {
        return Get()->isSdCardAvailable(printStatus);
    }
    virtual bool isSdCardAvailable(bool printStatus = true)
    {
        return true;
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Buzzer                                   */
    /* -------------------------------------------------------------------------- */
    /**
     * @brief Buzzer beep
     *
     * @param frequency
     * @param duration
     */
    static void Beep(float frequency, uint32_t duration = 4294967295U)
    {
        Get()->beep(frequency, duration);
    }
    virtual void beep(float frequency, uint32_t duration)
    {
    }

    /**
     * @brief Stop buzzer beep
     *
     */
    static void BeepStop()
    {
        Get()->beepStop();
    }
    virtual void beepStop()
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Gamepad                                  */
    /* -------------------------------------------------------------------------- */
    /**
     * @brief Get button state, 获取按键状态
     *
     * @param button
     * @return true Pressing, 按下
     * @return false Released, 松开
     */
    static bool GetButton(Gamepad::GamepadButton_t button)
    {
        return Get()->getButton(button);
    }
    virtual bool getButton(Gamepad::GamepadButton_t button)
    {
        return false;
    }

    /**
     * @brief Get any button state, 获取任意按键状态
     *
     * @return true Pressing, 按下
     * @return false Released, 松开
     */
    static bool GetAnyButton(bool waitRelease = false)
    {
        return Get()->getAnyButton(waitRelease);
    }
    virtual bool getAnyButton(bool waitRelease = false);

    /* -------------------------------------------------------------------------- */
    /*                                Status light                                */
    /* -------------------------------------------------------------------------- */
    static void SetStatusLight(const uint8_t& r, const uint8_t& g, const uint8_t& b)
    {
        Get()->setStatusLight(r, g, b);
    }
    virtual void setStatusLight(const uint8_t& r, const uint8_t& g, const uint8_t& b)
    {
    }
    static void SetStatusLightHex(const std::string& hexColor)
    {
        Get()->setStatusLightHex(hexColor);
    }
    virtual void setStatusLightHex(const std::string& hexColor)
    {
    }
    static std::string GetStatusLightHex()
    {
        return Get()->getStatusLightHex();
    }
    virtual std::string getStatusLightHex()
    {
        return "#000000";
    }

    /* -------------------------------------------------------------------------- */
    /*                                   PLC ios                                  */
    /* -------------------------------------------------------------------------- */
    static bool GetPlcInput(const uint8_t& channel)
    {
        return Get()->getPlcInput(channel);
    }
    virtual bool getPlcInput(const uint8_t& channel)
    {
        return false;
    }

    static bool GetPlcRelayState(const uint8_t& channel)
    {
        return Get()->getPlcRelayState(channel);
    }
    virtual bool getPlcRelayState(const uint8_t& channel)
    {
        return false;
    }

    static void SetPlcRelayState(const uint8_t& channel, const bool& state)
    {
        Get()->setPlcRelayState(channel, state);
    }
    virtual void setPlcRelayState(const uint8_t& channel, const bool& state)
    {
    }

    static void SetPlcAllRelayState(const uint8_t& state)
    {
        Get()->setPlcAllRelayState(state);
    }
    virtual void setPlcAllRelayState(const uint8_t& state)
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Thermal                                  */
    /* -------------------------------------------------------------------------- */
    static void UpdateTemp()
    {
        Get()->updateTemp();
    }
    virtual void updateTemp()
    {
    }

    static float GetTemp()
    {
        return Get()->_data.temp;
    }

    /* -------------------------------------------------------------------------- */
    /*                                Power monitor                               */
    /* -------------------------------------------------------------------------- */
    static void UpdatePowerMonitor()
    {
        Get()->updatePowerMonitor();
    }
    virtual void updatePowerMonitor()
    {
    }

    static float GetPowerVoltage()
    {
        return Get()->_data.power_voltage;
    }
    static float GetExpanderOutputCurrent()
    {
        return Get()->_data.expander_output_current;
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Ezdata                                   */
    /* -------------------------------------------------------------------------- */
    static bool IsWifiConnected()
    {
        return Get()->isWifiConnected();
    }
    virtual bool isWifiConnected()
    {
        return false;
    }

    static bool IsEzdataConnected()
    {
        return Get()->isEzdataConnected();
    }
    virtual bool isEzdataConnected()
    {
        return false;
    }

    /* -------------------------------------------------------------------------- */
    /*                                 Log monitor                                */
    /* -------------------------------------------------------------------------- */
    static void CreateLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
    {
        Get()->createLogMonitorDaemon(monitorType);
    }
    virtual void createLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
    {
    }

    static void DestroyLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
    {
        Get()->destroyLogMonitorDaemon(monitorType);
    }
    virtual void destroyLogMonitorDaemon(LogMonitor::LogMonitorType_t monitorType)
    {
    }

    /* -------------------------------------------------------------------------- */
    /*                                 Timer relay                                */
    /* -------------------------------------------------------------------------- */
    static bool StartTimerRelay(TimerRelay::TimerRelayConfig_t config)
    {
        return Get()->startTimerRelay(config);
    }
    virtual bool startTimerRelay(TimerRelay::TimerRelayConfig_t config)
    {
        return false;
    }

    static bool IsTimerRelayRunning()
    {
        return Get()->isTimerRelayRunning();
    }
    virtual bool isTimerRelayRunning()
    {
        return false;
    }

    static bool StopTimerRelay()
    {
        return Get()->stopTimerRelay();
    }
    virtual bool stopTimerRelay()
    {
        return false;
    }

    /* -------------------------------------------------------------------------- */
    /*                                Trigger relay                               */
    /* -------------------------------------------------------------------------- */
    static bool StartTriggerRelay(TriggerRelay::Config_t config)
    {
        return Get()->startTriggerRelay(config);
    }
    virtual bool startTriggerRelay(TriggerRelay::Config_t config)
    {
        return false;
    }

    static bool IsTriggerRelayRunning()
    {
        return Get()->isTriggerRelayRunning();
    }
    virtual bool isTriggerRelayRunning()
    {
        return false;
    }

    static bool StopTriggerRelay()
    {
        return Get()->stopTriggerRelay();
    }
    virtual bool stopTriggerRelay()
    {
        return false;
    }

    /* -------------------------------------------------------------------------- */
    /*                                   Network                                  */
    /* -------------------------------------------------------------------------- */
    static void StartWifiConfigApServer(std::function<void(const std::string&)> onEvent)
    {
        Get()->startWifiConfigApServer(onEvent);
    }
    virtual void startWifiConfigApServer(std::function<void(const std::string&)> onEvent)
    {
    }

    static std::string GetWifiConfigApSsid()
    {
        return Get()->getWifiConfigApSsid();
    }
    std::string getWifiConfigApSsid()
    {
        return "M5StamPLC-WiFi-Config";
    }

    static std::string GetWifiConfigLink()
    {
        return Get()->getWifiConfigLink();
    }
    std::string getWifiConfigLink()
    {
        return "http://192.168.4.1/syscfg";
    }

    static std::string GetEzdataStatusUrl()
    {
        return Get()->getEzdataStatusUrl();
    }
    virtual std::string getEzdataStatusUrl()
    {
        return "https://ezdata-stamplc.m5stack.com/aabbccddeeff00112233445566778899";
    }

    static bool ClearEzdataMonitoringUser()
    {
        return Get()->clearEzdataMonitoringUser();
    }
    virtual bool clearEzdataMonitoringUser()
    {
        return false;
    }

    static bool IsEadataTokenValid()
    {
        return Get()->isEadataTokenValid();
    }
    virtual bool isEadataTokenValid()
    {
        return false;
    }

    static std::string GetWifiCurrentStatus()
    {
        return Get()->getWifiCurrentStatus();
    }
    virtual std::string getWifiCurrentStatus()
    {
        return "";
    }

    /**
     * @brief Components
     *
     */
protected:
    struct Data_t {
        LGFX_Device* display  = nullptr;
        LGFX_SpriteFx* canvas = nullptr;
        time_t time_buffer;
        Config::SystemConfig_t config;
        // IMU::ImuData_t imu_data;

        float temp                    = 0.0f;
        float power_voltage           = 0.0f;
        float expander_output_current = 0.0f;
    };
    Data_t _data;

private:
    static HAL* _hal;
};
