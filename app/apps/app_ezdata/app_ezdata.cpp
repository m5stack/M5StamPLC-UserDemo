/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_ezdata.h"
#include <cstdint>
#include <mutex>
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../../shared/shared.h"
#include "../utils/system/system.h"
#include "../utils/qrcode/qrcode.h"
#include "../app_setting/view/setting_page.h"
#include "apps/utils/system/inputs/button/button.h"
#include "core/math/math.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;

static const std::vector<std::string> _option_list = {"Monitor Link", "Setup WiFi Config", "Info",
                                                      "Clear Monitoring User"};
static std::unique_ptr<SettingPage> _page;

static void _render_setting_page()
{
    if (_page) {
        _page->stop_input_reading = true;
        _page->push_canvas        = false;
        _page->update(HAL::Millis());
        _page->stop_input_reading = false;
        _page->push_canvas        = true;
    }
}

// App name
const char* AppEzdata_Packer::getAppName()
{
    return "EzData";
}

// Theme color
constexpr static uint32_t _theme_color = 0x9DB6F7;
void* AppEzdata_Packer::getCustomData()
{
    return (void*)(&_theme_color);
}

// Icon
void* AppEzdata_Packer::getAppIcon()
{
    return (void*)AssetPool::GetImage().AppLauncher.icon_app_ezdata;
}

// Like setup()...
void AppEzdata::onResume()
{
    spdlog::info("{} onResume", getAppName());

    HAL::GetCanvas()->fillScreen(_theme_color);

    _page                     = std::make_unique<SettingPage>();
    _page->theme().background = _theme_color;
    _page->theme().optionText = SmoothUIToolKit::BlendColorInDifference(_theme_color, 0xB5B5B5);
    _page->theme().selector   = SmoothUIToolKit::BlendColorInDifference(_theme_color, 0x2D2D2D);
    _page->init(_option_list, 0);
}

// Like loop()...
void AppEzdata::onRunning()
{
    _page->update(HAL::Millis());

    if (_page->isQuiting()) {
        destroyApp();
    }

    else if (_page->was_selected) {
        _page->was_selected = false;
        auto selected_index = _page->getSelectedOptionIndex();
        // spdlog::info("selected: {}", selected_index);

        if (selected_index == 0) {
            handle_status();
        } else if (selected_index == 1) {
            handle_wifi_config();
        } else if (selected_index == 2) {
            handle_info();
        } else if (selected_index == 3) {
            handle_clear_user();
        }
    }
}

void AppEzdata::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    _page.reset();
}

void AppEzdata::handle_status()
{
    spdlog::info("handle status");

    if (!HAL::IsWifiConnected()) {
        HAL::PopWarning("WiFi is disconnected");
        return;
    }

    if (!HAL::IsEzdataConnected()) {
        HAL::PopWarning("Ezdata is disconnected");
        return;
    }

    if (!HAL::IsEadataTokenValid()) {
        HAL::PopWarning("Ezdata token is invalid");
        return;
    }

    // Display status url code
    HAL::GetCanvas()->fillScreen(_theme_color);
    {
        spdlog::info("qr_text: {}", HAL::GetEzdataStatusUrl());
        QRCODE::RenderQRCode(13, 16, HAL::GetEzdataStatusUrl().c_str(), 110, 0x180142, _theme_color);
    }
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString("Open", 182, 48);
    HAL::GetCanvas()->drawString("Monitor", 182, 68);
    HAL::GetCanvas()->drawString("Link", 182, 88);
    HAL::CanvasUpdate();

    while (true) {
        HAL::Delay(100);
        HAL::FeedTheDog();

        Button::Update();
        if (Button::Ok()->wasClicked() || Button::Back()->wasClicked() || Button::Next()->wasClicked()) {
            break;
        }
    }
}

void AppEzdata::handle_info()
{
    spdlog::info("handle info");

    AssetPool::LoadFont16(HAL::GetCanvas());

    int status_update_count = 0;
    while (1) {
        if (status_update_count == 0) {
            HAL::GetCanvas()->fillScreen(_theme_color);

            HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
            HAL::GetCanvas()->setTextDatum(middle_left);
            HAL::GetCanvas()->drawString("EzData:", 18, 17);
            HAL::GetCanvas()->drawString("WiFi:", 18, 50);

            HAL::GetCanvas()->setTextColor((uint32_t)0x4C498A);
            HAL::GetCanvas()->setTextDatum(middle_right);
            HAL::GetCanvas()->drawString(HAL::IsEzdataConnected() ? "Connected" : "Disconnected", 222, 17);
            HAL::GetCanvas()->drawString(HAL::GetWifiCurrentStatus().c_str(), 222, 50);
            if (HAL::GetSystemConfig().wifiSsid.empty()) {
                HAL::GetCanvas()->drawString("WiFi Not Config!", 222, 85);
            } else {
                HAL::GetCanvas()->drawString(fmt::format("{}", HAL::GetSystemConfig().wifiSsid).c_str(), 222, 81);
                HAL::GetCanvas()->drawString(fmt::format("{}", HAL::GetSystemConfig().wifiPassword).c_str(), 222, 112);
            }

            HAL::GetCanvas()->fillRect(26, 76, 2, 51, (uint32_t)0x4C498A);
            HAL::CanvasUpdate();
        }
        status_update_count++;
        if (status_update_count > (500 / 100)) {
            status_update_count = 0;
        }

        HAL::Delay(100);
        HAL::FeedTheDog();

        Button::Update();
        if (Button::Back()->wasClicked()) {
            break;
        }
    }
}

