/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "../../../hal/hal.h"
#include <cstdint>
#include <smooth_ui_toolkit.h>

namespace VIEW {
namespace TimerRelayView {
class ConfigSelector : public SmoothUIToolKit::Widgets::Selector::SmoothSelector {
public:
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onRender() override;
    void onUpdate() override;
};

class ConfigPanel : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    ~ConfigPanel();
    void onInit() override;
    void popOut() override;
    void onRender() override;
    void onPostRender() override;
    void onUpdate() override;
    bool want2Quit()
    {
        return _data.want_2_quit;
    }

    inline const TimerRelay::TimerRelayConfig_t& getConfig()
    {
        return _data.config;
    }

    // Expose for select menu's background rendering
    static void RenderConfigPanel();

private:
    struct Data_t {
        ConfigSelector* selector = nullptr;
        TimerRelay::TimerRelayConfig_t config;
        bool want_2_quit = false;
    };
    Data_t _data;
};

class ConfigOnInterval : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    ConfigOnInterval(TimerRelay::TimerRelayConfig_t* config) : _config(config)
    {
    }
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
    void onSelect() override;
    void onUnSelect() override;

private:
    TimerRelay::TimerRelayConfig_t* _config;
    std::string _string_buffer;
};

class ConfigOffInterval : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    ConfigOffInterval(TimerRelay::TimerRelayConfig_t* config) : _config(config)
    {
    }
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
    void onSelect() override;
    void onUnSelect() override;

private:
    TimerRelay::TimerRelayConfig_t* _config;
    std::string _string_buffer;
};

class ConfigRelayId : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    ConfigRelayId(TimerRelay::TimerRelayConfig_t* config) : _config(config)
    {
    }
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
    void onSelect() override;
    void onUnSelect() override;

private:
    TimerRelay::TimerRelayConfig_t* _config;
    std::string _string_buffer;
};

class ConfigRunButton : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    ConfigRunButton(TimerRelay::TimerRelayConfig_t* config) : _config(config)
    {
    }
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
    void onSelect() override;
    void onUnSelect() override;

private:
    TimerRelay::TimerRelayConfig_t* _config;
    std::string _string_buffer;
    uint32_t _relay_state_update_time_count = 0;
    bool _relay_state                       = false;
    void _render_relay_icon(SmoothUIToolKit::Vector4D_t& frame);
};
}  // namespace TimerRelayView
}  // namespace VIEW
