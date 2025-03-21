/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "view/view.h"
#include <mooncake.h>

namespace MOONCAKE {
namespace APPS {
/**
 * @brief Launcher
 *
 */
class AppLauncher : public APP_BASE {
public:
    void onCreate() override;
    void onResume() override;
    void onRunning() override;
    void onRunningBG() override;
    void onDestroy() override;
    void onPause() override;

private:
    struct Data_t {
        VIEW::LauncherView* launcher_view = nullptr;
    };
    Data_t _data;
    void _create_launcher_view();
    void _update_launcher_view();
    void _destroy_launcher_view();
};

class AppLauncher_Packer : public APP_PACKER_BASE {
    const char* getAppName() override
    {
        return "Launcher";
    }
    void* newApp() override
    {
        return new AppLauncher;
    }
    void deleteApp(void* app) override
    {
        delete (AppLauncher*)app;
    }
};
}  // namespace APPS
}  // namespace MOONCAKE
