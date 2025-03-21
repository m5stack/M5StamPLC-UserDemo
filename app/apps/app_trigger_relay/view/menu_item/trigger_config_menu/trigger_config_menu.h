/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <hal/hal.h>
#include <cstdint>
#include <smooth_ui_toolkit.h>
#include "../../trigger_relay_config_menu.h"
#include "hal/utils/trigger_relay/types.h"

namespace VIEW {
namespace TriggerRelayView {
namespace TriggerConfigMenu {
class TriggerConfigMenu : public TirrgerRelayConfigMenu {
public:
    TriggerConfigMenu(TriggerRelay::Trigger_t* trigger) : _trigger(trigger)
    {
    }
    void onInit() override;

    // Expose for select menu's background rendering
    static void RenderConfigPanel();
    static void CreateTriggerConfigMenu(TriggerRelay::Trigger_t* trigger);

private:
    TriggerRelay::Trigger_t* _trigger = nullptr;
};

class MenuItem_IfCondition : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_IfCondition(TriggerRelay::Trigger_t* trigger) : _trigger(trigger)
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
    TriggerRelay::Trigger_t* _trigger;
    std::string _string_buffer;
};

class MenuItem_ThenAction : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_ThenAction(TriggerRelay::Trigger_t* trigger) : _trigger(trigger)
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
    TriggerRelay::Trigger_t* _trigger;
    std::string _string_buffer;
};

class MenuItem_ElseAction : public SmoothUIToolKit::Widgets::Selector::SmoothOption {
public:
    MenuItem_ElseAction(TriggerRelay::Trigger_t* trigger) : _trigger(trigger)
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
    TriggerRelay::Trigger_t* _trigger;
    std::string _string_buffer;
};

}  // namespace TriggerConfigMenu
}  // namespace TriggerRelayView
}  // namespace VIEW
