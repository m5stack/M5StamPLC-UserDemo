/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <mooncake.h>
#include "view/trigger_relay_config_menu.h"

namespace MOONCAKE {
namespace APPS {
/**
 * @brief AppTriggerRelay
 *
 */
class AppTriggerRelay : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        VIEW::TriggerRelayView::TirrgerRelayConfigMenu config_panel;
    };
    Data_t _data;
};

class AppTriggerRelay_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppTriggerRelay;
    }
    void deleteApp(void* app) override
    {
        delete (AppTriggerRelay*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
