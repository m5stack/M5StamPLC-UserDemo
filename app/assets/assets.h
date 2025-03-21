/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include "fonts/types.h"
#include "images/types.h"
#include "theme/types.h"
#include "localization/types.h"
#include "web/types.h"
#include "../hal/types.h"
#include "../hal/hal.h"

/**
 * @brief A struct to define static binary asset
 *
 */
struct StaticAsset_t {
    FontPool_t Font;
    ImagePool_t Image;
    ColorPool_t Color;
    TextPool_t Text;
    WebPagePool_t WebPage;
};

/**
 * @brief A Class to handle static asset
 *
 */
class AssetPool {
public:
    /* -------------------------------------------------------------------------- */
    /*                                  Singleton                                 */
    /* -------------------------------------------------------------------------- */
    static AssetPool* Get();

    /* ------------------------------ Static asset ------------------------------ */
    StaticAsset_t* getStaticAsset();
    bool injectStaticAsset(StaticAsset_t* pool);

    /* ---------------------------------- Text ---------------------------------- */
    void setLocalTextTo(LocaleCode_t code);
    void setLocaleCode(LocaleCode_t code);
    LocaleCode_t getLocaleCode()
    {
        return _data.locale_code;
    }
    bool isLocaleEn()
    {
        return _data.locale_code == locale_code_en;
    }
    inline const LocalTextPoolMap_t& getText()
    {
        return _data.local_text_pool_map;
    }

    /* ---------------------------------- Font ---------------------------------- */
    void loadFont10(LGFX_SpriteFx* lgfxDevice);
    void loadFont12(LGFX_SpriteFx* lgfxDevice);
    void loadFont14(LGFX_SpriteFx* lgfxDevice);
    void loadFont16(LGFX_SpriteFx* lgfxDevice);
    void loadFont24(LGFX_SpriteFx* lgfxDevice);
    void loadFont72(LGFX_SpriteFx* lgfxDevice);

    /* ------------------------------- Static wrap ------------------------------ */
    static StaticAsset_t* GetStaticAsset()
    {
        return Get()->getStaticAsset();
    }
    static bool InjectStaticAsset(StaticAsset_t* asset)
    {
        return Get()->injectStaticAsset(asset);
    }

    static const ImagePool_t& GetImage()
    {
        return GetStaticAsset()->Image;
    }
    static const ColorPool_t& GetColor()
    {
        return GetStaticAsset()->Color;
    }
    static const TextPool_t& GetTextPool()
    {
        return GetStaticAsset()->Text;
    }
    static const LocalTextPoolMap_t& GetText()
    {
        return Get()->getText();
    }
    static const WebPagePool_t& GetWebPage()
    {
        return GetStaticAsset()->WebPage;
    }

    static void SetLocaleCode(LocaleCode_t code)
    {
        Get()->setLocaleCode(code);
    }
    static void SetLocalTextTo(LocaleCode_t code)
    {
        Get()->setLocalTextTo(code);
    }
    static LocaleCode_t GetLocaleCode()
    {
        return Get()->getLocaleCode();
    }
    static bool IsLocaleEn()
    {
        return Get()->isLocaleEn();
    }

    static void LoadFont10(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont10(lgfxDevice);
    }
    static void LoadFont12(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont12(lgfxDevice);
    }
    static void LoadFont14(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont14(lgfxDevice);
    }
    static void LoadFont16(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont16(lgfxDevice);
    }
    static void LoadFont24(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont24(lgfxDevice);
    }
    static void LoadFont72(LGFX_SpriteFx* lgfxDevice)
    {
        Get()->loadFont72(lgfxDevice);
    }

    /* -------------------------- Generate static asset ------------------------- */
    static StaticAsset_t* CreateStaticAsset();
#ifndef ESP_PLATFORM
    static void CreateStaticAssetBin(StaticAsset_t* assetPool);
    static StaticAsset_t* GetStaticAssetFromBin();
#endif

private:
    struct Data_t {
        StaticAsset_t* static_asset = nullptr;
        LocalTextPoolMap_t local_text_pool_map;
        LocaleCode_t locale_code = locale_code_en;
    };
    Data_t _data;
    static AssetPool* _asset_pool;
};
