/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_template.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// App name
const char* AppTemplate_Packer::getAppName()
{
    return "AppTemplate";
}

// Theme color
constexpr static uint32_t _theme_color = 0x3D7AF5;
void* AppTemplate_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppTemplate_Packer::getAppIcon()
{
    return nullptr;
}

// Like setup()...
void AppTemplate::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::CanvasUpdate();
}

// Like loop()...
void AppTemplate::onRunning()
{
    // Print hi in every 1s
    if (HAL::Millis() - _data.time_count > 1000) {
        spdlog::info("Hi");
        _data.time_count = HAL::Millis();
    }

    // Update button state
    Button::Update();
    if (Button::Back()->wasClicked()) destroyApp();

    // ...
    // If you want to block here, feed the dog manually
    // HAL::FeedTheDog();
}

void AppTemplate::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
