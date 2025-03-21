/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "widgets.h"
#include "../../../../assets/assets.h"
#include "../../../../shared/shared.h"
#include <mooncake.h>
#include <smooth_ui_toolkit.h>

using namespace SmoothUIToolKit;

static constexpr int _panel_startup_x = 85 - 2;
static constexpr int _panel_startup_y = 135 + 2;
static constexpr int _panel_startup_w = 155;
static constexpr int _panel_startup_h = 135 - 4;

static constexpr int _panel_x = 85 - 2;
static constexpr int _panel_y = 0 + 2;
static constexpr int _panel_w = 155;
static constexpr int _panel_h = 135 - 4;

static constexpr int _pannel_input_h        = 55;
static constexpr int _pannel_input_r        = 6;
static constexpr int _inner_pannel_input_mt = 2;
static constexpr int _inner_pannel_input_ml = 2;
static constexpr int _inner_pannel_input_r  = 4;

// Same shape to panel input
static constexpr int _pannel_output_mt = 76;

static constexpr int _tag_mt = 48;

static uint32_t _color_pannel       = 0x3A3A3A;
static uint32_t _color_inner_pannel = 0xF3FFD5;

void WidgetIoPanel::onInit()
{
    _data.input_state_list.clear();
    _data.input_state_list.resize(8);
    _data.output_state_list.clear();
    _data.output_state_list.resize(4);
}

void WidgetIoPanel::onPopOut()
{
    constexpr int delay = 60;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().setEachDelay(delay + 70, delay, delay + 70, delay + 70);
    getTransition().setDuration(400);
    getTransition().setTransitionPath(EasingPath::easeOutBack);
}

void WidgetIoPanel::onHide()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().setEachDelay(delay, delay + 70, delay, delay);
}

void WidgetIoPanel::onUpdate()
{
    if (HAL::Millis() - _data.update_io_time_count > 50) {
        _update_input_panel();
        _update_output_panel();
        _data.update_io_time_count = HAL::Millis();
    }
}

void WidgetIoPanel::onRender()
{
    auto frame = getTransition().getValue();

    // Tag
    HAL::GetCanvas()->pushImage(frame.x, frame.y + _tag_mt, 155, 35, AssetPool::GetImage().AppDashboard.tag_io);
    _render_input_panel(frame);
    _render_output_panel(frame);
}

void WidgetIoPanel::_update_input_panel()
{
    for (int i = 0; i < _data.input_state_list.size(); i++) {
        _data.input_state_list[i] = HAL::GetPlcInput(i);
    }
}

void WidgetIoPanel::_update_output_panel()
{
    for (int i = 0; i < _data.output_state_list.size(); i++) {
        _data.output_state_list[i] = HAL::GetPlcRelayState(i);
    }
}
/* -------------------------------------------------------------------------- */
/*                             Input panel render                             */
/* -------------------------------------------------------------------------- */
static constexpr int _block_input_w   = 14;
static constexpr int _block_input_h   = 39;
static constexpr int _block_input_mt  = 8;
static constexpr int _block_input_ml  = 8;
static constexpr int _block_input_gap = 4;
static constexpr int _block_input_r   = 4;

static uint32_t _color_block_input_high       = 0x29DC97;
static uint32_t _color_block_input_low        = 0xDBDED2;
static uint32_t _color_block_input_label_high = 0x11A25D;
static uint32_t _color_block_input_label_low  = 0xA1A498;

void WidgetIoPanel::_render_input_panel(SmoothUIToolKit::Vector4D_t& frame)
{
    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, _pannel_input_h, _pannel_input_r, _color_pannel);
    HAL::GetCanvas()->fillSmoothRoundRect(
        frame.x + _inner_pannel_input_ml, frame.y + _inner_pannel_input_mt, frame.w - _inner_pannel_input_ml * 2,
        _pannel_input_h - _inner_pannel_input_mt * 2, _inner_pannel_input_r, _color_inner_pannel);

    // State blocks
    for (int i = 0; i < _data.input_state_list.size(); i++) {
        HAL::GetCanvas()->fillSmoothRoundRect(
            frame.x + _block_input_ml + (_block_input_w + _block_input_gap) * i, frame.y + _block_input_mt,
            _block_input_w, _block_input_h, _block_input_r,
            _data.input_state_list[i] ? _color_block_input_high : _color_block_input_low);

        AssetPool::LoadFont12(HAL::GetCanvas());
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(_data.input_state_list[i] ? _color_block_input_label_high
                                                                 : _color_block_input_label_low);
        HAL::GetCanvas()->drawNumber(
            i + 1, frame.x + _block_input_ml + (_block_input_w + _block_input_gap) * i + _block_input_w / 2,
            frame.y + _block_input_mt + _block_input_h / 2);
    }
}

/* -------------------------------------------------------------------------- */
/*                             Output panel render                            */
/* -------------------------------------------------------------------------- */
static constexpr int _block_output_w   = 32;
static constexpr int _block_output_h   = 41;
static constexpr int _block_output_mt  = 83;
static constexpr int _block_output_ml  = 7;
static constexpr int _block_output_gap = 4;
static constexpr int _block_output_r   = 4;

static uint32_t _color_block_output_high       = 0xFF6B6B;
static uint32_t _color_block_output_low        = 0xDBDED2;
static uint32_t _color_block_output_label_high = 0xC53131;
static uint32_t _color_block_output_label_low  = 0xA1A498;

void WidgetIoPanel::_render_output_panel(SmoothUIToolKit::Vector4D_t& frame)
{
    // Panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y + _pannel_output_mt, frame.w, _pannel_input_h,
                                          _pannel_input_r, _color_pannel);
    HAL::GetCanvas()->fillSmoothRoundRect(
        frame.x + _inner_pannel_input_ml, frame.y + _inner_pannel_input_mt + _pannel_output_mt,
        frame.w - _inner_pannel_input_ml * 2, _pannel_input_h - _inner_pannel_input_mt * 2, _inner_pannel_input_r,
        _color_inner_pannel);

    // State blocks
    for (int i = 0; i < _data.output_state_list.size(); i++) {
        HAL::GetCanvas()->fillSmoothRoundRect(
            frame.x + _block_output_ml + (_block_output_w + _block_output_gap) * i, frame.y + _block_output_mt,
            _block_output_w, _block_output_h, _block_output_r,
            _data.output_state_list[i] ? _color_block_output_high : _color_block_output_low);

        AssetPool::LoadFont12(HAL::GetCanvas());
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(_data.output_state_list[i] ? _color_block_output_label_high
                                                                  : _color_block_output_label_low);
        HAL::GetCanvas()->drawNumber(
            i + 1, frame.x + _block_output_ml + (_block_output_w + _block_output_gap) * i + _block_output_w / 2,
            frame.y + _block_output_mt + _block_output_h / 2);
    }
}
