/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include "../utils/daemon_control/daemon_control.h"
#include <mooncake.h>

class TimerRelayDaemonControl_t : public DaemonControl_t {
public:
    TimerRelay::TimerRelayConfig_t config;
};
static TimerRelayDaemonControl_t* _daemon_control = nullptr;

static void _timer_relay_daemon(void* param)
{
    spdlog::info("start timer relay with: id: {} on: {} off: {}", _daemon_control->config.relayId,
                 _daemon_control->config.onInterval, _daemon_control->config.offInterval);

    bool output_state = true;
    HAL::SetPlcRelayState(_daemon_control->config.relayId, output_state);
    uint32_t time_count = HAL::Millis();

    while (1) {
        // On 2 off
        if (output_state) {
            if (HAL::Millis() - time_count > _daemon_control->config.onInterval) {
                output_state = false;
                HAL::SetPlcRelayState(_daemon_control->config.relayId, output_state);
                time_count = HAL::Millis();
            }
        }
        // Off 2 on
        else {
            if (HAL::Millis() - time_count > _daemon_control->config.offInterval) {
                output_state = true;
                HAL::SetPlcRelayState(_daemon_control->config.relayId, output_state);
                time_count = HAL::Millis();
            }
        }

        vTaskDelay(pdTICKS_TO_MS(50));

        if (_daemon_control->CheckKillSignal()) {
            break;
        }
    }

    HAL::SetPlcRelayState(_daemon_control->config.relayId, false);

    _daemon_control->DaemonGone();
    vTaskDelete(NULL);
}

bool HAL_StamPLC::startTimerRelay(TimerRelay::TimerRelayConfig_t config)
{
    if (isTimerRelayRunning()) {
        spdlog::warn("timer relay is running");
        return false;
    }

    _daemon_control         = new TimerRelayDaemonControl_t;
    _daemon_control->config = config;
    xTaskCreate(_timer_relay_daemon, "tmr", 4000, NULL, 10, NULL);
    return true;
}

bool HAL_StamPLC::isTimerRelayRunning()
{
    return _daemon_control != nullptr;
}

bool HAL_StamPLC::stopTimerRelay()
{
    if (!isTimerRelayRunning()) {
        spdlog::warn("timer relay is not running");
        return false;
    }

    _daemon_control->SendKillSignalAndWait();
    delay(20);

    delete _daemon_control;
    _daemon_control = nullptr;
    return true;
}
