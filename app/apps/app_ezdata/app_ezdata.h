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
 * @brief AppEzdata
 *
 */
class AppEzdata : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        std::uint32_t time_count = 0;
    };
    Data_t _data;

    void handle_status();
    void handle_wifi_config();
    void handle_info();
    void handle_clear_user();
};

class AppEzdata_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppEzdata;
    }
    void deleteApp(void* app) override
    {
        delete (AppEzdata*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
