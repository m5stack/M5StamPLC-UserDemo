/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_setting.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"
#include "../../shared/types.h"
#include "spdlog/spdlog.h"
#include "view/setting_page.h"
#include <memory>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;

static const std::vector<std::string> _option_list = {"Modbus Slave Address", "NTP Time Zone", "Buzzer",
                                                      "Firmware Version: " + std::string(APP_VERSION)};
static std::unique_ptr<SettingPage> _page;
static int _hit_count = 0;
void start_arkanoid();

// App name
const char* AppSetting_Packer::getAppName()
{
    return "Setting";
}

// Theme color
constexpr static uint32_t _theme_color = 0xB4DCBB;
void* AppSetting_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppSetting_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_setting;
}

// Like setup()...
void AppSetting::onResume()
{
    spdlog::info("{} onResume", getAppName());

    HAL::GetCanvas()->fillScreen(_theme_color);

    _hit_count = 0;
    _page      = std::make_unique<SettingPage>();
    _page->init(_option_list, 0);
    _page->onOptionPressed = [](int optionIndex) {
        if (optionIndex == 3) {
            _hit_count++;
            if (_hit_count >= 6) {
                _hit_count = 0;
                start_arkanoid();
            }
        }
    };
}

// Like loop()...
void AppSetting::onRunning()
{
    _page->update(HAL::Millis());

    if (_page->isQuiting()) {
        destroyApp();
    }

    else if (_page->was_selected) {
        _page->was_selected = false;
        auto selected_index = _page->getSelectedOptionIndex();
        // spdlog::info("selected: {}", selected_index);

        if (selected_index == 0) {
            handle_setting_modbus_slave_addr();
        } else if (selected_index == 1) {
            handle_setting_ntp_time_zone();
        } else if (selected_index == 2) {
            handle_setting_buzzer_on_off();
        }
    }
}

void AppSetting::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    _page.reset();
}

void AppSetting::RenderSettingPage()
{
    if (_page) {
        _page->stop_input_reading = true;
        _page->push_canvas        = false;
        _page->update(HAL::Millis());
        _page->stop_input_reading = false;
        _page->push_canvas        = true;
    }
}

void AppSetting::handle_setting_modbus_slave_addr()
{
    auto new_addr = HAL::GetSystemConfig().modbusSlaveAddress;

    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = 0xB4DCBB;
        props.onPrimary                = 0x012706;
        props.title                    = "Slave Addr:";
        props.onCustomRenderBackground = []() { AppSetting::RenderSettingPage(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            new_addr = selectedIndex + 1;
        };

        // Option list
        uint8_t max_addr = 128;
        props.optionList.resize(max_addr);
        for (int i = 0; i < max_addr; i++) {
            props.optionList[i] = fmt::format("{}", i + 1);
        }

        props.startupIndex = HAL::GetSystemConfig().modbusSlaveAddress - 1;
        if (props.startupIndex >= max_addr) {
            props.startupIndex = 0;
        }
    });

    if (new_addr != HAL::GetSystemConfig().modbusSlaveAddress) {
        HAL::GetSystemConfig().modbusSlaveAddress = new_addr;
        spdlog::info("change modbus addr to {}", HAL::GetSystemConfig().modbusSlaveAddress);
        HAL::SaveSystemConfig();
    }
}

void AppSetting::handle_setting_ntp_time_zone()
{
    auto new_time_zone = HAL::GetSystemConfig().ntpTimeZone;

    std::vector<int8_t> time_zone_list;
    for (int i = -12; i < 15; i++) {
        time_zone_list.push_back(static_cast<int8_t>(i));
    }

    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = 0xB4DCBB;
        props.onPrimary                = 0x012706;
        props.title                    = "NTP Time Zone:";
        props.onCustomRenderBackground = []() { AppSetting::RenderSettingPage(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            new_time_zone = time_zone_list[selectedIndex];
        };

        // Option list
        props.optionList.resize(time_zone_list.size());
        for (int i = 0; i < time_zone_list.size(); i++) {
            props.optionList[i] = fmt::format("{}", time_zone_list[i]);
        }

        for (int i = 0; i < time_zone_list.size(); i++) {
            if (time_zone_list[i] == HAL::GetSystemConfig().ntpTimeZone) {
                props.startupIndex = i;
                break;
            }
        }
    });

    if (new_time_zone != HAL::GetSystemConfig().ntpTimeZone) {
        HAL::GetSystemConfig().ntpTimeZone = new_time_zone;
        spdlog::info("change ntp time zone to {}", HAL::GetSystemConfig().ntpTimeZone);
        HAL::SaveSystemConfig();
    }
}

void AppSetting::handle_setting_buzzer_on_off()
{
    auto new_buzzer_onoff = HAL::GetSystemConfig().beepOn;

    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = 0xB4DCBB;
        props.onPrimary                = 0x012706;
        props.title                    = "Buzzer:";
        props.onCustomRenderBackground = []() { AppSetting::RenderSettingPage(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            new_buzzer_onoff = selectedIndex == 0;
        };

        // Option list
        props.optionList.push_back("ON");
        props.optionList.push_back("OFF");

        props.startupIndex = HAL::GetSystemConfig().beepOn ? 0 : 1;
    });

    if (new_buzzer_onoff != HAL::GetSystemConfig().beepOn) {
        HAL::GetSystemConfig().beepOn = new_buzzer_onoff;
        spdlog::info("change beep on to {}", HAL::GetSystemConfig().beepOn);
        HAL::SaveSystemConfig();
    }
}

void AppSetting::handle_page_about()
{
}
