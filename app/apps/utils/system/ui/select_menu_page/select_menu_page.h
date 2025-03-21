/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "core/transition3d/transition3d.h"
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace SYSTEM {
namespace UI {
class SelectMenuPage : public SmoothUIToolKit::SelectMenu::SmoothSelector {
public:
    struct Props_t {
        uint32_t primary             = 0xE9B685;
        uint32_t onPrimary           = 0x4A2705;
        uint32_t backgroundMaskColor = 0x000000;
        uint8_t backgroundMaskAlpha  = 160;
        std::string title;
        std::vector<std::string> optionList;
        int startupIndex                                                        = 0;
        std::function<void()> onCustomRenderBackground                          = nullptr;
        std::function<void(int optionIndex, int x, int y)> onCustomOptionRender = nullptr;
        std::function<void(int selectedIndex)> onOptionSelected                 = nullptr;
    };

    // Set props before init()
    inline Props_t& setProps()
    {
        return _data.props;
    }
    void init();
    bool isSelectFinish();
    inline int getSelectResult()
    {
        return _data.select_result;
    }

    void onReadInput() override;
    void onRender() override;
    void onGoNext() override;
    void onPress() override;
    void onClick() override;
    void onOpenEnd() override;
    void onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime) override;
    void onQuit();

    // public:
    static int CreateAndWaitResult(std::function<void(Props_t& props)> onPropsSetup = nullptr);

private:
    struct Data_t {
        Props_t props;

        bool is_selected  = false;
        int select_result = -1;

        SmoothUIToolKit::Transition2D transition_title_panel;
        SmoothUIToolKit::Transition2D transition_option_panel;
        SmoothUIToolKit::Transition2D transition_background;
        SmoothUIToolKit::SmoothRGB transition_selected_label_color;
        int option_panel_x_offset = 0;
    };
    Data_t _data;
    void _update_camera_keyframe() override;
    void _render_background();
    void _render_selector();
    void _render_option_panel();
    void _render_options();
    void _render_title();
};
}  // namespace UI
}  // namespace SYSTEM
