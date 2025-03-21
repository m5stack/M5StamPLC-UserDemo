/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <mooncake.h>

namespace MOONCAKE {
namespace APPS {
/**
 * @brief AppSetting
 *
 */
class AppSetting : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

    static void RenderSettingPage();

private:
    void handle_setting_modbus_slave_addr();
    void handle_setting_ntp_time_zone();
    void handle_setting_buzzer_on_off();
    void handle_page_about();
};

class AppSetting_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppSetting;
    }
    void deleteApp(void* app) override
    {
        delete (AppSetting*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
