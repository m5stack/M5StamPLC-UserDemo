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
 * @brief AppTemplate
 *
 */
class AppTemplate : public APP_BASE {
public:
    void onResume() override;
    void onRunning() override;
    void onDestroy() override;

private:
    struct Data_t {
        std::uint32_t time_count = 0;
    };
    Data_t _data;
};

class AppTemplate_Packer : public APP_PACKER_BASE {
    const char* getAppName() override;
    void* getAppIcon() override;
    void* newApp() override
    {
        return new AppTemplate;
    }
    void deleteApp(void* app) override
    {
        delete (AppTemplate*)app;
    }
    void* getCustomData() override;
};
}  // namespace APPS
}  // namespace MOONCAKE
