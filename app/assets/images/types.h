/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                   Images                                   */
/* -------------------------------------------------------------------------- */
struct ImagePool_t {
    /* ----------------------- Add your image define here ----------------------- */
    struct AppLauncher_t {
        // image size: 70 x 70, array length: 4900
        uint16_t icon_app_dashboard[4900];
        uint16_t icon_app_log_monitor[4900];
        uint16_t icon_app_ezdata[4900];
        uint16_t icon_app_timer_relay[4900];
        uint16_t icon_app_trigger_relay[4900];
        uint16_t icon_app_setting[4900];
    };
    AppLauncher_t AppLauncher;

    struct AppDashboard_t {
        // image size: 15x15, array length: 225
        uint16_t icon_ezdata_not_ok[225];
        // image size: 15 x 15, array length: 225
        uint16_t icon_ezdata_ok[225];
        // image size: 15 x 15, array length: 225
        uint16_t icon_wifi_not_ok[225];
        // image size: 15 x 15, array length: 225
        uint16_t icon_wifi_ok[225];
        // image size: 35 x 17, array length: 595
        uint16_t tag_console[595];
        // image size: 155 x 35, array length: 5425
        uint16_t tag_io[5425];
    };
    AppDashboard_t AppDashboard;

    struct AppTriggerRelay_t {
        // image size: 18 x 18, array length: 324
        uint16_t icon_save[324];
        // image size: 18 x 18, array length: 324
        uint16_t icon_load[324];
    };
    AppTriggerRelay_t AppTriggerRelay;
};
