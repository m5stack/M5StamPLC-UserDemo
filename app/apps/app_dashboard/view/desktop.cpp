/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "desktop.h"
#include <cstdint>
#include <smooth_ui_toolkit.h>
#include "../../../assets/assets.h"
#include "lgfx/v1/misc/enum.hpp"
#include "widgets/widgets.h"

using namespace SmoothUIToolKit;

static uint32_t _color_background = 0xF3FFD5;

void WidgetDesktop::onInit()
{
    // Child widgets
    addChild(new WidgetConsole);
    addChild(new WidgetIoPanel);
    addChild(new WidgetStatusPanel);
}

void WidgetDesktop::onRender()
{
    HAL::GetCanvas()->fillScreen(_color_background);
}

void WidgetDesktop::onPostRender()
{
    HAL::CanvasUpdate();
}
