/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include "../utils/daemon_control/daemon_control.h"
#include "hal/utils/trigger_relay/trigger_relay.h"
#include "spdlog/spdlog.h"
#include <mooncake.h>

class TriggerRelayDaemonControl_t : public DaemonControl_t {
public:
    TriggerRelay::Config_t config;
};
static TriggerRelayDaemonControl_t* _daemon_control = nullptr;

static void _trigger_relay_daemon(void* param)
{
    spdlog::info("start trigger relay with {} triggers", _daemon_control->config.trigger_list.size());

    while (1) {
        TriggerRelay::RunConfigTriggers(_daemon_control->config);

        vTaskDelay(pdTICKS_TO_MS(50));

        if (_daemon_control->CheckKillSignal()) {
            break;
        }
    }

    _daemon_control->DaemonGone();
    vTaskDelete(NULL);
}

bool HAL_StamPLC::startTriggerRelay(TriggerRelay::Config_t config)
{
    if (config.trigger_list.empty()) {
        spdlog::warn("empty trigger");
        return false;
    }

    if (isTriggerRelayRunning()) {
        spdlog::warn("trigger relay is running");
        return false;
    }

    _daemon_control         = new TriggerRelayDaemonControl_t;
    _daemon_control->config = config;
    xTaskCreate(_trigger_relay_daemon, "tr", 4000, NULL, 10, NULL);
    return true;
}

bool HAL_StamPLC::isTriggerRelayRunning()
{
    return _daemon_control != nullptr;
}

bool HAL_StamPLC::stopTriggerRelay()
{
    if (!isTriggerRelayRunning()) {
        spdlog::warn("trigger relay is not running");
        return false;
    }

    _daemon_control->SendKillSignalAndWait();
    delay(20);

    spdlog::info("trigger relay stoped");

    delete _daemon_control;
    _daemon_control = nullptr;
    return true;
}
