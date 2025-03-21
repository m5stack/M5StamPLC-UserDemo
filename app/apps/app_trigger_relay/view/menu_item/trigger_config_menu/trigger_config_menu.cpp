/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "trigger_config_menu.h"
#include <cstring>
#include <mooncake.h>
#include "../../../../../hal/hal.h"
#include "../../../../../assets/assets.h"
#include "../../../../utils/system/system.h"
#include "apps/utils/system/inputs/button/button.h"
#include "spdlog/spdlog.h"

using namespace SmoothUIToolKit::Widgets;
using namespace VIEW::TriggerRelayView::TriggerConfigMenu;
using namespace SYSTEM::INPUTS;

static TriggerConfigMenu* _config_panel_instance = nullptr;

void TriggerConfigMenu::onInit()
{
    _config_panel_instance = this;

    // Add configs
    addChild(new MenuItem_IfCondition(_trigger));
    addChild(new MenuItem_ThenAction(_trigger));
    addChild(new MenuItem_ElseAction(_trigger));

    _data.selector = new TirrgerRelayConfigMenuSelector;
    _data.selector->init();
    _data.selector->enter(this);
    _data.selector->setRenderOnUpdate(false);
}

void TriggerConfigMenu::RenderConfigPanel()
{
    if (_config_panel_instance == nullptr) {
        return;
    }
    _config_panel_instance->render();
}

void TriggerConfigMenu::CreateTriggerConfigMenu(TriggerRelay::Trigger_t* trigger)
{
    spdlog::info("create trigger config menu");

    auto trigger_config_menu = new TriggerConfigMenu(trigger);

    trigger_config_menu->init();
    trigger_config_menu->popOut();

    while (1) {
        trigger_config_menu->update(HAL::Millis());
        HAL::CanvasUpdate();

        if (trigger_config_menu->want2Quit()) {
            break;
        }

        HAL::FeedTheDog();
    }

    delete trigger_config_menu;
    spdlog::info("quit trigger config menu");
}
