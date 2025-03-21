/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <functional>
#include <mooncake.h>
#include <smooth_ui_toolkit.h>
#include <string>
#include <vector>
#include "panels/panels.h"

namespace VIEW {
class MenuBar : public SmoothUIToolKit::SelectMenu::SmoothOptions {
public:
    struct AppOptionProps_t {
        uint32_t themeColor;
        uint32_t appNameColor;
        std::string name;
        MonitorPanelBase* panel_activity;
    };

    ~MenuBar();

    void init();
    void addAppOption(const AppOptionProps_t& appOptionProps);

    void onReadInput() override;
    void onGoNext() override;
    void onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime) override;
    void onRender() override;

private:
    struct Data_t {
        std::vector<AppOptionProps_t> launcher_option_props_list;

        int matching_index = 0;
        std::array<size_t, 5> render_order;

        SmoothUIToolKit::SmoothRGB bg_color;

        LGFX_SpriteFx* panel_canvas = nullptr;
    };
    Data_t _data;
    void _setup_option_keyframes();
};
}  // namespace VIEW
