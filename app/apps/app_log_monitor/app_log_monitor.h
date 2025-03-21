/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <mooncake.h>
#include "view/menu_bar.h"

namespace MOONCAKE {
namespace APPS {
/**
 * @brief AppLogMonitor
 *
 */
class AppLogMonitor : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        VIEW::MenuBar menu_bar;
        std::uint32_t time_count = 0;
    };
    Data_t _data;
};

class AppLogMonitor_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppLogMonitor;
    }
    void deleteApp(void* app) override
    {
        delete (AppLogMonitor*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
