/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "hal.h"
#include <mooncake.h>
#include "../assets/assets.h"
#include "apps/utils/system/inputs/button/button.h"
#include "spdlog/spdlog.h"
#include <sstream>
#include "../apps/utils/system/system.h"

using namespace SYSTEM::INPUTS;

HAL* HAL::_hal = nullptr;

HAL* HAL::Get()
{
    return _hal;
}

bool HAL::Check()
{
    return _hal != nullptr;
}

bool HAL::Inject(HAL* hal)
{
    if (_hal != nullptr) {
        spdlog::error("HAL already exist");
        return false;
    }

    if (hal == nullptr) {
        spdlog::error("invalid HAL ptr");
        return false;
    }

    hal->init();
    spdlog::info("HAL injected, type: {}", hal->type());

    _hal = hal;

    return true;
}

void HAL::Destroy()
{
    if (_hal == nullptr) {
        spdlog::error("HAL not exist");
        return;
    }

    delete _hal;
    _hal = nullptr;
}

void HAL::renderFpsPanel()
{
    static unsigned long time_count = 0;

    _data.canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _data.canvas->setTextSize(1);
    _data.canvas->setTextDatum(top_left);
    _data.canvas->drawNumber(1000 / (millis() - time_count), 0, 0, &fonts::Font0);

    time_count = millis();
}

static constexpr int _msg_start_x     = 15;
static constexpr int _msg_start_y     = 55;
static constexpr int _msg_line_height = 24;

void HAL::popFatalError(std::string msg)
{
    // Echo
    spdlog::error("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _data.canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopFatalError);
    _data.canvas->fillScreen(AssetPool::GetColor().Misc.bgPopFatalError);

    AssetPool::LoadFont24(_data.canvas);
    _data.canvas->setTextSize(1);
    _data.canvas->setTextDatum(top_left);
    _data.canvas->setCursor(20, 10);
    _data.canvas->printf(":(");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont16(_data.canvas);
    _data.canvas->setCursor(_msg_start_x, line_height);
    // _data.canvas->printf("Fatal Error!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line)) {
        _data.canvas->setCursor(_msg_start_x, line_height);
        _data.canvas->print(line.c_str());
        line_height += _msg_line_height;
    }

    /* ---------------------------------- Push ---------------------------------- */
    _data.canvas->pushSprite(0, 0);
    _data.canvas->pushSprite(0, 0);  // Sometimes need double hits after sd apis
    _data.display->setBrightness(255);

    // Press any button to poweroff
    while (1) {
        feedTheDog();
        delay(50);

        if (getAnyButton(true)) {
            reboot();
        }
    }

    // while (1)
    // {
    //     spdlog::error("{}", msg);
    //     feedTheDog();
    //     delay(1000);
    // }
}

void HAL::popWarning(std::string msg)
{
    // Echo
    spdlog::warn("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _data.canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopWarning);
    _data.canvas->fillScreen(AssetPool::GetColor().Misc.bgPopWarning);

    AssetPool::LoadFont24(_data.canvas);
    _data.canvas->setTextSize(1);
    _data.canvas->setTextDatum(top_left);
    _data.canvas->setCursor(20, 10);
    _data.canvas->printf(":(");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont16(_data.canvas);
    _data.canvas->setCursor(_msg_start_x, line_height);
    // _data.canvas->printf("Fatal Error!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line)) {
        _data.canvas->setCursor(_msg_start_x, line_height);
        _data.canvas->print(line.c_str());
        line_height += _msg_line_height;
    }

    /* ---------------------------------- Push ---------------------------------- */
    _data.canvas->pushSprite(0, 0);
    _data.canvas->pushSprite(0, 0);  // Sometimes need double hits after sd apis
    _data.display->setBrightness(255);

    // Press any button to poweroff
    while (1) {
        feedTheDog();
        delay(50);

        if (getAnyButton(true)) {
            break;
        }
    }
}

void HAL::popError(std::string msg)
{
    // Echo
    spdlog::error("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _data.canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopWarning);
    _data.canvas->fillScreen(AssetPool::GetColor().Misc.bgPopWarning);

    AssetPool::LoadFont24(_data.canvas);
    _data.canvas->setTextSize(1);
    _data.canvas->setTextDatum(top_left);
    _data.canvas->setCursor(20, 10);
    _data.canvas->printf(":(");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont16(_data.canvas);
    _data.canvas->setCursor(_msg_start_x, line_height);
    // _data.canvas->printf("Fatal Error!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line)) {
        _data.canvas->setCursor(_msg_start_x, line_height);
        _data.canvas->print(line.c_str());
        line_height += _msg_line_height;
    }

    /* ---------------------------------- Push ---------------------------------- */
    _data.canvas->pushSprite(0, 0);
    _data.canvas->pushSprite(0, 0);  // Sometimes need double hits after sd apis
    _data.display->setBrightness(255);

    // Press any button to poweroff
    while (1) {
        feedTheDog();
        delay(50);

        if (getAnyButton(true)) {
            break;
        }
    }
}

void HAL::popSuccess(std::string msg, bool showSuccessLabel)
{
    // Echo
    spdlog::info("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _data.canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopSuccess);
    _data.canvas->fillScreen(AssetPool::GetColor().Misc.bgPopSuccess);

    AssetPool::LoadFont24(_data.canvas);
    _data.canvas->setTextSize(1);
    _data.canvas->setTextDatum(top_left);
    _data.canvas->setCursor(20, 10);
    _data.canvas->printf(":)");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont16(_data.canvas);
    _data.canvas->setCursor(_msg_start_x, line_height);
    // _data.canvas->printf("Fatal Error!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line)) {
        _data.canvas->setCursor(_msg_start_x, line_height);
        _data.canvas->print(line.c_str());
        line_height += _msg_line_height;
    }

    /* ---------------------------------- Push ---------------------------------- */
    _data.canvas->pushSprite(0, 0);
    _data.canvas->pushSprite(0, 0);  // Sometimes need double hits after sd apis
    _data.display->setBrightness(255);

    // Press any button to poweroff
    while (1) {
        feedTheDog();
        delay(50);

        if (getAnyButton(true)) {
            break;
        }
    }
}

// Cpp sucks
tm* HAL::getLocalTime()
{
    time(&_data.time_buffer);
    return localtime(&_data.time_buffer);
}

bool HAL::getAnyButton(bool waitRelease)
{
    for (int i = Gamepad::BTN_START; i < Gamepad::GAMEPAD_BUTTON_NUM; i++) {
        if (getButton(static_cast<Gamepad::GamepadButton_t>(i))) {
            if (waitRelease) {
                while (getAnyButton(false)) {
                    feedTheDog();
                    delay(50);
                }
            }
            return true;
        }
    }
    return false;
}
