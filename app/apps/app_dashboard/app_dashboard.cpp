/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_dashboard.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// App name
const char* AppDashboard_Packer::getAppName()
{
    return "Dashboard";
}

// Theme color
constexpr static uint32_t _theme_color = 0xF3FFD5;
void* AppDashboard_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppDashboard_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_dashboard;
}

void AppDashboard::onResume()
{
    spdlog::info("{} onResume", getAppName());

    _data.widget_desktop.init();
    _data.widget_desktop.popOut();
}

void AppDashboard::onRunning()
{
    _data.widget_desktop.update(HAL::Millis());
    Button::Update();
    if (Button::Back()->wasClicked()) {
        destroyApp();
    }
}

void AppDashboard::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
}
