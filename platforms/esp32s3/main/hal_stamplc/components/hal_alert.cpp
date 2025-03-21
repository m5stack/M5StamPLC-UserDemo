/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
extern "C" {
#include "../utils/status_light/status_light.h"
}

void HAL_StamPLC::popFatalError(std::string msg)
{
    // Init rgb
    status_light_init(HAL_PIN_ALERT_LED);

    bool is_on = false;
    while (1) {
        feedTheDog();
        spdlog::error("{}", msg);

        is_on = !is_on;
        if (is_on) {
            status_light_set(80, 0, 0);
        } else {
            status_light_set(0, 0, 0);
        }

        delay(500);
    }
}
