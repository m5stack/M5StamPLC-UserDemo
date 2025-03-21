/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "setting_page.h"
#include "../../utils/system/system.h"
#include <mooncake.h>
#include <hal/hal.h>
#include <assets/assets.h>

using namespace SmoothUIToolKit;
using namespace SYSTEM::INPUTS;

static constexpr int _selector_padding_x  = 5;
static constexpr int _selector_padding_y  = 5;
static constexpr int _selector_padding_2x = 10;
static constexpr int _selector_padding_2y = 10;
static constexpr int _selector_radius     = 10;

static constexpr int _option_margin_top  = 14;
static constexpr int _option_margin_left = 15;
static constexpr int _option_height      = 16;

static constexpr int _option_open_anim_duration = 100;

void SettingPage::init(const std::vector<std::string>& optionList, int startIndex)
{
    // Selector
    setPositionDuration(200);
    setShapeDuration(400);
    setShapeTransitionPath(EasingPath::easeOutBack);

    // Slow down at the start up
    getSelectorPostion().jumpTo(-20, -20);
    getSelectorPostion().setDelay(100);
    getSelectorPostion().setUpdateCallback([](Transition2D* transition) {
        if (transition->isFinish()) {
            transition->setDelay(0);
            transition->setUpdateCallback(nullptr);
        }
    });
    getSelectorShape().setDelay(100);
    getSelectorShape().setUpdateCallback([](Transition2D* transition) {
        if (transition->isFinish()) {
            transition->setDelay(0);
            transition->setUpdateCallback(nullptr);
        }
    });

    // Camera
    setConfig().cameraSize.height = HAL::GetCanvas()->height();
    getCamera().setTransitionPath(EasingPath::easeOutBack);
    getCamera().setDuration(400);

    // Create start up transitions
    _data.option_start_up_transition_list.resize(optionList.size());

    // Add option
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_theme.optionText);
    HAL::GetCanvas()->setTextDatum(top_left);
    for (int i = 0; i < optionList.size(); i++) {
        OptionProps_t new_option_props;

        new_option_props.userData   = (void*)optionList[i].c_str();
        new_option_props.keyframe.x = _option_margin_left;
        new_option_props.keyframe.y = _option_margin_top + (_option_height + _option_margin_top) * i;
        new_option_props.keyframe.w = HAL::GetCanvas()->textWidth(optionList[i].c_str());
        new_option_props.keyframe.h = _option_height;

        addOption(new_option_props);

        // Set startup anim
        _data.option_start_up_transition_list[i].setDelay(i * 30 + 100);
        _data.option_start_up_transition_list[i].setDuration(400);
        _data.option_start_up_transition_list[i].setTransitionPath(EasingPath::easeOutBack);
        _data.option_start_up_transition_list[i].jumpTo(-240, new_option_props.keyframe.y - 24);
        _data.option_start_up_transition_list[i].moveTo(new_option_props.keyframe.x, new_option_props.keyframe.y);
    }

    // Offset for efont font
    _data.string_y_offset = 0;

    moveTo(startIndex);
}