void AppEzdata::handle_wifi_config()
{
    spdlog::info("handle wifi config");

    // Start wifi config ap server
    std::mutex ap_event_mutex;
    std::string ap_event;
    HAL::StartWifiConfigApServer([&](const std::string& event) {
        std::lock_guard<std::mutex> lock(ap_event_mutex);
        ap_event = event;
    });

    // Display wifi qr code
    HAL::GetCanvas()->fillScreen(_theme_color);
    {
        auto qr_text = fmt::format("WIFI:T:nopass;S:{};;", HAL::GetWifiConfigApSsid());
        spdlog::info("qr_text: {}", qr_text);
        QRCODE::RenderQRCode(9, 9, qr_text.c_str(), 110, 0x180142, _theme_color);
    }
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString("Connect", 182, 48);
    HAL::GetCanvas()->drawString("WiFi Config", 182, 68);
    HAL::GetCanvas()->drawString("AP", 182, 88);
    HAL::CanvasUpdate();

    // Wait client connect
    while (true) {
        HAL::Delay(100);
        HAL::FeedTheDog();

        Button::Update();
        if (Button::Ok()->wasClicked()) {
            break;
        }

        std::lock_guard<std::mutex> lock(ap_event_mutex);
        if (!ap_event.empty()) {
            spdlog::info("ap event: {}", ap_event);
            ap_event.clear();
            break;
        }
    }

    // Display wifi config link qr code
    HAL::GetCanvas()->fillScreen(_theme_color);
    {
        auto qr_text = HAL::GetWifiConfigLink();
        spdlog::info("qr_text: {}", qr_text);
        QRCODE::RenderQRCode(15, 17, qr_text.c_str(), 110, 0x180142, _theme_color);
    }
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString("Open", 182, 48);
    HAL::GetCanvas()->drawString("WiFi Config", 182, 68);
    HAL::GetCanvas()->drawString("Link", 182, 88);
    HAL::CanvasUpdate();

    // Wait config done
    while (true) {
        HAL::Delay(100);
        HAL::FeedTheDog();

        Button::Update();
        if (Button::Ok()->wasClicked()) {
            break;
        }

        std::lock_guard<std::mutex> lock(ap_event_mutex);
        if (!ap_event.empty()) {
            spdlog::info("ap event: {}", ap_event);
            ap_event.clear();
            break;
        }
    }

    // Reboot in 3s
    for (int i = 3; i > 0; i--) {
        HAL::GetCanvas()->fillScreen(_theme_color);
        HAL::GetCanvas()->drawString("Config Done", 120, 43);
        auto reboot_text = fmt::format("Reboot In {}s", i);
        HAL::GetCanvas()->drawString(reboot_text.c_str(), 120, 83);
        HAL::CanvasUpdate();

        HAL::Delay(1000);
        HAL::FeedTheDog();
    }
    HAL::Reboot();
}

void AppEzdata::handle_clear_user()
{
    spdlog::info("handle clear user");

    // Create select menu
    bool clear_user = false;
    SelectMenuPage::CreateAndWaitResult([&](SelectMenuPage::Props_t& props) {
        props.primary                  = 0xB4DCBB;
        props.onPrimary                = 0x012706;
        props.title                    = "Clear User?";
        props.onCustomRenderBackground = []() { _render_setting_page(); };
        props.onOptionSelected         = [&](int selectedIndex) {
            // Apply
            clear_user = selectedIndex == 0;
        };

        // Option list
        props.optionList.push_back("Yes");
        props.optionList.push_back("No");
    });

    if (!clear_user) {
        return;
    }

    if (HAL::GetSystemConfig().wifiSsid.empty()) {
        HAL::PopWarning("WiFi config is not set\nPlease setup WiFi first");
        return;
    }

    HAL::GetCanvas()->fillScreen(_theme_color);
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString("Clearing..", 120, 43);
    HAL::CanvasUpdate();

    bool ret = HAL::ClearEzdataMonitoringUser();
    if (!ret) {
        HAL::PopError("Clear Monitoring User\nFailed");
        return;
    }

    // Reboot in 3s
    for (int i = 3; i > 0; i--) {
        HAL::GetCanvas()->fillScreen(_theme_color);
        AssetPool::LoadFont16(HAL::GetCanvas());
        HAL::GetCanvas()->setTextColor((uint32_t)0x180142);
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->drawString("Clear Done", 120, 43);
        auto reboot_text = fmt::format("Reboot In {}s", i);
        HAL::GetCanvas()->drawString(reboot_text.c_str(), 120, 83);
        HAL::CanvasUpdate();

        HAL::Delay(1000);
        HAL::FeedTheDog();
    }
    HAL::Reboot();
}
