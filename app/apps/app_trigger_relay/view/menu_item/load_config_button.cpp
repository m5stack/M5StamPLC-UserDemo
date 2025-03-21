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
#include "hal/utils/trigger_relay/trigger_relay.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace VIEW::TriggerRelayView;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x         = 76;
static constexpr int _panel_y         = 94;
static constexpr int _panel_w         = 30;
static constexpr int _panel_h         = 33;
static constexpr int _panel_r         = 10;
static uint32_t _color_panel          = 0xBFEDE4;
static uint32_t _color_text           = 0x412110;
static constexpr int _icon_mt         = 7;
static constexpr int _icon_ml         = 6;

void MenuItem_LoadConfigButton::onInit()
{
}

void MenuItem_LoadConfigButton::onPopOut()
{
    constexpr int delay = 80;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setDuration(300);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void MenuItem_LoadConfigButton::onHide()
{
}

void MenuItem_LoadConfigButton::onRender()
{
    if (isHidden()) {
        return;
    }

    auto frame = getTransition().getValue();

    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_panel);

    HAL::GetCanvas()->pushImage(frame.x + _icon_ml, frame.y + _icon_mt, 18, 18,
                                AssetPool::GetImage().AppTriggerRelay.icon_load);
}

void MenuItem_LoadConfigButton::onUpdate()
{
}

void MenuItem_LoadConfigButton::onSelect()
{
    auto config_index_list = TriggerRelay::LoadExistingConfigsIndicesFromFs();

    if (config_index_list.empty()) {
        HAL::PopWarning("No saved config");
        unSelect();
        return;
    }

    while (1) {
        // Create a menu to show saved tirggers config
        auto selected_index_buffer = SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
            props.primary                  = _color_panel;
            props.onPrimary                = _color_text;
            props.title                    = "Triggers:";
            props.onCustomRenderBackground = []() { TirrgerRelayConfigMenu::RenderConfigPanel(); };

            // Option list
            props.optionList.resize(config_index_list.size());
            for (int i = 0; i < props.optionList.size(); i++) {
                _string_buffer      = fmt::format("Config: {:03}", config_index_list[i]);
                props.optionList[i] = _string_buffer;
            }
        });

        if (selected_index_buffer < 0) {
            break;
        }

        // Create a menu to show what to do with the selected triggers config
        int selected_config_index = config_index_list[selected_index_buffer];
        selected_index_buffer     = SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
            props.primary                  = _color_panel;
            props.onPrimary                = _color_text;
            props.title                    = fmt::format("Config: {:03}", selected_config_index);
            props.onCustomRenderBackground = []() { TirrgerRelayConfigMenu::RenderConfigPanel(); };

            // Option list
            props.optionList.push_back("Load");
            props.optionList.push_back("Delete");
        });

        if (selected_index_buffer == 0) {
            spdlog::info("try load config by index: {:03}", selected_config_index);
            *_config = TriggerRelay::LoadConfigFromFsByIndex(selected_config_index);
            break;
        } else if (selected_index_buffer == 1) {
            TriggerRelay::DeleteConfigByIndex(selected_config_index);
            config_index_list = TriggerRelay::LoadExistingConfigsIndicesFromFs();
            if (config_index_list.empty()) {
                break;
            }
        }
    }

    unSelect();
}

void MenuItem_LoadConfigButton::onUnSelect()
{
}