/* -------------------------------------------------------------------------- */
/*                                    Input                                   */
/* -------------------------------------------------------------------------- */
void SettingPage::onReadInput()
{
    if (isOpening()) {
        return;
    }

    if (stop_input_reading) {
        return;
    }

    /* ---------------------------------- Open ---------------------------------- */
    Button::Update();

    if (Button::Next()->wasClicked()) {
        goNext();
    } else if (Button::Next()->wasHold()) {
        goLast();
    }

    if (Button::Ok()->wasPressed()) {
        // Squeeze selector
        Vector4D_t squeeze_shape;
        squeeze_shape.w = getSelectedKeyframe().w * 5 / 6;
        squeeze_shape.h = getSelectedKeyframe().h * 2 / 3;
        squeeze_shape.x = getSelectedKeyframe().x + getSelectedKeyframe().w / 12;
        squeeze_shape.y = getSelectedKeyframe().y + getSelectedKeyframe().h / 6;
        press(squeeze_shape);
    }

    else if (Button::Ok()->wasReleased()) {
        release();
    }

    /* ---------------------------------- Back ---------------------------------- */
    else if (Button::Back()->wasClicked()) {
        _data.is_quiting = true;
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void SettingPage::_render_selector()
{
    HAL::GetCanvas()->fillSmoothRoundRect(getSelectorCurrentFrame().x - _selector_padding_x,
                                          getSelectorCurrentFrame().y - _selector_padding_y + 3 - getCameraOffset().y,
                                          getSelectorCurrentFrame().w + _selector_padding_2x,
                                          getSelectorCurrentFrame().h + _selector_padding_2y,
                                          isOpening() ? _selector_radius * 4 : _selector_radius, _theme.selector);
}

void SettingPage::_render_options()
{
    HAL::GetCanvas()->setTextColor(_theme.optionText);
    HAL::GetCanvas()->setTextDatum(top_left);
    if (_data.is_start_up_anim_done) {
        // Static
        for (int i = 0; i < getOptionList().size(); i++) {
            HAL::GetCanvas()->drawString((const char*)getOptionList()[i].userData, getOptionList()[i].keyframe.x,
                                         getOptionList()[i].keyframe.y - getCameraOffset().y + _data.string_y_offset);
        }
    } else {
        // Anim
        for (int i = 0; i < getOptionList().size(); i++) {
            HAL::GetCanvas()->drawString(
                (const char*)getOptionList()[i].userData, _data.option_start_up_transition_list[i].getValue().x,
                _data.option_start_up_transition_list[i].getValue().y - getCameraOffset().y + _data.string_y_offset);
        }
    }
}

void SettingPage::onRender()
{
    // Background
    HAL::GetCanvas()->fillScreen(_theme.background);

    // HAL::GetCanvas()->fillScreen(_theme.background);

    if (isOpening()) {
        _render_options();
        _render_selector();
    } else {
        _render_selector();
        _render_options();
    }

    // Push
    if (push_canvas) {
        HAL::CanvasUpdate();
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Camera                                   */
/* -------------------------------------------------------------------------- */
void SettingPage::_update_camera_keyframe()
{
    if (_config.cameraSize.height == 0) {
        return;
    }

    // Check if selector's target frame is inside of camera
    int new_y_offset = getCameraOffset().y;

    // Top
    if (getSelectedKeyframe().y - _selector_padding_y - _option_height < new_y_offset) {
        new_y_offset = getSelectedKeyframe().y - _option_margin_top;
    }

    // Bottom
    else if (getSelectedKeyframe().y + _selector_padding_y + 3 + getSelectedKeyframe().h >
             new_y_offset + _config.cameraSize.height) {
        new_y_offset = getSelectedKeyframe().y + getSelectedKeyframe().h - _config.cameraSize.height +
                       _option_margin_top + _selector_padding_y;
    }

    getCamera().moveTo(0, new_y_offset);
}

void SettingPage::onPress()
{
    // Make postion and shape anim same
    setDuration(400);
    setTransitionPath(EasingPath::easeOutBack);
    if (onOptionPressed) {
        onOptionPressed(getSelectedOptionIndex());
    }
}

void SettingPage::onClick()
{
    was_selected = true;
}

void SettingPage::onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime)
{
    if (_data.is_start_up_anim_done) {
        return;
    }

    // Update start up anim
    for (int i = 0; i < _data.option_start_up_transition_list.size(); i++) {
        _data.option_start_up_transition_list[i].update(currentTime);

        // Check the last anim
        if (i == _data.option_start_up_transition_list.size() - 1) {
            if (_data.option_start_up_transition_list[i].isFinish()) {
                _data.is_start_up_anim_done = true;
                _data.option_start_up_transition_list.clear();
            }
        }
    }
}
