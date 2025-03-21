/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "assets.h"
#include "localization/types.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#ifndef ESP_PLATFORM
#include "fonts/fonts.h"
#include "images/images.h"
#include "images/rgb565_converter/rgb565_converter.h"
#include <iostream>
#include <fstream>
#endif

AssetPool* AssetPool::_asset_pool = nullptr;

AssetPool* AssetPool::Get()
{
    if (_asset_pool == nullptr) {
        _asset_pool = new AssetPool;
    }
    return _asset_pool;
}

StaticAsset_t* AssetPool::getStaticAsset()
{
    if (_data.static_asset == nullptr) {
        spdlog::error("static asset not exsit");
        return nullptr;
    }
    return _data.static_asset;
}

bool AssetPool::injectStaticAsset(StaticAsset_t* asset)
{
    if (_data.static_asset != nullptr) {
        spdlog::error("static asset already exist");
        return false;
    }

    if (asset == nullptr) {
        spdlog::error("invalid static asset ptr");
        return false;
    }

    _data.static_asset = asset;

    // Default local text map
    setLocalTextTo(_data.locale_code);

    spdlog::info("static asset injected");
    return true;
}

void AssetPool::setLocalTextTo(LocaleCode_t code)
{
    if (code == locale_code_en) {
        getStaticAsset()->Text.TextEN.getMap(_data.local_text_pool_map);
    } else if (code == locale_code_cn) {
        getStaticAsset()->Text.TextCN.getMap(_data.local_text_pool_map);
    } else if (code == locale_code_jp) {
        getStaticAsset()->Text.TextJP.getMap(_data.local_text_pool_map);
    }
}

void AssetPool::setLocaleCode(LocaleCode_t code)
{
    _data.locale_code = code;
    setLocalTextTo(_data.locale_code);
}

void AssetPool::loadFont10(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_italic_10);
}

void AssetPool::loadFont12(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_italic_12);
}

void AssetPool::loadFont14(LGFX_SpriteFx* lgfxDevice)
{
    /* ------------------- Load your font by locale code here ------------------- */

    // lgfxDevice->setTextSize(1);
    // if (_data.locale_code == locale_code_en)
    //     lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_14);
    // else
    //     ...
}

void AssetPool::loadFont16(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_italic_16);
}

void AssetPool::loadFont24(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_italic_24);
}

void AssetPool::loadFont72(LGFX_SpriteFx* lgfxDevice)
{
}

/* -------------------------------------------------------------------------- */
/*                            Static asset generate                           */
/* -------------------------------------------------------------------------- */
#ifndef ESP_PLATFORM

/**
 * @brief Copy file into target as binary
 *
 * @param filePath
 * @param target
 * @return true
 * @return false
 */
static bool _copy_file(std::string filePath, uint8_t* target)
{
    spdlog::info("try open {}", filePath);

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        spdlog::error("open failed!", filePath);
        return false;
    }
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    spdlog::info("file binary size {}", file_size);

    // Copy and go
    if (target != nullptr) {
        file.read(reinterpret_cast<char*>(target), file_size);
    }
    file.close();
    return true;
}

static bool _copy_png_image(std::string filePath, uint16_t* target)
{
    spdlog::info("try convert: {}", filePath);
    size_t output_length = 0;
    int width = 0, height = 0;

    ImageConversionError result = convertPNGToR5G6B5(filePath.c_str(), target, &output_length, &width, &height);

    if (result != ImageConversionError::Success) {
        spdlog::error("convert failed: {}", static_cast<int>(result));
        return false;
    }

    spdlog::info("ok, image size: {} x {}, array length: {}", width, height, output_length);
    return true;
}

