/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "../../../../hal/hal.h"
#include <cstdint>

namespace VIEW {
class MonitorPanelBase {
public:
    virtual ~MonitorPanelBase()
    {
    }

    void reset(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor);
    virtual void update(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor);

    virtual void onEnter()
    {
    }
    virtual void onQuit()
    {
    }

protected:
    struct Data_t {
        uint32_t msg_update_time_count    = 0;
        uint32_t msg_update_interval      = 20;
        uint32_t cursor_update_time_count = 0;
        uint32_t cursor_update_interval   = 500;
        bool cursor_type                  = true;
    };
    Data_t _data;
};

class MonitorPanel_I2C : public MonitorPanelBase {
public:
    void onEnter() override;
    void onQuit() override;
};

class MonitorPanel_UART : public MonitorPanelBase {
public:
    void onEnter() override;
    void onQuit() override;
};

class MonitorPanel_Modbus : public MonitorPanelBase {
public:
    void onEnter() override;
    void onQuit() override;
};

class MonitorPanel_CAN : public MonitorPanelBase {
public:
    void onEnter() override;
    void onQuit() override;
};

class MonitorPanel_SENS : public MonitorPanelBase {
public:
    void update(LGFX_SpriteFx* panelCanvas, const uint32_t& bgColor, const uint32_t& fgColor) override;
    void onEnter() override;
    void onQuit() override;

private:
    SmoothUIToolKit::SmoothRGB _text_color;
    bool _is_first_in = true;
};
}  // namespace VIEW
