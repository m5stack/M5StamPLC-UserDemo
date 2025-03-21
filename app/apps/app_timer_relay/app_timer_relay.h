/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <mooncake.h>
#include "view/config_panel.h"

namespace MOONCAKE {
namespace APPS {
/**
 * @brief AppTimerRelay
 *
 */
class AppTimerRelay : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        VIEW::TimerRelayView::ConfigPanel config_panel;
    };
    Data_t _data;
};

class AppTimerRelay_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppTimerRelay;
    }
    void deleteApp(void* app) override
    {
        delete (AppTimerRelay*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
