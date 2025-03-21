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
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace VIEW::TriggerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 38;
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 30;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xBFEDE4;
// static uint32_t _color_text = 0x412110;
static constexpr int _icon_mt = 7;
static constexpr int _icon_ml = 6;

void MenuItem_SaveConfigButton::onInit()
{
}

void MenuItem_SaveConfigButton::onPopOut()
{
    constexpr int delay = 80;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
}

void MenuItem_SaveConfigButton::onHide()
{
}

void MenuItem_SaveConfigButton::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    HAL::GetCanvas()->pushImage(frame.x + _icon_ml, frame.y + _icon_mt, 18, 18,
                                AssetPool::GetImage().AppTriggerRelay.icon_save);
}

void MenuItem_SaveConfigButton::onUpdate()
{
}

void MenuItem_SaveConfigButton::onSelect()
{
    // Check size
    if (_config->trigger_list.empty()) {
        HAL::PopWarning("Save trigger config failed:\nEmpty Trigger");
    }

    // Save
    else {
        auto ret = TriggerRelay::SaveConfigToFs(*_config);
        if (ret == "") {
            HAL::PopWarning("Save trigger config failed");
        } else {
            size_t pos = ret.rfind('/');
            if (pos != std::string::npos) {
                ret.insert(pos + 1, "\n");
            }

            ret = fmt::format("Save trigger config at:\n{}", ret);
            HAL::PopSuccess(ret);
        }
    }

    unSelect();
}

void MenuItem_SaveConfigButton::onUnSelect()
{
}