StaticAsset_t* AssetPool::CreateStaticAsset()
{
    auto asset_pool = new StaticAsset_t;

    // Copy data
    /* -------------------------------------------------------------------------- */
    /*                                    Fonts                                   */
    /* -------------------------------------------------------------------------- */

    // Copy your font here (or set pointer)
    // std::memcpy(asset_pool->Font.montserrat_semibolditalic_24, montserrat_semibolditalic_24,
    // montserrat_semibolditalic_24_size);

    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-10.vlw",
               asset_pool->Font.montserrat_semibold_italic_10);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-12.vlw",
               asset_pool->Font.montserrat_semibold_italic_12);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-16.vlw",
               asset_pool->Font.montserrat_semibold_italic_16);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-24.vlw",
               asset_pool->Font.montserrat_semibold_italic_24);

    /* -------------------------------------------------------------------------- */
    /*                                   Images                                   */
    /* -------------------------------------------------------------------------- */

    // Copy your image here (or set pointer)
    // std::memcpy(asset_pool->Image.AppLauncher.icon, image_data_icon, image_data_icon_size);

    _copy_png_image("../../app/assets/images/app_dashboard/icon_ezdata_not_ok.png",
                    asset_pool->Image.AppDashboard.icon_ezdata_not_ok);
    _copy_png_image("../../app/assets/images/app_dashboard/icon_ezdata_ok.png",
                    asset_pool->Image.AppDashboard.icon_ezdata_ok);
    _copy_png_image("../../app/assets/images/app_dashboard/icon_wifi_not_ok.png",
                    asset_pool->Image.AppDashboard.icon_wifi_not_ok);
    _copy_png_image("../../app/assets/images/app_dashboard/icon_wifi_ok.png",
                    asset_pool->Image.AppDashboard.icon_wifi_ok);
    _copy_png_image("../../app/assets/images/app_dashboard/tag_console.png",
                    asset_pool->Image.AppDashboard.tag_console);
    _copy_png_image("../../app/assets/images/app_dashboard/tag_io.png", asset_pool->Image.AppDashboard.tag_io);

    _copy_png_image("../../app/assets/images/app_launcher/icon_app_dashboard.png",
                    asset_pool->Image.AppLauncher.icon_app_dashboard);
    _copy_png_image("../../app/assets/images/app_launcher/icon_app_ezdata.png",
                    asset_pool->Image.AppLauncher.icon_app_ezdata);
    _copy_png_image("../../app/assets/images/app_launcher/icon_app_log_monitor.png",
                    asset_pool->Image.AppLauncher.icon_app_log_monitor);
    _copy_png_image("../../app/assets/images/app_launcher/icon_app_setting.png",
                    asset_pool->Image.AppLauncher.icon_app_setting);
    _copy_png_image("../../app/assets/images/app_launcher/icon_app_timer_relay.png",
                    asset_pool->Image.AppLauncher.icon_app_timer_relay);
    _copy_png_image("../../app/assets/images/app_launcher/icon_app_trigger_relay.png",
                    asset_pool->Image.AppLauncher.icon_app_trigger_relay);

    _copy_png_image("../../app/assets/images/app_trigger_relay/icon_save.png",
                    asset_pool->Image.AppTriggerRelay.icon_save);
    _copy_png_image("../../app/assets/images/app_trigger_relay/icon_load.png",
                    asset_pool->Image.AppTriggerRelay.icon_load);

    _copy_file("../../app/assets/web/syscfg.html", asset_pool->WebPage.syscfg);
    _copy_file("../../app/assets/web/favicon.ico", asset_pool->WebPage.favicon);

    return asset_pool;
}

void AssetPool::CreateStaticAssetBin(StaticAsset_t* assetPool)
{
    /* -------------------------------------------------------------------------- */
    /*                                Output to bin                               */
    /* -------------------------------------------------------------------------- */
    std::string bin_path = "AssetPool.bin";

    std::ofstream outFile(bin_path, std::ios::binary);
    if (!outFile) {
        spdlog::error("open {} failed", bin_path);
    }

    outFile.write(reinterpret_cast<const char*>(assetPool), sizeof(StaticAsset_t));
    outFile.close();
    spdlog::info("output asset pool to: {}", bin_path);
}

StaticAsset_t* AssetPool::GetStaticAssetFromBin()
{
    auto asset_pool = new StaticAsset_t;

    // Read from bin
    std::string bin_path = "AssetPool.bin";

    std::ifstream inFile(bin_path, std::ios::binary);
    if (!inFile) {
        spdlog::error("open {} failed", bin_path);
    }

    inFile.read(reinterpret_cast<char*>(asset_pool), sizeof(StaticAsset_t));
    inFile.close();

    spdlog::info("load asset pool from: {}", bin_path);
    return asset_pool;
}
#endif
