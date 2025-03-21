/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class SettingPage : public SmoothUIToolKit::SelectMenu::SmoothSelector {
public:
    struct Theme_t {
        uint32_t background = 0xB4DCBB;
        uint32_t optionText = 0x012706;
        uint32_t selector   = 0x7EA685;
    };

    void init(const std::vector<std::string>& optionList, int startIndex = 0);
    void onReadInput() override;
    void onRender() override;
    void onPress() override;
    void onClick() override;
    void onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime) override;

    bool was_selected       = false;
    bool stop_input_reading = false;
    bool push_canvas        = true;
    std::function<void(int optionIndex)> onOptionPressed;

    inline bool isQuiting()
    {
        return _data.is_quiting;
    }

    inline Theme_t& theme()
    {
        return _theme;
    }

private:
    struct Data_t {
        int string_y_offset        = 0;
        bool is_quiting            = false;
        bool is_start_up_anim_done = false;
        std::vector<SmoothUIToolKit::Transition2D> option_start_up_transition_list;
    };
    Data_t _data;
    Theme_t _theme;

    void _update_camera_keyframe() override;
    void _render_selector();
    void _render_options();
};
