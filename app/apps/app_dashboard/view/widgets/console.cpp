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

static constexpr int _panel_startup_x = 0 + 2;
static constexpr int _panel_startup_y = 135 - 2;
static constexpr int _panel_startup_w = 79;
static constexpr int _panel_startup_h = 90;

static constexpr int _panel_x = 0 + 2;
static constexpr int _panel_y = 45 - 2;
static constexpr int _panel_w = 79;
static constexpr int _panel_h = 90;
static constexpr int _panel_r = 6;

static constexpr int _inner_pannel_mt = 2;
static constexpr int _inner_pannel_ml = 2;
static constexpr int _inner_pannel_r  = 4;

static constexpr int _canvas_w  = 65;
static constexpr int _canvas_h  = 70;
static constexpr int _canvas_mt = 5;
static constexpr int _canvas_ml = 8;

static constexpr int _tag_mt = 73;

static uint32_t _color_pannel       = 0x3A3A3A;
static uint32_t _color_inner_pannel = 0xF3FFD5;

WidgetConsole::~WidgetConsole()
{
    if (_data.terminal_canvas != nullptr) {
        _data.terminal_canvas->unloadFont();
        _data.terminal_canvas->deleteSprite();
        delete _data.terminal_canvas;
    }
}

void WidgetConsole::onInit()
{
    if (_data.terminal_canvas != nullptr) {
        spdlog::error("canvas exist");
    } else {
        _data.terminal_canvas = new LGFX_SpriteFx(HAL::GetCanvas());
        _data.terminal_canvas->createSprite(_canvas_w, _canvas_h);
        // terminal_canvas->fillScreen(TFT_WHITE);

        AssetPool::LoadFont10(_data.terminal_canvas);
        _data.terminal_canvas->setTextScroll(true);
        _data.terminal_canvas->setTextColor(_color_pannel);
        _data.terminal_canvas->setBaseColor(_color_inner_pannel);
        _data.terminal_canvas->fillScreen(_color_inner_pannel);

        SharedData::Console().log("Console created :)");
        SharedData::Console().setAutoNewLine(true);
    }
}

void WidgetConsole::onPopOut()
{
    constexpr int delay = 120;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setEachDelay(delay + 70, delay, delay + 70, delay + 70);
    // getTransition().setEachDuration(500, 300, 500, 500);
    getTransition().setDuration(400);
    getTransition().setTransitionPath(EasingPath::easeOutBack);

    _data.msg_update_time_count = 0;
    _data.cursor_type           = true;
    SharedData::Console().setEnable(true);
}

void WidgetConsole::onHide()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().setEachDelay(delay, delay + 70, delay, delay);

    SharedData::Console().setEnable(false);
}

void WidgetConsole::onUpdate()
{
    if (!isPoppedOut()) {
        return;
    }

    SharedData::BorrowData();
    _update_message();
    SharedData::ReturnData();
}

void WidgetConsole::onRender()
{
    auto frame = getTransition().getValue();

    // panel
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_pannel);
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x + _inner_pannel_ml, frame.y + _inner_pannel_mt,
                                          frame.w - _inner_pannel_ml * 2, frame.h - _inner_pannel_mt * 2,
                                          _inner_pannel_r, _color_inner_pannel);

    // Tag
    HAL::GetCanvas()->pushImage(frame.x, frame.y + _tag_mt, 35, 17, AssetPool::GetImage().AppDashboard.tag_console);

    // Console
    if (isPoppedOut()) {
        _data.terminal_canvas->pushSprite(_panel_x + _canvas_ml, _panel_y + _canvas_mt);
    }
}

void WidgetConsole::_update_message()
{
    // Message
    if (HAL::Millis() - _data.msg_update_time_count > _data.msg_update_interval) {
        if (!SharedData::Console().isEmpty()) {
            // Remove old cursor
            _data.terminal_canvas->fillRect(_data.terminal_canvas->getCursorX(),
                                            _data.terminal_canvas->getCursorY() + 10, 6, 3, _color_inner_pannel);

            char c = 0;
            if (SharedData::Console().valueNum() < 60) {
                SharedData::Console().get(c);
                _data.terminal_canvas->print(c);
            } else {
                while (SharedData::Console().valueNum() >= 60) {
                    SharedData::Console().get(c);
                    _data.terminal_canvas->print(c);
                }
            }

            // New cursor
            _data.terminal_canvas->fillRect(_data.terminal_canvas->getCursorX(),
                                            _data.terminal_canvas->getCursorY() + 10, 6, 2, _color_pannel);
        }

        _data.msg_update_time_count = HAL::Millis();
    }

    // Cursor blink
    if (HAL::Millis() - _data.cursor_update_time_count > _data.cursor_update_interval) {
        _data.cursor_type = !_data.cursor_type;
        _data.terminal_canvas->fillRect(_data.terminal_canvas->getCursorX(), _data.terminal_canvas->getCursorY() + 10,
                                        6, 2, _data.cursor_type ? _color_pannel : _color_inner_pannel);
        _data.cursor_update_time_count = HAL::Millis();
    }
}
