/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "panels.h"
#include "../../../../hal/hal.h"
#include "../../../../assets/assets.h"
#include "../../../../shared/shared.h"
#include "../../../utils/system/system.h"
#include <mooncake.h>

using namespace SmoothUIToolKit;
using namespace SmoothUIToolKit::SelectMenu;
using namespace SYSTEM::INPUTS;
using namespace VIEW;

/* -------------------------------------------------------------------------- */
/*                                    Base                                    */
/* -------------------------------------------------------------------------- */
void MonitorPanelBase::reset(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor)
{
    AssetPool::LoadFont12(panelCanvas);

    panelCanvas->setCursor(0, 0);
    panelCanvas->setTextScroll(true);
    panelCanvas->setTextColor(fgColor);
    panelCanvas->setBaseColor(bgColor);
    panelCanvas->fillScreen(bgColor);

    SharedData::BorrowData();
    SharedData::Console().clear();
    SharedData::Console().setAutoNewLine(true);
    SharedData::Console().setEnable(true);
    SharedData::ReturnData();
}

void MonitorPanelBase::update(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor)
{
    SharedData::BorrowData();

    // Message
    if (HAL::Millis() - _data.msg_update_time_count > _data.msg_update_interval) {
        if (!SharedData::Console().isEmpty()) {
            if (SharedData::Console().needClear()) {
                panelCanvas->fillScreen(bgColor);
                panelCanvas->setCursor(0, 0);
                SharedData::Console().clearDone();
            } else {
                // Remove old cursor
                panelCanvas->fillRect(panelCanvas->getCursorX(), panelCanvas->getCursorY() + 10, 6, 3, bgColor);
            }

            char c = 0;
            if (SharedData::Console().valueNum() < 60) {
                SharedData::Console().get(c);
                panelCanvas->print(c);

                // Speed up
                if (SharedData::Console().get(c)) {
                    panelCanvas->print(c);
                }
            } else {
                while (SharedData::Console().valueNum() >= 60) {
                    SharedData::Console().get(c);
                    panelCanvas->print(c);
                }
            }

            // New cursor
            panelCanvas->fillRect(panelCanvas->getCursorX(), panelCanvas->getCursorY() + 10, 6, 2, fgColor);
        }

        _data.msg_update_time_count = HAL::Millis();
    }

    // Cursor blink
    if (HAL::Millis() - _data.cursor_update_time_count > _data.cursor_update_interval) {
        _data.cursor_type = !_data.cursor_type;

        panelCanvas->fillRect(panelCanvas->getCursorX(), panelCanvas->getCursorY() + 10, 6, 2,
                              _data.cursor_type ? fgColor : bgColor);

        _data.cursor_update_time_count = HAL::Millis();
    }

    SharedData::ReturnData();
}

/* -------------------------------------------------------------------------- */
/*                                     I2C                                    */
/* -------------------------------------------------------------------------- */
void MonitorPanel_I2C::onEnter()
{
    spdlog::info("enter i2c panel");
    HAL::CreateLogMonitorDaemon(LogMonitor::I2C);
}

void MonitorPanel_I2C::onQuit()
{
    spdlog::info("quit i2c panel");
    HAL::DestroyLogMonitorDaemon(LogMonitor::I2C);
}

/* -------------------------------------------------------------------------- */
/*                                    UART                                    */
/* -------------------------------------------------------------------------- */
void MonitorPanel_UART::onEnter()
{
    spdlog::info("enter uart panel");
    HAL::CreateLogMonitorDaemon(LogMonitor::UART);
}

void MonitorPanel_UART::onQuit()
{
    spdlog::info("quit uart panel");
    HAL::DestroyLogMonitorDaemon(LogMonitor::UART);
}

/* -------------------------------------------------------------------------- */
/*                                   Modbus                                   */
/* -------------------------------------------------------------------------- */
void MonitorPanel_Modbus::onEnter()
{
    spdlog::info("enter modbus panel");
    HAL::CreateLogMonitorDaemon(LogMonitor::Modbus);
}

void MonitorPanel_Modbus::onQuit()
{
    spdlog::info("quit modbus panel");
    HAL::DestroyLogMonitorDaemon(LogMonitor::Modbus);
}

/* -------------------------------------------------------------------------- */
/*                                     CAN                                    */
/* -------------------------------------------------------------------------- */
void MonitorPanel_CAN::onEnter()
{
    spdlog::info("enter can panel");
    HAL::CreateLogMonitorDaemon(LogMonitor::CAN);
}

void MonitorPanel_CAN::onQuit()
{
    spdlog::info("quit can panel");
    HAL::DestroyLogMonitorDaemon(LogMonitor::CAN);
}

/* -------------------------------------------------------------------------- */
/*                                    SENS                                    */
/* -------------------------------------------------------------------------- */
void MonitorPanel_SENS::update(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor)
{
    if (_is_first_in) {
        _is_first_in = false;
        _text_color.jumpTo(bgColor);
        _text_color.moveTo(fgColor);
        _text_color.setDuration(600);
    }

    _text_color.update(HAL::Millis());
    panelCanvas->setTextColor(_text_color.getCurrentColorHex());

    // Clear
    panelCanvas->fillScreen(bgColor);

    // Sens infos
    if (HAL::Millis() - _data.msg_update_time_count > 500) {
        HAL::UpdatePowerMonitor();
        HAL::UpdateTemp();
        _data.msg_update_time_count = HAL::Millis();
    }

    panelCanvas->setTextDatum(middle_left);
    panelCanvas->drawString("Power Voltage:", 10, 11);
    panelCanvas->drawString("Ext Current:", 10, 37);
    panelCanvas->drawString("Device Temp:", 10, 63);

    std::string string_buffer;
    panelCanvas->setTextDatum(middle_right);

    string_buffer = fmt::format("{:.1f}V", HAL::GetPowerVoltage());
    panelCanvas->drawString(string_buffer.c_str(), 200, 11);
    string_buffer = fmt::format("{:.2f}A", HAL::GetExpanderOutputCurrent());
    panelCanvas->drawString(string_buffer.c_str(), 200, 37);
    string_buffer = fmt::format("{:.1f}°C", HAL::GetTemp());
    panelCanvas->drawString(string_buffer.c_str(), 200, 63);
}

void MonitorPanel_SENS::onEnter()
{
    spdlog::info("enter sens panel");
    _is_first_in = true;
}

void MonitorPanel_SENS::onQuit()
{
    spdlog::info("quit sens panel");
}
