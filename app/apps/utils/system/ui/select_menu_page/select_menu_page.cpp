/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "select_menu_page.h"
#include "../../../../../assets/assets.h"
#include "../../inputs/inputs.h"
#include "apps/utils/system/inputs/button/button.h"
#include "core/easing_path/easing_path.h"
#include "core/transition3d/transition3d.h"
#include "core/types/types.h"
#include "hal/hal.h"
#include "lgfx/v1/misc/enum.hpp"
#include <mooncake.h>

using namespace SYSTEM::UI;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

static constexpr int _selector_startup_x  = 240;
static constexpr int _selector_startup_y  = -30;
static constexpr int _selector_padding_x  = 5;
static constexpr int _selector_padding_y  = 3;
static constexpr int _selector_padding_2x = _selector_padding_x * 2;
static constexpr int _selector_padding_2y = _selector_padding_y * 2;
static constexpr int _selector_radius     = 10;

static constexpr int _title_panel_height    = 43;
static constexpr int _title_panel_startup_y = -45;
static constexpr int _title_panel_y         = -10;
static constexpr int _title_panel_radius    = 10;
static constexpr int _title_label_ml        = 8;
static constexpr int _title_label_mt        = _title_panel_height / 2 + 4;

static constexpr int _option_labels_mt = 14;
static constexpr int _option_label_my  = 14;
static constexpr int _option_label_mx  = 24;
static constexpr int _option_label_h   = 24;

static constexpr int _option_panel_radius        = 12;
static constexpr int _option_panel_stroke_width  = 2;
static constexpr int _option_panel_stroke_radius = 10;

void SelectMenuPage::init()
{
    /* -------------------------------- Selector -------------------------------- */
    setPositionDuration(200);
    setShapeDuration(400);
    setShapeTransitionPath(EasingPath::easeOutBack);

    // Slow down at the start up
    getSelectorPostion().jumpTo(_selector_startup_x, _selector_startup_y);
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

    /* --------------------------------- Camera --------------------------------- */
    setConfig().cameraSize.height = HAL::GetCanvas()->height();
    getCamera().setTransitionPath(EasingPath::easeOutBack);
    getCamera().setDuration(400);

    /* ------------------------------- Title panel ------------------------------ */
    _data.transition_title_panel.setDelay(0);
    _data.transition_title_panel.setDuration(400);
    _data.transition_title_panel.setTransitionPath(EasingPath::easeOutBack);
    _data.transition_title_panel.jumpTo(0, _title_panel_startup_y);
    _data.transition_title_panel.moveTo(0, _title_panel_y);

    /* ------------------------------ Option panel ------------------------------ */
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_data.props.onPrimary);
    HAL::GetCanvas()->setTextDatum(top_left);

    // Add option
    int max_option_width = -1;
    for (int i = 0; i < _data.props.optionList.size(); i++) {
        OptionProps_t new_option_props;

        new_option_props.keyframe.x = _option_label_mx;
        new_option_props.keyframe.y = _option_labels_mt + _option_label_my + (_option_label_h + _option_label_my) * i;
        new_option_props.keyframe.w = HAL::GetCanvas()->textWidth(_data.props.optionList[i].c_str());
        new_option_props.keyframe.h = _option_label_h;

        if (new_option_props.keyframe.w > max_option_width) {
            max_option_width = new_option_props.keyframe.w;
        }

        addOption(new_option_props);
    }
    _data.option_panel_x_offset = HAL::GetCanvas()->width() - max_option_width - _option_label_mx * 2;

    // Option panel transition
    _data.transition_option_panel.setDelay(0);
    _data.transition_option_panel.setDuration(200);
    _data.transition_option_panel.jumpTo(HAL::GetCanvas()->width(), 0);
    _data.transition_option_panel.moveTo(_data.option_panel_x_offset, 0);

    /* ------------------------------- Background ------------------------------- */
    _data.transition_background.setDelay(0);
    _data.transition_background.setDuration(200);
    _data.transition_background.jumpTo(0, 0);
    _data.transition_background.moveTo(_data.props.backgroundMaskAlpha, 0);

    /* -------------------------- Selected label color -------------------------- */
    _data.transition_selected_label_color.setDelay(200);
    _data.transition_selected_label_color.setUpdateCallback([](Transition3D* transition) {
        if (transition->isFinish()) {
            transition->setDelay(0);
            transition->setUpdateCallback(nullptr);
        }
    });
    _data.transition_selected_label_color.setDuration(200);
    _data.transition_selected_label_color.jumpTo(_data.props.onPrimary);
    _data.transition_selected_label_color.moveTo(_data.props.primary);

    /* ------------------------------ Startup index ----------------------------- */
    moveTo(_data.props.startupIndex);
}

