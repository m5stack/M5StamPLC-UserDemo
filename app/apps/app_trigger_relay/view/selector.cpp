/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "apps/utils/system/inputs/button/button.h"
#include "trigger_relay_config_menu.h"
#include <cstdint>
#include <mooncake.h>
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "core/easing_path/easing_path.h"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;
using namespace VIEW::TriggerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_r         = 12;
static constexpr int _stroke_width    = 2;
static uint32_t _color_panel          = 0x412110;

void TirrgerRelayConfigMenuSelector::onInit()
{
    moveInloop(true);
}

void TirrgerRelayConfigMenuSelector::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);

    getTransition().setUpdateCallback([](Transition4D* transition) {
        if (transition->isFinish()) {
            transition->setDelay(0);
            transition->setDuration(150);
            transition->setUpdateCallback(nullptr);
        }
    });

    moveTo(0);
}

void TirrgerRelayConfigMenuSelector::onHide()
{
}

void TirrgerRelayConfigMenuSelector::onRender()
{
    auto frame = getTransition().getValue();
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x - _stroke_width, frame.y - _stroke_width, frame.w + _stroke_width * 2,
                                          frame.h + _stroke_width * 2, _panel_r, _color_panel);
}

void TirrgerRelayConfigMenuSelector::onUpdate()
{
    if (isHidden()) {
        return;
    }

    if (Button::Next()->wasClicked()) {
        goNext();
    } else if (Button::Next()->wasHold()) {
        goLast();
    }

    if (Button::Ok()->wasClicked()) {
        selectOption();
        // Clear
        Button::Update();
    }
}
