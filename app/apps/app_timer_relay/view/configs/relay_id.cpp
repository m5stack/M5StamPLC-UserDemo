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
using namespace SYSTEM::UI;
using namespace VIEW::TimerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 8;
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 104;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xE9B685;
static uint32_t _color_text           = 0x4A2705;

void ConfigRelayId::onInit()
{
}

void ConfigRelayId::onPopOut()
{
    constexpr int delay = 80;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void ConfigRelayId::onHide()
{
}

void ConfigRelayId::onRender()
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
    HAL::GetCanvas()->drawString("Relay:", frame.x + 10, frame.y + _panel_h / 2 - 1);

    // Value
    HAL::GetCanvas()->setTextDatum(middle_right);
    _string_buffer = fmt::format("{}", _config->relayId + 1);
    HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _panel_w - 10, frame.y + _panel_h / 2 - 1);
}

void ConfigRelayId::onUpdate()
{
}

static std::vector<std::uint32_t> _relay_id_option_list = {0, 1, 2, 3};

void ConfigRelayId::onSelect()
{
    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = _color_panel;
        props.onPrimary                = _color_text;
        props.title                    = "Relay ID:";
        props.onCustomRenderBackground = []() { ConfigPanel::RenderConfigPanel(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            _config->relayId = _relay_id_option_list[selectedIndex];
        };

        // Option list
        props.optionList.resize(_relay_id_option_list.size());
        for (int i = 0; i < _relay_id_option_list.size(); i++) {
            _string_buffer      = fmt::format("{}", _relay_id_option_list[i] + 1);
            props.optionList[i] = _string_buffer;
        }

        // Startup index
        for (int i = 0; i < _relay_id_option_list.size(); i++) {
            if (_config->relayId == _relay_id_option_list[i]) {
                props.startupIndex = i;
                break;
            }
        }
    });

    unSelect();
}

void ConfigRelayId::onUnSelect()
{
}
