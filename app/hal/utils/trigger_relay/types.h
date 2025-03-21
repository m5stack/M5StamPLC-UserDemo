/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace TriggerRelay {
namespace Condition {
enum Condition_t {
    False = 0,
    True,

    // Input
    Input_1_High,
    Input_1_Low,
    Input_2_High,
    Input_2_Low,
    Input_3_High,
    Input_3_Low,
    Input_4_High,
    Input_4_Low,
    Input_5_High,
    Input_5_Low,
    Input_6_High,
    Input_6_Low,
    Input_7_High,
    Input_7_Low,
    Input_8_High,
    Input_8_Low,
};
}

namespace Action {
enum Action_t {
    None = 0,

    // Relay
    Relay_1_On,
    Relay_1_Off,
    Relay_2_On,
    Relay_2_Off,
    Relay_3_On,
    Relay_3_Off,
    Relay_4_On,
    Relay_4_Off,
};
}

struct Trigger_t {
    Condition::Condition_t condition = Condition::False;
    Action::Action_t onTrueAction    = Action::None;
    Action::Action_t onFalseAction   = Action::None;
};

struct Config_t {
    std::vector<Trigger_t> trigger_list;
};

struct InterpreterMap_t {
    std::unordered_map<Condition::Condition_t, std::function<bool(void)>> conditionMap;
    std::unordered_map<Condition::Condition_t, const char*> conditionDescriptionMap;
    std::unordered_map<Action::Action_t, std::function<void(void)>> actionMap;
    std::unordered_map<Action::Action_t, const char*> actionDescriptionMap;
};
}  // namespace TriggerRelay
