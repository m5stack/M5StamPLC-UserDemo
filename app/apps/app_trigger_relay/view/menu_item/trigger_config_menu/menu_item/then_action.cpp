/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../trigger_config_menu.h"
#include <cstdint>
#include <mooncake.h>
#include <string>
#include <hal/hal.h>
#include <assets/assets.h>
#include "../../../../../utils/system/system.h"
#include "core/easing_path/easing_path.h"
#include "hal/utils/trigger_relay/trigger_relay.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace VIEW::TriggerRelayView::TriggerConfigMenu;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 8;
static constexpr int _panel_y         = 51;
static constexpr int _panel_w         = 224;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xBFEDE4;
static uint32_t _color_text           = 0x412110;

void MenuItem_ThenAction::onInit()
{
}

void MenuItem_ThenAction::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void MenuItem_ThenAction::onHide()
{
}

void MenuItem_ThenAction::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_color_text);

    HAL::GetCanvas()->setTextDatum(middle_left);
    HAL::GetCanvas()->drawString("Then:", frame.x + 8, frame.y + _panel_h / 2 - 1);

    HAL::GetCanvas()->setTextDatum(middle_right);
    HAL::GetCanvas()->drawString(TriggerRelay::GetActionDescription(_trigger->onTrueAction), frame.x + _panel_w - 8,
                                 frame.y + _panel_h / 2 - 1);
}

void MenuItem_ThenAction::onUpdate()
{
}

void MenuItem_ThenAction::onSelect()
{
    // Create select menu
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = _color_panel;
        props.onPrimary                = _color_text;
        props.title                    = "Then:";
        props.onCustomRenderBackground = []() { TriggerConfigMenu::RenderConfigPanel(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            _trigger->onTrueAction = TriggerRelay::GetActionList()[selectedIndex];
        };

        // Option list
        props.optionList.resize(TriggerRelay::GetActionList().size());
        for (int i = 0; i < TriggerRelay::GetActionList().size(); i++) {
            props.optionList[i] = TriggerRelay::GetActionDescription(TriggerRelay::GetActionList()[i]);
        }

        // Startup index
        for (int i = 0; i < TriggerRelay::GetActionList().size(); i++) {
            if (_trigger->onTrueAction == TriggerRelay::GetActionList()[i]) {
                props.startupIndex = i;
                break;
            }
        }
    });

    unSelect();
}

void MenuItem_ThenAction::onUnSelect()
{
}
