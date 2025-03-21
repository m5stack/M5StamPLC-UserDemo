/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <esp_vfs_fat.h>
#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>
// https://gist.github.com/lovyan03/7a5a9b66130e81e1cb84776b5346e0fe
// https://github.com/espressif/esp-idf/tree/v5.1.4/examples/storage/sd_card/sdspi

static sdmmc_card_t* _card = nullptr;

void HAL_StamPLC::_sd_card_init()
{
    spdlog::info("sd card init");

    esp_err_t err;

    sdspi_device_config_t device_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    device_config.host_id               = SPI2_HOST;
    device_config.gpio_cs               = (gpio_num_t)HAL_PIN_SD_CS;  // SDカードのCSピンの番号を指定
    device_config.gpio_cd               = GPIO_NUM_NC;
    device_config.gpio_wp               = GPIO_NUM_NC;
    device_config.gpio_int              = GPIO_NUM_NC;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot         = device_config.host_id;

    esp_vfs_fat_mount_config_t mount_config;
    mount_config.format_if_mount_failed   = true;
    mount_config.max_files                = 5;
    mount_config.allocation_unit_size     = 16 * 1024;
    mount_config.disk_status_check_enable = false;

    // sdmmc_card_t* card;
    err = esp_vfs_fat_sdspi_mount(SD_CARD_MOUNT_POINT, &host, &device_config, &mount_config, &_card);
    if (err != ESP_OK) {
        spdlog::error("sd card mount failed");
        _card = nullptr;
    }
    spdlog::info("sd mount ok");

    // Info and status
    if (_card != nullptr) {
        sdmmc_card_print_info(stdout, _card);
        if (isSdCardAvailable()) spdlog::info("sd card status ok");
    }

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
}

bool HAL_StamPLC::isSdCardAvailable(bool printStatus)
{
    if (_card == nullptr) {
        return false;
    }

    if (printStatus) {
        esp_err_t err = sdmmc_get_status(_card);
        if (err != ESP_OK) {
            spdlog::error("get sd card status failed");
            return false;
        }
    }

    return true;
}
