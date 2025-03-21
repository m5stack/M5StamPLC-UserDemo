/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_launcher.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "spdlog/spdlog.h"

using namespace MOONCAKE::APPS;
using namespace SmoothUIToolKit;

static bool _is_just_boot_up = true;

void AppLauncher::onCreate()
{
    spdlog::info("{} onCreate", getAppName());
    startApp();
    setAllowBgRunning(true);
}

void AppLauncher::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // If just boot up, open history app directly
    if (_is_just_boot_up) {
        _is_just_boot_up = false;

        // Create app
        auto history_app_index = HAL::NvsGet(NVS_KEY_APP_HISTORY);
        if (history_app_index < 0 || history_app_index > 4) {
            history_app_index = 0;
        }
        if (history_app_index == 5) {
            history_app_index = 0;
        }

        mcAppGetFramework()->createAndStartApp(mcAppGetFramework()->getInstalledAppList()[history_app_index + 1]);
        VIEW::LauncherView::SetLastSelectedOptionIndex(history_app_index);

        // Stack launcher into background
        closeApp();
        return;
    }

    _create_launcher_view();
}

void AppLauncher::onRunning()
{
    _update_launcher_view();
}

void AppLauncher::onRunningBG()
{
    // If only launcher left, treat app layer 0 as normal app
    if (mcAppGetFramework()->getCreatedAppNumByLayer(0) <= 1) {
        // Back to the game
        startApp();
    }
}

void AppLauncher::onPause()
{
    _destroy_launcher_view();
}

void AppLauncher::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
}

/* -------------------------------------------------------------------------- */
/*                                    View                                    */
/* -------------------------------------------------------------------------- */
void AppLauncher::_create_launcher_view()
{
    // Create menu
    _data.launcher_view = new VIEW::LauncherView;

    // Add app option
    for (const auto& app_packer : mcAppGetFramework()->getInstalledAppList()) {
        // Pass launcher
        if (app_packer == getAppPacker()) {
            continue;
        }

        VIEW::LauncherView::AppOptionProps_t new_app_option;

        if (app_packer->getCustomData() != nullptr) {
            new_app_option.themeColor = *(uint32_t*)app_packer->getCustomData();
        }
        new_app_option.appNameColor = BlendColorInDifference(new_app_option.themeColor, 0xB5B5B5);
        new_app_option.name         = app_packer->getAppName();
        new_app_option.icon         = (const uint16_t*)app_packer->getAppIcon();

        _data.launcher_view->addAppOption(new_app_option);
    }

    // App opened callback
    _data.launcher_view->app_open_callback = [&](const std::string& appName, int appOptionIndex) {
        spdlog::info("open app: {} {}", appName, appOptionIndex);

        if (!mcAppGetFramework()->createAndStartApp(mcAppGetFramework()->getInstalledAppByName(appName))) {
            spdlog::error("create and start app failed");
            return;
        }

        // Footprint
        if (appOptionIndex == 5) {
        } else {
            HAL::NvsSet(NVS_KEY_APP_HISTORY, appOptionIndex);
        }

        // Stack launcher into background
        closeApp();
    };

    _data.launcher_view->init();
}

void AppLauncher::_update_launcher_view()
{
    _data.launcher_view->update(HAL::Millis());
}

void AppLauncher::_destroy_launcher_view()
{
    if (_data.launcher_view != nullptr) {
        delete _data.launcher_view;
        _data.launcher_view = nullptr;
    }
}
