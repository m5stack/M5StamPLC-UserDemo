/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "core/types/types.h"
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <string>
#include "../../../../hal/hal.h"

class WidgetConsole : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    ~WidgetConsole();

    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;

private:
    struct Data_t {
        uint32_t msg_update_time_count    = 0;
        uint32_t msg_update_interval      = 20;
        uint32_t cursor_update_time_count = 0;
        uint32_t cursor_update_interval   = 500;
        bool cursor_type                  = true;
        LGFX_SpriteFx* terminal_canvas    = nullptr;
    };
    Data_t _data;
    void _update_message();
};

class WidgetIoPanel : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;

private:
    struct Data_t {
        uint32_t update_io_time_count = 0;
        std::vector<bool> input_state_list;
        std::vector<bool> output_state_list;
    };
    Data_t _data;
    void _update_input_panel();
    void _update_output_panel();
    void _render_input_panel(SmoothUIToolKit::Vector4D_t& frame);
    void _render_output_panel(SmoothUIToolKit::Vector4D_t& frame);
};

class WidgetStatusPanel : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;

private:
    struct Data_t {
        bool wifi_state   = false;
        bool ezdata_state = false;
        std::string time;
        std::string date;
        uint32_t update_time_count = 0;
    };
    Data_t _data;
};
