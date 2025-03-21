/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_trigger_relay.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"
#include "apps/utils/system/inputs/button/button.h"
#include "hal/utils/trigger_relay/trigger_relay.h"
#include "hal/utils/trigger_relay/types.h"
#include "spdlog/spdlog.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// App name
const char* AppTriggerRelay_Packer::getAppName()
{
    return "TriggerRelay";
}

// Theme color
constexpr static uint32_t _theme_color = 0x74D6C5;
void* AppTriggerRelay_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppTriggerRelay_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_trigger_relay;
}

// Like setup()...
void AppTriggerRelay::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::CanvasUpdate();

    _data.config_panel.init();
    _data.config_panel.popOut();
}

// Like loop()...
void AppTriggerRelay::onRunning()
{
    _data.config_panel.update(HAL::Millis());
    HAL::CanvasUpdate();

    if (_data.config_panel.want2Quit()) {
        destroyApp();
    }
}

void AppTriggerRelay::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    HAL::StopTriggerRelay();
}
