/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "config_panel.h"
#include <cstring>
#include <mooncake.h>
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "apps/utils/system/inputs/button/button.h"

using namespace SmoothUIToolKit::Widgets;
using namespace VIEW::TimerRelayView;
using namespace SYSTEM::INPUTS;

constexpr static uint32_t _theme_color = 0xFFDCB0;

static TimerRelay::TimerRelayConfig_t* _config_storage = nullptr;
static ConfigPanel* _config_panel_instance             = nullptr;

ConfigPanel::~ConfigPanel()
{
    if (_data.selector != nullptr) {
        delete _data.selector;
    }

    // Store config
    if (_config_storage == nullptr) {
        _config_storage = new TimerRelay::TimerRelayConfig_t;
    }
    std::memcpy(_config_storage, &_data.config, sizeof(TimerRelay::TimerRelayConfig_t));
}

void ConfigPanel::onInit()
{
    _config_panel_instance = this;

    // Restore storage
    if (_config_storage != nullptr) {
        std::memcpy(&_data.config, _config_storage, sizeof(TimerRelay::TimerRelayConfig_t));
    }

    // Add configs
    addChild(new ConfigOnInterval(&_data.config));
    addChild(new ConfigOffInterval(&_data.config));
    addChild(new ConfigRelayId(&_data.config));
    addChild(new ConfigRunButton(&_data.config));

    _data.selector = new ConfigSelector;
    _data.selector->init();
    _data.selector->enter(this);
    _data.selector->setRenderOnUpdate(false);
}

void ConfigPanel::popOut()
{
    SmoothWidgetBase::popOut();
    _data.selector->popOut();
}

void ConfigPanel::onRender()
{
    HAL::GetCanvas()->fillScreen(_theme_color);
    _data.selector->render();
}

void ConfigPanel::onPostRender()
{
    // HAL::CanvasUpdate();
}

void ConfigPanel::onUpdate()
{
    Button::Update();

    _data.selector->update(HAL::Millis());

    if (Button::Back()->wasClicked()) {
        _data.want_2_quit = true;
    }
}

void ConfigPanel::RenderConfigPanel()
{
    if (_config_panel_instance == nullptr) {
        return;
    }
    _config_panel_instance->render();
}
