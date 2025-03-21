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
namespace TriggerRelayView {
class TirrgerRelayConfigMenuSelector : public SmoothUIToolKit::Widgets::Selector::SmoothSelector {
public:
    virtual void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onRender() override;
    void onUpdate() override;
};

class TirrgerRelayConfigMenu : public SmoothUIToolKit::Widgets::SmoothWidgetBase {
public:
    ~TirrgerRelayConfigMenu();
    void onInit() override;
    void popOut() override;
    void onRender() override;
    void onPostRender() override;
    void onUpdate() override;
    bool want2Quit()
    {
        return _data.want_2_quit;
    }

    inline const TriggerRelay::Config_t& getConfig()
    {
        return _data.config;
    }

    // Expose for select menu's background rendering
    static void RenderConfigPanel();

protected:
    struct Data_t {
        TirrgerRelayConfigMenuSelector* selector = nullptr;
        TriggerRelay::Config_t config;
        bool want_2_quit = false;
    };
    Data_t _data;
};

class MenuItem_TriggersPreview : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_TriggersPreview(TriggerRelay::Config_t* config) : _config(config)
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
    TriggerRelay::Config_t* _config;
    std::string _string_buffer;
};

class MenuItem_AddTriggerButton : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_AddTriggerButton(TriggerRelay::Config_t* config) : _config(config)
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
    TriggerRelay::Config_t* _config;
};

class MenuItem_SaveConfigButton : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_SaveConfigButton(TriggerRelay::Config_t* config) : _config(config)
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
    TriggerRelay::Config_t* _config;
    std::string _string_buffer;
};

class MenuItem_LoadConfigButton : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_LoadConfigButton(TriggerRelay::Config_t* config) : _config(config)
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
    TriggerRelay::Config_t* _config;
    std::string _string_buffer;
};

class MenuItem_RunButton : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_RunButton(TriggerRelay::Config_t* config) : _config(config)
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
    TriggerRelay::Config_t* _config;
    std::string _string_buffer;
};
}  // namespace TriggerRelayView
}  // namespace VIEW
