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

namespace VIEW {
class LauncherView : public SmoothUIToolKit::SelectMenu::SmoothOptions {
public:
    struct AppOptionProps_t {
        uint32_t themeColor   = 0xFFFFFF;
        uint32_t appNameColor = 0x000000;
        std::string name;
        const uint16_t* icon = nullptr;
    };

    void init();
    void addAppOption(const AppOptionProps_t& appOptionProps);

    std::function<void(const std::string&, int)> app_open_callback = nullptr;

    void onReadInput() override;
    void onRender() override;
    void onPress() override;
    void onClick() override;
    void onOpenEnd() override;

    static void SetLastSelectedOptionIndex(const int& index);

private:
    struct Data_t {
        std::vector<AppOptionProps_t> launcher_option_props_list;
        std::array<size_t, 3> render_order;
        int matching_index = 0;
    };
    Data_t _data;
    void _setup_option_keyframes();
};
}  // namespace VIEW
