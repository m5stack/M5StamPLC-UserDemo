/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <smooth_ui_toolkit.h>

class WidgetDesktop : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    void onInit() override;
    void onRender() override;
    void onPostRender() override;
};
