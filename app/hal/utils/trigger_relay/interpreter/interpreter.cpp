/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../trigger_relay.h"
#include <mooncake.h>
#include "spdlog/spdlog.h"

static TriggerRelay::InterpreterMap_t* _interpreter_map = nullptr;

void TriggerRelay::Interpreter::_create_interpreter_maps()
{
    spdlog::info("create interpreter map");

    // Create instance
    if (_interpreter_map != nullptr) {
        delete _interpreter_map;
    }
    _interpreter_map = new InterpreterMap_t;

    // Setup mapping
    _create_condition_maps(_interpreter_map);
    _create_action_maps(_interpreter_map);
}

TriggerRelay::InterpreterMap_t* TriggerRelay::Interpreter::_get_interpreter_map()
{
    // Lazy loading
    if (_interpreter_map == nullptr) {
        _create_interpreter_maps();
    }
    return _interpreter_map;
}

void TriggerRelay::RunConfigTriggers(const Config_t& config)
{
    TriggerRelay::Interpreter::_get_interpreter_map();

    // Run triggers
    for (const auto& i : config.trigger_list) {
        if (_interpreter_map->conditionMap.at(i.condition)()) {
            _interpreter_map->actionMap.at(i.onTrueAction)();
        } else {
            _interpreter_map->actionMap.at(i.onFalseAction)();
        }
    }
}

void TriggerRelay::FreeResource()
{
    if (_interpreter_map != nullptr) {
        delete _interpreter_map;
    }
}
