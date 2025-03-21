/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>

void HAL_StamPLC::_speaker_init()
{
    spdlog::info("speaker init");

    // Should be the first time of arduino api call
    initArduino();
}

void HAL_StamPLC::beep(float frequency, uint32_t duration)
{
    if (!_data.config.beepOn) {
        return;
    }
    tone(HAL_PIN_BUZZ, frequency, duration);
}

void HAL_StamPLC::beepStop()
{
    noTone(HAL_PIN_BUZZ);
}
