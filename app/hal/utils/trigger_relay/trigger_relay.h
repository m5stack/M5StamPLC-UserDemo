/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "types.h"
#include <string>

namespace TriggerRelay {
/* -------------------------------------------------------------------------- */
/*                                Serialization                               */
/* -------------------------------------------------------------------------- */
std::string EncodeConfigJson(const Config_t& config);
Config_t ParseConfigJson(const std::string& configJson);

/* -------------------------------------------------------------------------- */
/*                                     FS                                     */
/* -------------------------------------------------------------------------- */
std::string SaveConfigToFs(const std::string configJson);
inline std::string SaveConfigToFs(const Config_t& config)
{
    if (config.trigger_list.size() == 0) {
        return "";
    }
    return SaveConfigToFs(EncodeConfigJson(config));
}
std::vector<Config_t> LoadConfigsFromFs();

std::vector<int> LoadExistingConfigsIndicesFromFs();
Config_t LoadConfigFromFsByIndex(int index);
bool DeleteConfigByIndex(int index);

/* -------------------------------------------------------------------------- */
/*                                 Interpreter                                */
/* -------------------------------------------------------------------------- */
namespace Interpreter {
void _create_interpreter_maps();
void _create_condition_maps(InterpreterMap_t* interpreterMap);
void _create_action_maps(InterpreterMap_t* interpreterMap);
InterpreterMap_t* _get_interpreter_map();
}  // namespace Interpreter

const std::vector<Condition::Condition_t>& GetConditionList();
const std::vector<Action::Action_t>& GetActionList();

inline InterpreterMap_t* GetInterpreterMap()
{
    return Interpreter::_get_interpreter_map();
}
inline const char* GetConditionDescription(Condition::Condition_t condition)
{
    return GetInterpreterMap()->conditionDescriptionMap.at(condition);
}
inline const char* GetActionDescription(Action::Action_t action)
{
    return GetInterpreterMap()->actionDescriptionMap.at(action);
}

void RunConfigTriggers(const Config_t& config);
void FreeResource();
}  // namespace TriggerRelay
