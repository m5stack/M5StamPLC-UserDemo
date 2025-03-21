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
/*                                   Actions                                  */
/* -------------------------------------------------------------------------- */
static void _action_none()
{
}

static void _action_relay_1_on()
{
    HAL::SetPlcRelayState(0, true);
}
static void _action_relay_1_off()
{
    HAL::SetPlcRelayState(0, false);
}
static void _action_relay_2_on()
{
    HAL::SetPlcRelayState(1, true);
}
static void _action_relay_2_off()
{
    HAL::SetPlcRelayState(1, false);
}
static void _action_relay_3_on()
{
    HAL::SetPlcRelayState(2, true);
}
static void _action_relay_3_off()
{
    HAL::SetPlcRelayState(2, false);
}
static void _action_relay_4_on()
{
    HAL::SetPlcRelayState(3, true);
}
static void _action_relay_4_off()
{
    HAL::SetPlcRelayState(3, false);
}

static void _setup_action_mapping(InterpreterMap_t* interpreterMap)
{
    spdlog::info("create action mapping");
    interpreterMap->actionMap.clear();

    interpreterMap->actionMap.emplace(Action::None, _action_none);

    // Relay
    interpreterMap->actionMap.emplace(Action::Relay_1_On, _action_relay_1_on);
    interpreterMap->actionMap.emplace(Action::Relay_1_Off, _action_relay_1_off);
    interpreterMap->actionMap.emplace(Action::Relay_2_On, _action_relay_2_on);
    interpreterMap->actionMap.emplace(Action::Relay_2_Off, _action_relay_2_off);
    interpreterMap->actionMap.emplace(Action::Relay_3_On, _action_relay_3_on);
    interpreterMap->actionMap.emplace(Action::Relay_3_Off, _action_relay_3_off);
    interpreterMap->actionMap.emplace(Action::Relay_4_On, _action_relay_4_on);
    interpreterMap->actionMap.emplace(Action::Relay_4_Off, _action_relay_4_off);
}

/* -------------------------------------------------------------------------- */
/*                                 Description                                */
/* -------------------------------------------------------------------------- */
static void _setup_action_description_mapping(InterpreterMap_t* interpreterMap)
{
    spdlog::info("create action description mapping");
    interpreterMap->actionDescriptionMap.clear();

    interpreterMap->actionDescriptionMap.emplace(Action::None, "None");

    // Relay
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_1_On, "Relay 1 ON");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_1_Off, "Relay 1 OFF");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_2_On, "Relay 2 ON");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_2_Off, "Relay 2 OFF");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_3_On, "Relay 3 ON");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_3_Off, "Relay 3 OFF");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_4_On, "Relay 4 ON");
    interpreterMap->actionDescriptionMap.emplace(Action::Relay_4_Off, "Relay 4 OFF");
}

void Interpreter::_create_action_maps(InterpreterMap_t* interpreterMap)
{
    _setup_action_mapping(interpreterMap);
    _setup_action_description_mapping(interpreterMap);
}

/* -------------------------------------------------------------------------- */
/*                           Vector list for iterate                          */
/* -------------------------------------------------------------------------- */
static const std::vector<Action::Action_t> _action_list = {
    Action::None,       Action::Relay_1_On,  Action::Relay_1_Off, Action::Relay_2_On,  Action::Relay_2_Off,
    Action::Relay_3_On, Action::Relay_3_Off, Action::Relay_4_On,  Action::Relay_4_Off,
};

const std::vector<Action::Action_t>& TriggerRelay::GetActionList()
{
    return _action_list;
}
