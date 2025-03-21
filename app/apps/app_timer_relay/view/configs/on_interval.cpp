/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../config_panel.h"
#include <cstdint>
#include <mooncake.h>
#include <string>
#include "../../../../hal/hal.h"
#include "../../../../assets/assets.h"
#include "../../../utils/system/system.h"
#include "core/easing_path/easing_path.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace VIEW::TimerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 8;
static constexpr int _panel_y         = 8;
static constexpr int _panel_w         = 225;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xE9B685;
static uint32_t _color_text           = 0x4A2705;

void ConfigOnInterval::onInit()
{
}

void ConfigOnInterval::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void ConfigOnInterval::onHide()
{
}

void ConfigOnInterval::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    // Label
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_color_text);
    HAL::GetCanvas()->setTextDatum(middle_left);
    HAL::GetCanvas()->drawString("ON Interval:", frame.x + 10, frame.y + _panel_h / 2 - 1);

    // Value
    HAL::GetCanvas()->setTextDatum(middle_right);
    if (_config->onInterval >= (1000 * 10 * 6)) {
        _string_buffer = fmt::format("{}min", _config->onInterval / (1000 * 10 * 6));
    } else if (_config->onInterval >= 1000) {
        _string_buffer = fmt::format("{}s", _config->onInterval / 1000);
    } else {
        _string_buffer = fmt::format("{:.1f}s", (float)_config->onInterval / 1000);
    }
    HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _panel_w - 10, frame.y + _panel_h / 2 - 1);
}

void ConfigOnInterval::onUpdate()
{
}

static std::vector<std::uint32_t> _on_interval_option_list = {100,
                                                              200,
                                                              500,
                                                              800,
                                                              1000,
                                                              1000 * 2,
                                                              1000 * 3,
                                                              1000 * 4,
                                                              1000 * 5,
                                                              1000 * 6,
                                                              1000 * 7,
                                                              1000 * 8,
                                                              1000 * 9,
                                                              1000 * 10,
                                                              1000 * 10 * 2,
                                                              1000 * 10 * 3,
                                                              1000 * 10 * 4,
                                                              1000 * 10 * 5,
                                                              1000 * 10 * 6,
                                                              1000 * 10 * 6 * 2,
                                                              1000 * 10 * 6 * 3,
                                                              1000 * 10 * 6 * 4,
                                                              1000 * 10 * 6 * 5,
                                                              1000 * 10 * 6 * 6,
                                                              1000 * 10 * 6 * 7,
                                                              1000 * 10 * 6 * 8,
                                                              1000 * 10 * 6 * 9,
                                                              1000 * 10 * 6 * 10};

void ConfigOnInterval::onSelect()
{
    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = _color_panel;
        props.onPrimary                = _color_text;
        props.title                    = "ON Interval:";
        props.onCustomRenderBackground = []() { ConfigPanel::RenderConfigPanel(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            _config->onInterval = _on_interval_option_list[selectedIndex];
        };

        // Option list
        props.optionList.resize(_on_interval_option_list.size());
        for (int i = 0; i < _on_interval_option_list.size(); i++) {
            if (_on_interval_option_list[i] >= 1000 * 10 * 6) {
                _string_buffer = fmt::format("{} min", _on_interval_option_list[i] / (1000 * 10 * 6));
            } else if (_on_interval_option_list[i] >= 1000) {
                _string_buffer = fmt::format("{} s", _on_interval_option_list[i] / 1000);
            } else {
                _string_buffer = fmt::format("{:.1f} s", (float)_on_interval_option_list[i] / 1000);
            }
            props.optionList[i] = _string_buffer;
        }

        // Startup index
        for (int i = 0; i < _on_interval_option_list.size(); i++) {
            if (_config->onInterval == _on_interval_option_list[i]) {
                props.startupIndex = i;
                break;
            }
        }
    });

    unSelect();
}

void ConfigOnInterval::onUnSelect()
{
}
