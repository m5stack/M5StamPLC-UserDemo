/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "view.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "../app_launcher.h"
#include <cstdint>
#include <mooncake.h>

using namespace SmoothUIToolKit;
using namespace SmoothUIToolKit::SelectMenu;
using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace VIEW;

/* -------------------------------------------------------------------------- */
/*                                   Config                                   */
/* -------------------------------------------------------------------------- */
static constexpr int _card_w              = 240 + 120;
static constexpr int _card_h              = 135 + 30;
static constexpr int _card_ml             = 100;
static constexpr int _card_triangle_x0_ml = 0;
static constexpr int _card_triangle_x0_mt = 0;
static constexpr int _card_triangle_x1_ml = -72;
static constexpr int _card_triangle_x1_mt = _card_h;
static constexpr int _card_triangle_x2_ml = 0;
static constexpr int _card_triangle_x2_mt = _card_h;
static constexpr int _app_name_label_ml   = 52;
static constexpr int _app_name_label_mt   = 108;
static constexpr int _icon_ml             = 19;
static constexpr int _icon_mt             = 14;
static constexpr int _icon_w              = 70;
static constexpr int _icon_h              = 70;

/* -------------------------------------------------------------------------- */
/*                                    Setup                                   */
/* -------------------------------------------------------------------------- */
// For restoration to last opened
static int _last_selected_option_index = -1;

void LauncherView::SetLastSelectedOptionIndex(const int& index)
{
    _last_selected_option_index = index;
}

void LauncherView::init()
{
    _setup_option_keyframes();

    // Config
    setConfig().renderInterval    = 15;
    setConfig().readInputInterval = 50;
    setDuration(400);
    setPositionTransitionPath(EasingPath::easeOutBack);
    for (auto& i : setOptionList()) {
        i.position.jumpTo(-120, 0);
        i.shape.jumpTo(_card_w, _card_h);
    }

    // If need restoration
    if (_last_selected_option_index != -1) {
        // Jump to selected option
        jumpTo(_last_selected_option_index);

        // Jump to opened anim
        getSelectedOption().position.jumpTo(0, 0);
        getSelectedOption().shape.jumpTo(240, 135);
    }
}

void LauncherView::_setup_option_keyframes()
{
    spdlog::info("size {}", _data.launcher_option_props_list.size());

    // Selected one
    setKeyframe(0).reset(_card_ml, 0, _card_w, _card_h);

    // Waiting line
    for (int i = 0; i < _data.launcher_option_props_list.size() - 2; i++) {
        setKeyframe(i + 1).reset(_card_ml, _card_h + _card_h * i, _card_w, _card_h);
    }

    // Set the last one next to selected one to smooth the loop
    setKeyframe(_data.launcher_option_props_list.size() - 2).reset(-130, _card_h, _card_w, _card_h);
    setLastKeyframe({-130, 0, _card_w, _card_h});

    // Setup render order
    _data.render_order[0] = 1;
    _data.render_order[1] = getKeyframeList().size() - 1;
    _data.render_order[2] = 0;
}

void LauncherView::addAppOption(const AppOptionProps_t& appOptionProps)
{
    _data.launcher_option_props_list.push_back(appOptionProps);
    addOption();
}

/* -------------------------------------------------------------------------- */
/*                                 Navigation                                 */
/* -------------------------------------------------------------------------- */
void LauncherView::onReadInput()
{
    if (isOpening()) {
        return;
    }

    Button::Update();

    if (Button::Next()->wasClicked()) {
        goNext();
    } else if (Button::Next()->wasHold()) {
        goLast();
    }

    if (Button::Ok()->wasPressed()) {
        // Squeeze it
        press({_card_ml - 16, 0, _card_w, _card_h});
    } else if (Button::Ok()->wasReleased()) {
        release();
    }
}

void LauncherView::onPress()
{
    // Set press anim
    setDuration(400);
}

void LauncherView::onClick()
{
    // Set open anim
    setDuration(100);
    setTransitionPath(EasingPath::easeOutQuad);
    open({0, 0, 240, 135});
}

void LauncherView::onOpenEnd()
{
    app_open_callback(_data.launcher_option_props_list[getSelectedOptionIndex()].name, getSelectedOptionIndex());

    // Store selected option for restoration in next create
    _last_selected_option_index = getSelectedOptionIndex();

    close();
    // Launcher view (this) will be destroy on next mooncake update, no need to close anymore
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void LauncherView::onRender()
{
    // Clear
    HAL::GetCanvas()->fillScreen(TFT_WHITE);
    HAL::GetCanvas()->setTextDatum(middle_center);
    AssetPool::LoadFont24(HAL::GetCanvas());

    for (const auto& i : _data.render_order) {
        getMatchingOptionIndex(i, _data.matching_index);
        auto frame = getOptionCurrentFrame(_data.matching_index);

        // Render cards
        HAL::GetCanvas()->fillRect(frame.x, frame.y, frame.w, frame.h,
                                   _data.launcher_option_props_list[_data.matching_index].themeColor);
        HAL::GetCanvas()->fillTriangle(frame.x + _card_triangle_x0_ml, frame.y + _card_triangle_x0_mt,
                                       frame.x + _card_triangle_x1_ml, frame.y + _card_triangle_x1_mt,
                                       frame.x + _card_triangle_x2_ml, frame.y + _card_triangle_x2_mt,
                                       _data.launcher_option_props_list[_data.matching_index].themeColor);

        if (isOpening()) {
            continue;
        }

        // Render icons
        if (_data.launcher_option_props_list[_data.matching_index].icon != nullptr) {
            HAL::GetCanvas()->pushImage(frame.x + _icon_ml, frame.y + _icon_mt, _icon_w, _icon_h,
                                        _data.launcher_option_props_list[_data.matching_index].icon);
        }

        // Render name label
        HAL::GetCanvas()->setTextColor(_data.launcher_option_props_list[_data.matching_index].appNameColor);
        HAL::GetCanvas()->drawString(_data.launcher_option_props_list[_data.matching_index].name.c_str(),
                                     frame.x + _app_name_label_ml, frame.y + _app_name_label_mt);
    }

    // Push
    HAL::CanvasUpdate();
}