/* -------------------------------------------------------------------------- */
/*                                    Input                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::onReadInput()
{
    if (isOpening()) {
        return;
    }

    if (_data.is_selected) {
        return;
    }

    Button::Update();

    if (Button::Back()->wasClicked()) {
        _data.select_result = -1;
        onQuit();
    }

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
    } else if (Button::Ok()->wasReleased()) {
        release();
    }
}

void SelectMenuPage::onQuit()
{
    _data.is_selected = true;

    _data.transition_title_panel.setDelay(0);
    _data.transition_title_panel.jumpTo(0, _title_panel_y);
    _data.transition_title_panel.moveTo(0, _title_panel_startup_y);

    _data.transition_option_panel.setDelay(0);
    _data.transition_option_panel.jumpTo(_data.option_panel_x_offset, 0);
    _data.transition_option_panel.moveTo(HAL::GetCanvas()->width(), 0);

    _data.transition_background.setDelay(0);
    _data.transition_background.jumpTo(80, 0);
    _data.transition_background.moveTo(0, 0);

    getSelectorPostion().moveTo(_selector_startup_x, _selector_startup_y);
}

bool SelectMenuPage::isSelectFinish()
{
    if (!_data.is_selected) {
        return false;
    }
    if (!_data.transition_title_panel.isFinish()) {
        return false;
    }
    if (!_data.transition_option_panel.isFinish()) {
        return false;
    }
    if (!_data.transition_background.isFinish()) {
        return false;
    }
    if (!getSelectorPostion().isFinish()) {
        return false;
    }
    return true;
}

void SelectMenuPage::onGoNext()
{
    _data.transition_selected_label_color.jumpTo(_data.props.onPrimary);
    _data.transition_selected_label_color.moveTo(_data.props.primary);
}

void SelectMenuPage::onPress()
{
    // Make postion and shape anim same
    setDuration(400);
    setTransitionPath(EasingPath::easeOutBack);
}

void SelectMenuPage::onClick()
{
    open(getSelectedKeyframe());
}

void SelectMenuPage::onOpenEnd()
{
    _data.select_result = getSelectedOptionIndex();
    if (_data.props.onOptionSelected != nullptr) {
        _data.props.onOptionSelected(_data.select_result);
    }
    onQuit();
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::_render_background()
{
    if (_data.props.onCustomRenderBackground != nullptr) {
        _data.props.onCustomRenderBackground();
        HAL::GetCanvas()->fillRectAlpha(0, 0, HAL::GetCanvas()->width(), HAL::GetCanvas()->height(),
                                        _data.transition_background.getValue().x, _data.props.backgroundMaskColor);
    } else {
        HAL::GetCanvas()->fillScreen(_data.props.primary);
    }
}

void SelectMenuPage::_render_selector()
{
    HAL::GetCanvas()->fillSmoothRoundRect(
        getSelectorCurrentFrame().x - _selector_padding_x + _data.option_panel_x_offset,
        getSelectorCurrentFrame().y - _selector_padding_y - getCameraOffset().y,
        getSelectorCurrentFrame().w + _selector_padding_2x, getSelectorCurrentFrame().h + _selector_padding_2y,
        isOpening() ? _selector_radius * 4 : _selector_radius, _data.props.onPrimary);
}

void SelectMenuPage::_render_option_panel()
{
    HAL::GetCanvas()->fillSmoothRoundRect(_data.transition_option_panel.getXTransition().getValue(), 0,
                                          HAL::GetCanvas()->width(), HAL::GetCanvas()->height(), _option_panel_radius,
                                          _data.props.onPrimary);
    HAL::GetCanvas()->fillSmoothRoundRect(
        _data.transition_option_panel.getXTransition().getValue() + _option_panel_stroke_width,
        0 + _option_panel_stroke_width, HAL::GetCanvas()->width() - _option_panel_stroke_width * 2,
        HAL::GetCanvas()->height() - _option_panel_stroke_width * 2, _option_panel_stroke_radius, _data.props.primary);
}

void SelectMenuPage::_render_options()
{
    HAL::GetCanvas()->setTextDatum(top_left);
    HAL::GetCanvas()->setTextColor(_data.props.primary);

    Vector4D_t frame;
    for (int i = 0; i < getOptionList().size(); i++) {
        frame = getOptionList()[i].keyframe;
        frame.x += _data.transition_option_panel.getXTransition().getValue();
        // frame.y += _data.transition_option_panel.getYTransition().getValue();

        if (_data.props.onCustomOptionRender != nullptr) {
            _data.props.onCustomOptionRender(i, frame.x, frame.y - getCameraOffset().y);
            continue;
        }

        HAL::GetCanvas()->setTextColor(i == getSelectedOptionIndex()
                                           ? _data.transition_selected_label_color.getCurrentColorHex()
                                           : _data.props.onPrimary);
        HAL::GetCanvas()->drawString(_data.props.optionList[i].c_str(), frame.x, frame.y - getCameraOffset().y);
    }
}

void SelectMenuPage::_render_title()
{
    auto frame = _data.transition_title_panel.getValue();

    HAL::GetCanvas()->fillSmoothRoundRect(0, frame.y, HAL::GetCanvas()->width(), _title_panel_height,
                                          _title_panel_radius, _data.props.primary);

    HAL::GetCanvas()->setTextColor(_data.props.onPrimary);
    HAL::GetCanvas()->setTextDatum(middle_left);
    HAL::GetCanvas()->drawString(_data.props.title.c_str(), _title_label_ml, frame.y + _title_label_mt);
}

void SelectMenuPage::onRender()
{
    _render_background();
    _render_title();
    _render_option_panel();
    _render_selector();
    _render_options();

    // Push
    HAL::CanvasUpdate();
}

/* -------------------------------------------------------------------------- */
/*                                   Camera                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::_update_camera_keyframe()
{
    if (_config.cameraSize.height == 0) {
        return;
    }

    // Check if selector's target frame is inside of camera
    int new_y_offset = getCameraOffset().y;

    // Top
    if (getSelectedKeyframe().y - _selector_padding_y - _option_label_h < new_y_offset) {
        new_y_offset = getSelectedKeyframe().y - _option_label_h;
    }

    // Bottom
    else if (getSelectedKeyframe().y + _selector_padding_y + 3 + getSelectedKeyframe().h >
             new_y_offset + _config.cameraSize.height) {
        new_y_offset = getSelectedKeyframe().y + getSelectedKeyframe().h - _config.cameraSize.height +
                       _option_label_my + _selector_padding_y;
    }

    getCamera().moveTo(0, new_y_offset);
}

void SelectMenuPage::onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime)
{
    _data.transition_title_panel.update(currentTime);
    _data.transition_option_panel.update(currentTime);
    _data.transition_background.update(currentTime);
    _data.transition_selected_label_color.update(currentTime);
}

int SelectMenuPage::CreateAndWaitResult(std::function<void(Props_t& props)> onPropsSetup)
{
    // Create menu
    auto select_menu = new SelectMenuPage;

    // Props setup
    if (onPropsSetup != nullptr) {
        onPropsSetup(select_menu->setProps());
    }
    select_menu->init();

    /* -------------------------- Wait option selected -------------------------- */
    while (1) {
        HAL::FeedTheDog();

        select_menu->update(HAL::Millis());
        if (select_menu->isSelectFinish()) {
            break;
        }
    }
    auto ret = select_menu->getSelectResult();

    /* ---------------------------------- Free ---------------------------------- */
    delete select_menu;

    return ret;
}
