/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../trigger_relay_config_menu.h"
#include <cstdint>
#include <mooncake.h>
#include "../../../../hal/hal.h"
#include "../../../../assets/assets.h"
#include "../../../utils/system/system.h"
#include "core/easing_path/easing_path.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace VIEW::TriggerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 114;
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 119;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xB3E9A2;
static uint32_t _color_text           = 0x412110;
static uint32_t _color_button_running = 0xFF7777;

void MenuItem_RunButton::onInit()
{
}

void MenuItem_RunButton::onPopOut()
{
    constexpr int delay = 120;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void MenuItem_RunButton::onHide()
{
}

void MenuItem_RunButton::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r,
                                          HAL::IsTriggerRelayRunning() ? _color_button_running : _color_panel);

    // Label
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_color_text);
    HAL::GetCanvas()->setTextDatum(middle_center);
    _string_buffer = HAL::IsTriggerRelayRunning() ? "STOP" : "RUN";
    HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _panel_w / 2, frame.y + _panel_h / 2 - 1);
}

void MenuItem_RunButton::onUpdate()
{
}

void MenuItem_RunButton::onSelect()
{
    if (HAL::IsTriggerRelayRunning()) {
        HAL::StopTriggerRelay();
    } else {
        HAL::StartTriggerRelay(*_config);
    }

    unSelect();
}

void MenuItem_RunButton::onUnSelect()
{
}
