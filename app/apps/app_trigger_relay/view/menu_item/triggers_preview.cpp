/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../trigger_relay_config_menu.h"
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
#include "trigger_config_menu/trigger_config_menu.h"

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace VIEW::TriggerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 8;
static constexpr int _panel_y         = 8;
static constexpr int _panel_w         = 224;
static constexpr int _panel_h         = 80;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xBFEDE4;
static uint32_t _color_text           = 0x412110;

void MenuItem_TriggersPreview::onInit()
{
}

void MenuItem_TriggersPreview::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void MenuItem_TriggersPreview::onHide()
{
}

static constexpr int _description_mt = 9;
static constexpr int _description_ml = 10;

void MenuItem_TriggersPreview::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextDatum(top_left);
    if (_config->trigger_list.empty()) {
        HAL::GetCanvas()->drawString("No trigger.", frame.x + _description_ml, frame.y + _description_mt);
    } else {
        for (int i = 0; i < 3; i++) {
            if (i >= _config->trigger_list.size()) {
                break;
            }

            _string_buffer = fmt::format("{}. If {} ...", i + 1,
                                         TriggerRelay::GetConditionDescription(_config->trigger_list[i].condition));

            HAL::GetCanvas()->drawString(_string_buffer.c_str(), frame.x + _description_ml,
                                         frame.y + _description_mt + i * 20);
        }
    }
}

void MenuItem_TriggersPreview::onUpdate()
{
}

void MenuItem_TriggersPreview::onSelect()
{
    if (_config->trigger_list.empty()) {
        unSelect();
        return;
    }

    while (1) {
        // Create a menu to show current triggers
        auto trigger_index = SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
            props.primary                  = _color_panel;
            props.onPrimary                = _color_text;
            props.title                    = "Triggers";
            props.onCustomRenderBackground = []() { TirrgerRelayConfigMenu::RenderConfigPanel(); };

            // Option list
            props.optionList.resize(_config->trigger_list.size());
            for (int i = 0; i < _config->trigger_list.size(); i++) {
                _string_buffer      = fmt::format("{}. If {} ...", i + 1,
                                             TriggerRelay::GetConditionDescription(_config->trigger_list[i].condition));
                props.optionList[i] = _string_buffer;
            }
        });

        if (trigger_index < 0) {
            break;
        }

        // Create a menu to show what to do with selected trigger
        auto action_index = SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
            props.primary                  = _color_panel;
            props.onPrimary                = _color_text;
            props.title                    = fmt::format("Trigger: {}", trigger_index + 1);
            props.onCustomRenderBackground = []() { TirrgerRelayConfigMenu::RenderConfigPanel(); };

            // Option list
            props.optionList.push_back("Edit");
            props.optionList.push_back("Delete");
        });

        if (action_index == 0) {
            TriggerConfigMenu::TriggerConfigMenu::CreateTriggerConfigMenu(&_config->trigger_list[trigger_index]);
        } else if (action_index == 1) {
            _config->trigger_list.erase(_config->trigger_list.begin() + trigger_index);
            if (_config->trigger_list.empty()) {
                break;
            }
        }
    }

    unSelect();
}

void MenuItem_TriggersPreview::onUnSelect()
{
    HAL::GetCanvas()->setFont(&fonts::efontCN_16);
}
