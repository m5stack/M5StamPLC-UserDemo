/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "app_template/app_template.h"
#include "app_dashboard/app_dashboard.h"
#include "app_launcher/app_launcher.h"
#include "app_log_monitor/app_log_monitor.h"
#include "app_ezdata/app_ezdata.h"
#include "app_timer_relay/app_timer_relay.h"
#include "app_trigger_relay/app_trigger_relay.h"
#include "app_setting/app_setting.h"
/* Header files locator (Don't remove) */

/**
 * @brief Run startup anim app
 *
 * @param mooncake
 */
inline void app_run_startup_anim(MOONCAKE::Mooncake* mooncake)
{
    /* -------------------- Install and run startup anim here ------------------- */
    // ...
}

/**
 * @brief Install and start default startup app
 *
 * @param mooncake
 */
inline void app_install_default_startup_app(MOONCAKE::Mooncake* mooncake)
{
    std::vector<MOONCAKE::APP_PACKER_BASE*> app_packers;

    app_packers.push_back(new MOONCAKE::APPS::AppLauncher_Packer);

    for (const auto& i : app_packers) {
        mooncake->installApp(i);
        mooncake->createAndStartApp(i);
    }
}

/**
 * @brief Install apps
 *
 * @param mooncake
 */
inline void app_install_apps(MOONCAKE::Mooncake* mooncake)
{
    mooncake->installApp(new MOONCAKE::APPS::AppDashboard_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppLogMonitor_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppEzdata_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppTimerRelay_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppTriggerRelay_Packer);
    mooncake->installApp(new MOONCAKE::APPS::AppSetting_Packer);
    /* Install app locator (Don't remove) */
}
