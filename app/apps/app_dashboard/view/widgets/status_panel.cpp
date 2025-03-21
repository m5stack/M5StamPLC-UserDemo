/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "widgets.h"
#include "../../../../assets/assets.h"
#include "../../../../shared/shared.h"
#include <mooncake.h>
#include <smooth_ui_toolkit.h>

using namespace SmoothUIToolKit;

static constexpr int _panel_startup_x = 0 + 2;
static constexpr int _panel_startup_y = 135 + 2;
static constexpr int _panel_startup_w = 79;
static constexpr int _panel_startup_h = 39;

static constexpr int _panel_x = 0 + 2;
static constexpr int _panel_y = 0 + 2;
static constexpr int _panel_w = 79;
static constexpr int _panel_h = 39;
static constexpr int _panel_r = 6;

static constexpr int _inner_pannel_mt = 19;
static constexpr int _inner_pannel_ml = 2;
static constexpr int _inner_pannel_h  = 18;
static constexpr int _inner_pannel_r  = 4;

static uint32_t _color_pannel       = 0x3A3A3A;
static uint32_t _color_inner_pannel = 0xF3FFD5;

void WidgetStatusPanel::onInit()
{
}

void WidgetStatusPanel::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().setEachDelay(delay + 70, delay, delay + 70, delay + 70);
    getTransition().setDuration(400);
    getTransition().setTransitionPath(EasingPath::easeOutBack);
}

void WidgetStatusPanel::onHide()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().setEachDelay(delay, delay + 70, delay, delay);
}

void WidgetStatusPanel::onUpdate()
{
    if (HAL::Millis() - _data.update_time_count > 1000) {
        auto now_tm = HAL::GetLocalTime();
        _data.time  = fmt::format("{:02}:{:02}", now_tm->tm_hour, now_tm->tm_min);
        _data.date  = fmt::format("{:04}.{:02}.{:02}", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday);

        _data.wifi_state   = HAL::IsWifiConnected();
        _data.ezdata_state = HAL::IsEzdataConnected();

        _data.update_time_count = HAL::Millis();
    }
}

void WidgetStatusPanel::onRender()
{
    auto frame = getTransition().getValue();

    // panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_pannel);
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x + _inner_pannel_ml, frame.y + _inner_pannel_mt,
                                          frame.w - _inner_pannel_ml * 2, _inner_pannel_h, _inner_pannel_r,
                                          _color_inner_pannel);

    // Time
    AssetPool::LoadFont12(HAL::GetCanvas());
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->setTextColor(_color_inner_pannel);
    HAL::GetCanvas()->drawString(_data.time.c_str(), frame.x + 21, frame.y + 9);

    // Date
    HAL::GetCanvas()->setTextColor(_color_pannel);
    HAL::GetCanvas()->drawString(_data.date.c_str(), frame.x + _panel_w / 2, frame.y + 28);

    // Wifi
    HAL::GetCanvas()->pushImage(frame.x + 43, frame.y + _inner_pannel_ml, 15, 15,
                                _data.wifi_state ? AssetPool::GetImage().AppDashboard.icon_wifi_ok
                                                 : AssetPool::GetImage().AppDashboard.icon_wifi_not_ok);
    // Cloud
    HAL::GetCanvas()->pushImage(frame.x + 60, frame.y + _inner_pannel_ml, 15, 15,
                                _data.ezdata_state ? AssetPool::GetImage().AppDashboard.icon_ezdata_ok
                                                   : AssetPool::GetImage().AppDashboard.icon_ezdata_not_ok);
}
