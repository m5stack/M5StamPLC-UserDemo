/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../trigger_relay.h"
#include <mooncake.h>
#include <hal/hal.h>
#include <vector>
#include "spdlog/spdlog.h"

using namespace TriggerRelay;

/* -------------------------------------------------------------------------- */
/*                                 Conditions                                 */
/* -------------------------------------------------------------------------- */
static bool _condition_false()
{
    return false;
}
static bool _condition_true()
{
    return true;
}

static bool _condition_input_1_high()
{
    return HAL::GetPlcInput(0) == true;
}
static bool _condition_input_1_low()
{
    return HAL::GetPlcInput(0) == false;
}
static bool _condition_input_2_high()
{
    return HAL::GetPlcInput(1) == true;
}
static bool _condition_input_2_low()
{
    return HAL::GetPlcInput(1) == false;
}
static bool _condition_input_3_high()
{
    return HAL::GetPlcInput(2) == true;
}
static bool _condition_input_3_low()
{
    return HAL::GetPlcInput(2) == false;
}
static bool _condition_input_4_high()
{
    return HAL::GetPlcInput(3) == true;
}
static bool _condition_input_4_low()
{
    return HAL::GetPlcInput(3) == false;
}
static bool _condition_input_5_high()
{
    return HAL::GetPlcInput(4) == true;
}
static bool _condition_input_5_low()
{
    return HAL::GetPlcInput(4) == false;
}
static bool _condition_input_6_high()
{
    return HAL::GetPlcInput(5) == true;
}
static bool _condition_input_6_low()
{
    return HAL::GetPlcInput(5) == false;
}
static bool _condition_input_7_high()
{
    return HAL::GetPlcInput(6) == true;
}
static bool _condition_input_7_low()
{
    return HAL::GetPlcInput(6) == false;
}
static bool _condition_input_8_high()
{
    return HAL::GetPlcInput(7) == true;
}
static bool _condition_input_8_low()
{
    return HAL::GetPlcInput(7) == false;
}

static void _setup_condition_mapping(InterpreterMap_t* interpreterMap)
{
    spdlog::info("create condition mapping");
    interpreterMap->conditionMap.clear();

    interpreterMap->conditionMap.emplace(Condition::False, _condition_false);
    interpreterMap->conditionMap.emplace(Condition::True, _condition_true);

    // Input
    interpreterMap->conditionMap.emplace(Condition::Input_1_High, _condition_input_1_high);
    interpreterMap->conditionMap.emplace(Condition::Input_1_Low, _condition_input_1_low);
    interpreterMap->conditionMap.emplace(Condition::Input_2_High, _condition_input_2_high);
    interpreterMap->conditionMap.emplace(Condition::Input_2_Low, _condition_input_2_low);
    interpreterMap->conditionMap.emplace(Condition::Input_3_High, _condition_input_3_high);
    interpreterMap->conditionMap.emplace(Condition::Input_3_Low, _condition_input_3_low);
    interpreterMap->conditionMap.emplace(Condition::Input_4_High, _condition_input_4_high);
    interpreterMap->conditionMap.emplace(Condition::Input_4_Low, _condition_input_4_low);
    interpreterMap->conditionMap.emplace(Condition::Input_5_High, _condition_input_5_high);
    interpreterMap->conditionMap.emplace(Condition::Input_5_Low, _condition_input_5_low);
    interpreterMap->conditionMap.emplace(Condition::Input_6_High, _condition_input_6_high);
    interpreterMap->conditionMap.emplace(Condition::Input_6_Low, _condition_input_6_low);
    interpreterMap->conditionMap.emplace(Condition::Input_7_High, _condition_input_7_high);
    interpreterMap->conditionMap.emplace(Condition::Input_7_Low, _condition_input_7_low);
    interpreterMap->conditionMap.emplace(Condition::Input_8_High, _condition_input_8_high);
    interpreterMap->conditionMap.emplace(Condition::Input_8_Low, _condition_input_8_low);
}

/* -------------------------------------------------------------------------- */
/*                                 Description                                */
/* -------------------------------------------------------------------------- */
static void _setup_condition_description_mapping(InterpreterMap_t* interpreterMap)
{
    spdlog::info("create condition description mapping");
    interpreterMap->conditionDescriptionMap.clear();

    interpreterMap->conditionDescriptionMap.emplace(Condition::False, "False");
    interpreterMap->conditionDescriptionMap.emplace(Condition::True, "True");

    // Input
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_1_High, "Input 1 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_1_Low, "Input 1 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_2_High, "Input 2 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_2_Low, "Input 2 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_3_High, "Input 3 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_3_Low, "Input 3 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_4_High, "Input 4 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_4_Low, "Input 4 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_5_High, "Input 5 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_5_Low, "Input 5 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_6_High, "Input 6 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_6_Low, "Input 6 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_7_High, "Input 7 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_7_Low, "Input 7 Low");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_8_High, "Input 8 High");
    interpreterMap->conditionDescriptionMap.emplace(Condition::Input_8_Low, "Input 8 Low");
}

void Interpreter::_create_condition_maps(InterpreterMap_t* interpreterMap)
{
    _setup_condition_mapping(interpreterMap);
    _setup_condition_description_mapping(interpreterMap);
}

/* -------------------------------------------------------------------------- */
/*                           Vector list for iterate                          */
/* -------------------------------------------------------------------------- */
static const std::vector<Condition::Condition_t> _condition_list = {
    Condition::False,        Condition::True,        Condition::Input_1_High, Condition::Input_1_Low,
    Condition::Input_2_High, Condition::Input_2_Low, Condition::Input_3_High, Condition::Input_3_Low,
    Condition::Input_4_High, Condition::Input_4_Low, Condition::Input_5_High, Condition::Input_5_Low,
    Condition::Input_6_High, Condition::Input_6_Low, Condition::Input_7_High, Condition::Input_7_Low,
    Condition::Input_8_High, Condition::Input_8_Low,
};

const std::vector<Condition::Condition_t>& TriggerRelay::GetConditionList()
{
    return _condition_list;
}
