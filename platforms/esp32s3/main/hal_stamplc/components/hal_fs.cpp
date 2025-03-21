/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <string>
#include <ArduinoJson.h>
#include <dirent.h>
extern "C" {
#include "../utils/wear_levelling/wear_levelling.h"
}

void HAL_StamPLC::_fs_init()
{
    spdlog::info("fs init");

    if (!fs_init()) {
        _disp_init();
        popFatalError("Fs init failed");
    }

    loadSystemConfig();
    _log_out_system_config();
}

void HAL_StamPLC::_log_out_system_config()
{
    spdlog::info("system config:");
    spdlog::info(" - beep on: {}", _data.config.beepOn);
    spdlog::info(" - modbus addr: {}", _data.config.modbusSlaveAddress);
    spdlog::info(" - ntp time zone: {}", _data.config.ntpTimeZone);
    spdlog::info(" - wifi ssid: {}", _data.config.wifiSsid);
    spdlog::info(" - wifi password: {}", _data.config.wifiPassword);
}

/* -------------------------------------------------------------------------- */
/*                               Config realated                              */
/* -------------------------------------------------------------------------- */
static const char* _system_config_path        = "/spiflash/system_config.json";
static const char* _system_config_backup_path = "/spiflash/system_config.json.bk";

void HAL_StamPLC::_config_check_valid()
{
    spdlog::info("check config valid");

    if (_data.config.modbusSlaveAddress < 1) {
        _data.config.modbusSlaveAddress = 1;
    } else if (_data.config.modbusSlaveAddress > 128) {
        _data.config.modbusSlaveAddress = 128;
    }

    if (_data.config.ntpTimeZone < -12) {
        _data.config.ntpTimeZone = -12;
    } else if (_data.config.ntpTimeZone > 14) {
        _data.config.ntpTimeZone = 14;
    }
}

void HAL_StamPLC::loadSystemConfig()
{
    spdlog::info("load config from fs");

    // Open config
    FILE* config_file = fopen(_system_config_path, "rb");
    if (config_file == NULL) {
        spdlog::warn("{} not exist", _system_config_path);
        saveSystemConfig();
        _backup_config_file();
        return;
    }

    // Copy content
    char* file_content = 0;
    long file_length   = 0;
    fseek(config_file, 0, SEEK_END);
    file_length = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);
    file_content = (char*)malloc(file_length);

    // If error happened
    if (!file_content) {
        fclose(config_file);

        spdlog::warn("malloc failed, size: {}", file_length);
        spdlog::info("try config backup");

        config_file = fopen(_system_config_backup_path, "rb");
        if (config_file == NULL) {
            spdlog::info("open backup failed, recreate config file");
            saveSystemConfig();
            _backup_config_file();
            return;
        }
    } else {
        fread(file_content, 1, file_length, config_file);
        fclose(config_file);
    }

    // Parse
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file_content);
    if (error != DeserializationError::Ok) {
        free(file_content);
        // spdlog::error("json invaild");

        _disp_init();
        popWarning("Config JSON\nInvaild");

        saveSystemConfig();
        return;
    }

    // Copy
    _data.config.beepOn             = doc["beepOn"];
    _data.config.modbusSlaveAddress = doc["modbusSlaveAddress"];
    _data.config.ntpTimeZone        = doc["ntpTimeZone"];
    _data.config.wifiSsid           = doc["wifiSsid"].as<std::string>();
    _data.config.wifiPassword       = doc["wifiPassword"].as<std::string>();

    int locale_code = doc["localeCode"];
    if (locale_code <= (int)locale_code_jp && locale_code >= 0) {
        _data.config.localeCode = (LocaleCode_t)locale_code;
    }

    free(file_content);
    spdlog::info("done");

    _config_check_valid();
}

std::string HAL_StamPLC::_create_config_json()
{
    // Create json
    JsonDocument doc;

    _config_check_valid();

    // Copy
    if (_data.display != nullptr) {
        doc["brightness"] = _data.display->getBrightness();
    } else {
        doc["brightness"] = _data.config.brightness;
    }

    doc["beepOn"]             = _data.config.beepOn;
    doc["localeCode"]         = _data.config.localeCode;
    doc["modbusSlaveAddress"] = _data.config.modbusSlaveAddress;
    doc["ntpTimeZone"]        = _data.config.ntpTimeZone;
    doc["wifiSsid"]           = _data.config.wifiSsid;
    doc["wifiPassword"]       = _data.config.wifiPassword;

    // Serialize
    std::string json_content;
    if (serializeJson(doc, json_content) == 0) {
        _disp_init();
        popFatalError("Serialize failed");
    }

    return json_content;
}

void HAL_StamPLC::_backup_config_file()
{
    spdlog::info("create config backup");

    // Config
    spdlog::info("open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path, "rb");
    if (config_file == NULL) {
        spdlog::warn("open config failed, return");
        return;
    }

    // Backup
    spdlog::info("open {}", _system_config_backup_path);
    FILE* config_backup_file = fopen(_system_config_backup_path, "wb");
    if (config_backup_file == NULL) {
        _disp_init();
        popFatalError("Open config backup failed");
    }

    // Copy and save
    char* buffer = new char[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, 1024, config_file)) > 0) {
        fwrite(buffer, 1, bytesRead, config_backup_file);
    }
    delete[] buffer;

    fclose(config_file);
    fclose(config_backup_file);
}

void HAL_StamPLC::saveSystemConfig()
{
    spdlog::info("save config to fs");

    _backup_config_file();

    std::string json_content = _create_config_json();

    // Open file
    spdlog::info("open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path, "wb");
    if (config_file == NULL) {
        _disp_init();
        popFatalError("Open config failed");
    }

    // Write and save
    fputs(json_content.c_str(), config_file);
    fclose(config_file);

    spdlog::info("config saved: {}", _system_config_path);
}

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */
std::vector<std::string> HAL_StamPLC::_ls(const std::string& path)
{
    spdlog::info("ls {}", path);

    std::vector<std::string> result;

    auto folder = opendir(path.c_str());

    // If no directory
    if (folder == NULL) {
        return result;
    }

    // Iterate all rec files
    struct dirent* entry;
    while ((entry = readdir(folder)) != NULL) {
        // pass
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Get file name
        result.push_back(entry->d_name);
    }

    // Close folder
    closedir(folder);

    return result;
}

/* -------------------------------------------------------------------------- */
/*                                Factory reset                               */
/* -------------------------------------------------------------------------- */
void HAL_StamPLC::factoryReset(OnLogPageRenderCallback_t onLogPageRender)
{
    std::string string_buffer = "start factory reset..";
    onLogPageRender(string_buffer, true, true);
    spdlog::info(string_buffer);

    if (!fs_format()) {
        popFatalError("Factory format\nFailed");
    }

    if (!_nvs_reset()) {
        popFatalError("Nvs format\nFailed");
    }

    reboot();
}
