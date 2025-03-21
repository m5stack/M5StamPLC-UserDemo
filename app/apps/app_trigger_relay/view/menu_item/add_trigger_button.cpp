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
#include "apps/utils/system/inputs/button/button.h"
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
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 22;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xBFEDE4;
static uint32_t _color_text           = 0x412110;

void MenuItem_AddTriggerButton::onInit()
{
}

void MenuItem_AddTriggerButton::onPopOut()
{
    constexpr int delay = 40;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void MenuItem_AddTriggerButton::onHide()
{
}

void MenuItem_AddTriggerButton::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    // Label
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_color_text);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString("+", frame.x + _panel_w / 2 - 1, frame.y + _panel_h / 2 + 1);
}

void MenuItem_AddTriggerButton::onUpdate()
{
}

void MenuItem_AddTriggerButton::onSelect()
{
    TriggerRelay::Trigger_t new_trigger;
    TriggerConfigMenu::TriggerConfigMenu::CreateTriggerConfigMenu(&new_trigger);
    _config->trigger_list.push_back(new_trigger);

    unSelect();
}

void MenuItem_AddTriggerButton::onUnSelect()
{
}
