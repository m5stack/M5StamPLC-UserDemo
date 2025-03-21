/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <mooncake.h>
#include "view/desktop.h"

namespace MOONCAKE {
namespace APPS {
/**
 * @brief AppDashboard
 *
 */
class AppDashboard : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        WidgetDesktop widget_desktop;
    };
    Data_t _data;
};

class AppDashboard_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppDashboard;
    }
    void deleteApp(void* app) override
    {
        delete (AppDashboard*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
