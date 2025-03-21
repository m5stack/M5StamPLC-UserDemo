/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "trigger_relay_config_menu.h"
#include <cstring>
#include <mooncake.h>
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "apps/utils/system/inputs/button/button.h"

using namespace SmoothUIToolKit::Widgets;
using namespace VIEW::TriggerRelayView;
using namespace SYSTEM::INPUTS;

constexpr static uint32_t _theme_color = 0x74D6C5;

static TirrgerRelayConfigMenu* _config_panel_instance = nullptr;

TirrgerRelayConfigMenu::~TirrgerRelayConfigMenu()
{
    if (_data.selector != nullptr) {
        delete _data.selector;
    }
}

void TirrgerRelayConfigMenu::onInit()
{
    _config_panel_instance = this;

    // Add configs
    addChild(new MenuItem_TriggersPreview(&_data.config));
    addChild(new MenuItem_AddTriggerButton(&_data.config));
    addChild(new MenuItem_SaveConfigButton(&_data.config));
    addChild(new MenuItem_LoadConfigButton(&_data.config));
    addChild(new MenuItem_RunButton(&_data.config));

    _data.selector = new TirrgerRelayConfigMenuSelector;
    _data.selector->init();
    _data.selector->enter(this);
    _data.selector->setRenderOnUpdate(false);
}

void TirrgerRelayConfigMenu::popOut()
{
    SmoothWidgetBase::popOut();
    _data.selector->popOut();
}

void TirrgerRelayConfigMenu::onRender()
{
    HAL::GetCanvas()->fillScreen(_theme_color);
    _data.selector->render();
}

void TirrgerRelayConfigMenu::onPostRender()
{
    // HAL::CanvasUpdate();
}

void TirrgerRelayConfigMenu::onUpdate()
{
    Button::Update();

    _data.selector->update(HAL::Millis());

    if (Button::Back()->wasClicked()) {
        _data.want_2_quit = true;
    }
}

void TirrgerRelayConfigMenu::RenderConfigPanel()
{
    if (_config_panel_instance == nullptr) {
        return;
    }
    _config_panel_instance->render();
}
