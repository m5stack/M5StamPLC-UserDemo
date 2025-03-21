/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "wear_levelling.h"

/* Wear levelling and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This sample shows how to store files inside a FAT filesystem.
   FAT filesystem is stored in a partition inside SPI flash, using the
   flash wear levelling library.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"

#define EXAMPLE_MAX_CHAR_SIZE 128

static const char* TAG = "fs";

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char* base_path = "/spiflash";

uint8_t fs_init()
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4, .format_if_mount_failed = true, .allocation_unit_size = CONFIG_WL_SECTOR_SIZE};
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        // return;
        return false;
    }
    return true;
}

uint8_t fs_deinit()
{
    // Unmount FATFS
    ESP_LOGI(TAG, "Unmounting FAT filesystem");
    esp_err_t err = esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(err));
        return false;
    }
    return true;
}

uint8_t fs_format()
{
    // Format FATFS
    esp_err_t err = esp_vfs_fat_spiflash_format_rw_wl(base_path, "storage");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(err));
        return false;
    }
    return true;
}

const char* fs_get_base_path()
{
    return base_path;
}

void* fs_get_wl_handler()
{
    return (void*)&s_wl_handle;
}
