/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../config_panel.h"
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
using namespace VIEW::TimerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 122;
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 69;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xE9B685;
static uint32_t _color_text           = 0x4A2705;
static uint32_t _color_button_running = 0xFF7777;

void ConfigRunButton::onInit()
{
}

void ConfigRunButton::onPopOut()
{
    constexpr int delay = 120;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void ConfigRunButton::onHide()
{
}

void ConfigRunButton::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r,
                                          HAL::IsTimerRelayRunning() ? _color_button_running : _color_panel);

    // Label
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_color_text);
    HAL::GetCanvas()->setTextDatum(middle_center);
    _string_buffer = HAL::IsTimerRelayRunning() ? "STOP" : "RUN";
    HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _panel_w / 2, frame.y + _panel_h / 2 - 1);

    // Relay icon
    _render_relay_icon(frame);
}

void ConfigRunButton::onUpdate()
{
}

void ConfigRunButton::onSelect()
{
    if (HAL::IsTimerRelayRunning()) {
        HAL::StopTimerRelay();
    } else {
        HAL::StartTimerRelay(*_config);
    }

    unSelect();
}

void ConfigRunButton::onUnSelect()
{
}

static constexpr int _relay_icon_x                      = 200;
static constexpr int _relay_icon_y                      = 95;
static constexpr int _relay_icon_ml                     = _relay_icon_x - _panel_x;
static constexpr int _relay_icon_w                      = 33;
static constexpr int _relay_icon_h                      = 33;
static constexpr int _relay_icon_r                      = _relay_icon_w / 2;
static constexpr int _relay_icon_stroke_w               = 2;
static constexpr uint32_t _color_block_input_high       = 0xFF6B6B;
static constexpr uint32_t _color_block_input_low        = 0x29DC97;
static constexpr uint32_t _color_block_input_label_high = 0xC53131;
static constexpr uint32_t _color_block_input_label_low  = 0x11A25D;

void ConfigRunButton::_render_relay_icon(SmoothUIToolKit::Vector4D_t& frame)
{
    if (HAL::Millis() - _relay_state_update_time_count > 100) {
        _relay_state                   = HAL::GetPlcRelayState(_config->relayId);
        _relay_state_update_time_count = HAL::Millis();
        // spdlog::info("relay state: {}", _relay_state);
    }

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x + _relay_icon_ml, frame.y, _relay_icon_w, _relay_icon_h,
                                          _relay_icon_r, _color_panel);
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x + _relay_icon_ml + _relay_icon_stroke_w,
                                          frame.y + _relay_icon_stroke_w, _relay_icon_w - _relay_icon_stroke_w * 2,
                                          _relay_icon_h - _relay_icon_stroke_w * 2,
                                          (_relay_icon_w - _relay_icon_stroke_w * 2) / 2,
                                          _relay_state ? _color_block_input_high : _color_block_input_low);

    // Label
    // AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_relay_state ? _color_block_input_label_high : _color_block_input_label_low);
    HAL::GetCanvas()->setTextDatum(middle_center);
    _string_buffer = fmt::format("{}", _config->relayId + 1);
    HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _relay_icon_ml + _relay_icon_w / 2,
                                 frame.y + _relay_icon_h / 2 - 1);
}
