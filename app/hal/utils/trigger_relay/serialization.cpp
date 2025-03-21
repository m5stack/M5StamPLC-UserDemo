/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "trigger_relay.h"
#include <mooncake.h>
#include "spdlog/spdlog.h"
#include <ArduinoJson.h>

#define CONFIG_JSON_KEY_TRIGGER_LIST    "triggerList"
#define CONFIG_JSON_KEY_CONDITION       "condition"
#define CONFIG_JSON_KEY_ON_TRUE_ACTION  "onTrueAction"
#define CONFIG_JSON_KEY_ON_FALSE_ACTION "onFalseAction"

std::string TriggerRelay::EncodeConfigJson(const Config_t& config)
{
    std::string ret;
    JsonDocument doc;

    // Trigger list
    for (int i = 0; i < config.trigger_list.size(); i++) {
        auto trigger_doc                             = doc[CONFIG_JSON_KEY_TRIGGER_LIST][i];
        trigger_doc[CONFIG_JSON_KEY_CONDITION]       = (int)config.trigger_list[i].condition;
        trigger_doc[CONFIG_JSON_KEY_ON_TRUE_ACTION]  = (int)config.trigger_list[i].onTrueAction;
        trigger_doc[CONFIG_JSON_KEY_ON_FALSE_ACTION] = (int)config.trigger_list[i].onFalseAction;
    }

    serializeJson(doc, ret);
    // serializeJsonPretty(doc, ret);
    spdlog::info("encode config json get:\n{}", ret);

    return ret;
}

TriggerRelay::Config_t TriggerRelay::ParseConfigJson(const std::string& configJson)
{
    TriggerRelay::Config_t ret;

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configJson);
    if (error) {
        spdlog::error("parse config: {} failed: {}", configJson, error.c_str());
        return ret;
    }

    // Trigger list
    JsonArray trigger_list_doc = doc[CONFIG_JSON_KEY_TRIGGER_LIST];
    for (auto i : trigger_list_doc) {
        TriggerRelay::Trigger_t trigger;
        trigger.condition     = (TriggerRelay::Condition::Condition_t)(i[CONFIG_JSON_KEY_CONDITION].as<int>());
        trigger.onTrueAction  = (TriggerRelay::Action::Action_t)(i[CONFIG_JSON_KEY_ON_TRUE_ACTION].as<int>());
        trigger.onFalseAction = (TriggerRelay::Action::Action_t)i[CONFIG_JSON_KEY_ON_FALSE_ACTION].as<int>();

        ret.trigger_list.push_back(trigger);
    }

    return ret;
}
