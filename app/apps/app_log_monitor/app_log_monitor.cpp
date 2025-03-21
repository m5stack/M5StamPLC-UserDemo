/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_log_monitor.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"
#include <smooth_ui_toolkit.h>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

// App name
const char* AppLogMonitor_Packer::getAppName()
{
    return "LogMonitor";
}

// Theme color
constexpr static uint32_t _theme_color = 0xFFAAAA;
void* AppLogMonitor_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppLogMonitor_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_log_monitor;
}

void AppLogMonitor::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Add options
    std::vector<VIEW::MenuBar::AppOptionProps_t> option_list = {
        {0xFFAAAA, BlendColorInDifference(0xFFAAAA, 0xB5B5B5), "I2C", new VIEW::MonitorPanel_I2C},
        {0xBED2F4, BlendColorInDifference(0xBED2F4, 0xB5B5B5), "UART", new VIEW::MonitorPanel_UART},
        {0xF3FFD5, BlendColorInDifference(0xF3FFD5, 0xB5B5B5), "Modbus", new VIEW::MonitorPanel_Modbus},
        {0xFFDCB0, BlendColorInDifference(0xFFDCB0, 0xB5B5B5), "CAN", new VIEW::MonitorPanel_CAN},
        {0xB4DCBB, BlendColorInDifference(0xB4DCBB, 0xB5B5B5), "SENS", new VIEW::MonitorPanel_SENS},
    };
    for (auto i : option_list) {
        _data.menu_bar.addAppOption(i);
    }

    _data.menu_bar.init();
}

void AppLogMonitor::onRunning()
{
    _data.menu_bar.update(HAL::Millis());

    if (Button::Back()->wasClicked()) {
        destroyApp();
    }
}

void AppLogMonitor::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
}
