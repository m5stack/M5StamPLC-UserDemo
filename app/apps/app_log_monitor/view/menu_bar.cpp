/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "menu_bar.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "../app_log_monitor.h"
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
static constexpr int _card_w              = 80;
static constexpr int _card_h              = 18;
static constexpr int _app_name_label_ml   = 40;
static constexpr int _app_name_label_mt   = 9;
static constexpr int _card_triangle_x0_ml = 80;
static constexpr int _card_triangle_x0_mt = 0;
static constexpr int _card_triangle_x1_ml = 80;
static constexpr int _card_triangle_x1_mt = _card_h - 1;
static constexpr int _card_triangle_x2_ml = 80 + 14;
static constexpr int _card_triangle_x2_mt = _card_h - 1;

static constexpr int _canvas_w = 210;
// static constexpr int _canvas_h  = 90;
static constexpr int _canvas_h  = 70;
static constexpr int _canvas_ml = 15;
static constexpr int _canvas_mt = 32;

/* -------------------------------------------------------------------------- */
/*                                    Setup                                   */
/* -------------------------------------------------------------------------- */
MenuBar::~MenuBar()
{
    // Quit current panel activity
    getMatchingOptionIndex(0, _data.matching_index);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->onQuit();

    // Free panel activities
    spdlog::info("free panel activities");
    for (auto& i : _data.launcher_option_props_list) {
        delete i.panel_activity;
    }

    if (_data.panel_canvas != nullptr) {
        spdlog::info("free panel canvas");
        _data.panel_canvas->unloadFont();
        _data.panel_canvas->deleteSprite();
        delete _data.panel_canvas;
    }
}

void MenuBar::init()
{
    _setup_option_keyframes();

    // Config
    setConfig().renderInterval    = 15;
    setConfig().readInputInterval = 50;
    setDuration(400);
    setPositionTransitionPath(EasingPath::easeOutBack);
    for (int i = 0; i < getOptionList().size(); i++) {
        setOptionList()[i].position.getYTransition().setTransitionPath(EasingPath::easeOutQuad);
        setOptionList()[i].position.getYTransition().setDuration(200);

        setOptionList()[i].position.jumpTo(i * _card_w, -_card_h);
        setOptionList()[i].shape.jumpTo(_card_w, _card_h);
        setOptionList()[i].position.getYTransition().setDelay(i * 60);
    }

    // Bg color
    _data.bg_color.jumpTo(_data.launcher_option_props_list[0].themeColor);
    _data.bg_color.setDuration(200);

    // Setup render order
    _data.render_order[0] = 3;
    _data.render_order[1] = getKeyframeList().size() - 1;
    _data.render_order[2] = 2;
    _data.render_order[3] = 1;
    _data.render_order[4] = 0;

    // Panel
    if (_data.panel_canvas != nullptr) {
        spdlog::error("panel canvas already exist");
        return;
    }
    _data.panel_canvas = new LGFX_SpriteFx(HAL::GetCanvas());
    _data.panel_canvas->createSprite(_canvas_w, _canvas_h);

    getMatchingOptionIndex(0, _data.matching_index);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->reset(
        _data.panel_canvas, _data.launcher_option_props_list[_data.matching_index].themeColor,
        _data.launcher_option_props_list[_data.matching_index].appNameColor);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->onEnter();
}

void MenuBar::_setup_option_keyframes()
{
    spdlog::info("size {}", _data.launcher_option_props_list.size());

    // Waiting line
    for (int i = 0; i < _data.launcher_option_props_list.size() - 1; i++) {
        setKeyframe(i).reset(i * _card_w, 0, _card_w, _card_h);
    }

    // Set the last one next to selected one to smooth the loop
    setLastKeyframe({-_card_w, 0, _card_w, _card_h});
}

void MenuBar::addAppOption(const AppOptionProps_t& appOptionProps)
{
    _data.launcher_option_props_list.push_back(appOptionProps);
    addOption();
}

/* -------------------------------------------------------------------------- */
/*                                 Navigation                                 */
/* -------------------------------------------------------------------------- */
void MenuBar::onReadInput()
{
    if (isOpening()) {
        return;
    }

    Button::Update();

    if (Button::Next()->wasClicked()) {
        goNext();
    }
}

void MenuBar::onGoNext()
{
    // Update bg color
    getMatchingOptionIndex(0, _data.matching_index);
    _data.bg_color.moveTo(_data.launcher_option_props_list[_data.matching_index].themeColor);

    // Switch panel activity
    // Old panel
    getMatchingOptionIndex(_data.launcher_option_props_list.size() - 1, _data.matching_index);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->onQuit();
    // New panel
    getMatchingOptionIndex(0, _data.matching_index);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->reset(
        _data.panel_canvas, _data.launcher_option_props_list[_data.matching_index].themeColor,
        _data.launcher_option_props_list[_data.matching_index].appNameColor);
    _data.launcher_option_props_list[_data.matching_index].panel_activity->onEnter();
}

/* -------------------------------------------------------------------------- */
/*                                   Update                                   */
/* -------------------------------------------------------------------------- */
void MenuBar::onUpdate(const TimeSize_t& currentTime)
{
    _data.bg_color.update(currentTime);
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void MenuBar::onRender()
{
    // Clear
    HAL::GetCanvas()->fillScreen(_data.bg_color.getCurrentColorHex());
    HAL::GetCanvas()->setTextDatum(middle_center);
    AssetPool::LoadFont12(HAL::GetCanvas());

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

        // Render name
        HAL::GetCanvas()->setTextColor(_data.launcher_option_props_list[_data.matching_index].appNameColor);
        HAL::GetCanvas()->drawString(_data.launcher_option_props_list[_data.matching_index].name.c_str(),
                                     frame.x + _app_name_label_ml, frame.y + _app_name_label_mt);
    }

    // Panel canvas
    if (_data.bg_color.isFinish()) {
        getMatchingOptionIndex(0, _data.matching_index);
        _data.launcher_option_props_list[_data.matching_index].panel_activity->update(
            _data.panel_canvas, _data.launcher_option_props_list[_data.matching_index].themeColor,
            _data.launcher_option_props_list[_data.matching_index].appNameColor);

        _data.panel_canvas->pushSprite(_canvas_ml, _canvas_mt);
    }

    // Push
    HAL::CanvasUpdate();
}
