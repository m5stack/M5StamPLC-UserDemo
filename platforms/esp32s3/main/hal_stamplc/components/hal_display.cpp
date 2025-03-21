/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
// Ref:  https://github.com/lovyan03/LovyanGFX/blob/master/examples/HowToUse/2_user_setting/2_user_setting.ino
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <lgfx/v1/panel/Panel_ST7789.hpp>
#include "../utils/pi4ioe5v6408/pi4ioe5v6408.h"

// #define __ENABLE_TEST

/* -------------------------------------------------------------------------- */
/*                                  Backlight                                 */
/* -------------------------------------------------------------------------- */
PI4IOE5V6408_Class* __get_io_expander_a();

static bool _is_backlight_inited = false;
class Light_StamPLC : public lgfx::ILight {
public:
    bool init(uint8_t brightness) override
    {
        spdlog::info("lcd backlight init");

        if (__get_io_expander_a() == nullptr) {
            spdlog::warn("io expander a is null");
            return true;
        }

        __get_io_expander_a()->setDirection(7, true);
        __get_io_expander_a()->setPullMode(7, false);
        __get_io_expander_a()->setHighImpedance(7, false);

        _is_backlight_inited = true;
        return true;
    }

    void setBrightness(uint8_t brightness) override
    {
        if (__get_io_expander_a() == nullptr) {
            return;
        }

        if (!_is_backlight_inited) {
            init(127);
        }

        spdlog::info("set bright to {}", brightness);
        if (brightness == 0) {
            __get_io_expander_a()->digitalWrite(7, true);
        } else {
            __get_io_expander_a()->digitalWrite(7, false);
        }
    }
};

/* -------------------------------------------------------------------------- */
/*                                    Panel                                   */
/* -------------------------------------------------------------------------- */

class LGFX_StamPLC : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    Light_StamPLC _light_instance;

public:
    LGFX_StamPLC(void)
    {
        {
            auto cfg = _bus_instance.config();

            cfg.pin_mosi   = HAL_PIN_LCD_MOSI;
            cfg.pin_miso   = HAL_PIN_LCD_MISO;
            cfg.pin_sclk   = HAL_PIN_LCD_SCLK;
            cfg.pin_dc     = HAL_PIN_LCD_DC;
            cfg.freq_write = 40000000;
            cfg.spi_host   = SPI2_HOST;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();

            cfg.invert          = true;
            cfg.pin_cs          = HAL_PIN_LCD_CS;
            cfg.pin_rst         = HAL_PIN_LCD_RST;
            cfg.pin_busy        = HAL_PIN_LCD_BUSY;
            cfg.panel_width     = 135;
            cfg.panel_height    = 240;
            cfg.offset_x        = 52;
            cfg.offset_y        = 40;
            cfg.bus_shared      = true;
            cfg.offset_rotation = 1;

            _panel_instance.config(cfg);
        }
        {  // バックライト制御の設定を行います。（必要なければ削除）
            _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
        }

        setPanel(&_panel_instance);
    }
};

void HAL_StamPLC::_disp_init()
{
    spdlog::info("display init");

    if (_data.display != nullptr) {
        spdlog::warn("already inited");
        return;
    }

    _data.display = new LGFX_StamPLC;
    if (!_data.display->init()) {
        spdlog::error("init failed");
        popFatalError("display init failed");
    }

    _data.canvas = new LGFX_SpriteFx(_data.display);
    _data.canvas->createSprite(_data.display->width(), _data.display->height());

/* -------------------------------------------------------------------------- */
/*                                    Test                                    */
/* -------------------------------------------------------------------------- */
#ifdef __ENABLE_TEST
    _disp_test();
#endif
}

static int _file_rw_test()
{
    // 定义文件路径
    const char* filePath = "/sdcard/sss.txt";

    // 1. 打开文件以写入模式
    FILE* file = fopen(filePath, "w");
    if (file == NULL) {
        perror("无法打开文件进行写入");
        return 1;
    }

    // 2. 写入信息
    const char* text = "这是写入文件的测试信息。\n";
    fprintf(file, "%s", text);

    // 3. 关闭文件
    fclose(file);

    // 4. 再次打开文件以读取模式
    file = fopen(filePath, "r");
    if (file == NULL) {
        perror("无法打开文件进行读取");
        return 1;
    }

    // 5. 读取并输出文件内容
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("读取到的内容: %s", buffer);
    }

    // 6. 关闭文件
    fclose(file);

    return 0;
}

void HAL_StamPLC::_disp_test()
{
    while (1) {
        spdlog::info("rrr");
        _data.display->fillScreen(TFT_RED);
        spdlog::info("-----------------");
        spdlog::info("sd status: {}", isSdCardAvailable());

        feedTheDog();
        delay(1000);

        spdlog::info("ggg");
        _data.display->fillScreen(TFT_GREEN);
        spdlog::info("-----------------");
        spdlog::info("sd status: {}", isSdCardAvailable());
        feedTheDog();
        delay(1000);

        _file_rw_test();
        feedTheDog();

        spdlog::info("bbb");
        _data.display->fillScreen(TFT_BLUE);
        spdlog::info("-----------------");
        spdlog::info("sd status: {}", isSdCardAvailable());
        feedTheDog();
        delay(1000);
    }
}
