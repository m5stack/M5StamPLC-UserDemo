/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "status_light.h"
#include <led_strip.h>

static led_strip_handle_t led_strip;

void status_light_init(int pin)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num   = pin,                   // The GPIO that connected to the LED strip's data line
        .max_leds         = 1,                     // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,  // Pixel format of your LED strip
        .led_model        = LED_MODEL_SK6812,      // LED strip model
        .flags.invert_out =
            false,  // whether to invert the output signal (useful when your hardware has a level inverter)
    };

    led_strip_rmt_config_t rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
#else
        .clk_src        = RMT_CLK_SRC_DEFAULT,  // different clock source can lead to different power consumption
        .resolution_hz  = 10 * 1000 * 1000,     // 10MHz
        .flags.with_dma = false,                // whether to enable the DMA feature
#endif
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
}

void status_light_set(uint32_t red, uint32_t green, uint32_t blue)
{
    led_strip_set_pixel(led_strip, 0, red, green, blue);
    led_strip_refresh(led_strip);
}
