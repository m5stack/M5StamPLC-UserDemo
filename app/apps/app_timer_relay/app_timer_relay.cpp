/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_timer_relay.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// App name
const char* AppTimerRelay_Packer::getAppName()
{
    return "TimerRelay";
}

// Theme color
constexpr static uint32_t _theme_color = 0xFFDCB0;
void* AppTimerRelay_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppTimerRelay_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_timer_relay;
}

// Like setup()...
void AppTimerRelay::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::CanvasUpdate();

    _data.config_panel.init();
    _data.config_panel.popOut();
}

// Like loop()...
void AppTimerRelay::onRunning()
{
    _data.config_panel.update(HAL::Millis());
    HAL::CanvasUpdate();

    if (_data.config_panel.want2Quit()) {
        destroyApp();
    }
}

void AppTimerRelay::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    HAL::StopTimerRelay();
}
