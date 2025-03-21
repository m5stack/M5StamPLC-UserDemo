/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "trigger_relay.h"
#include <mooncake.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>
#include <regex>
#include <spdlog/spdlog.h>
#include <cstring>
#include <vector>

// static constants for file name prefix and suffix
#ifdef PLATFORM_BUILD_DESKTOP
static const char* _config_dir_path = "trigger_relay_config";
#else
static const char* _config_dir_path = "/sdcard/trigger_relay_config";
#endif
static const std::string _file_prefix = "trigger_config_";
static const std::string _file_suffix = ".json";

// check if a directory exists
static bool _directory_exists(const char* path)
{
    struct stat info;
    if (stat(path, &info) != 0) {
        return false;  // directory does not exist
    }
    return (info.st_mode & S_IFDIR) != 0;  // is a directory
}

// create a directory
static bool _create_directory(const char* path)
{
    return mkdir(path, 0777) == 0 || errno == EEXIST;
}

// extract index from filename based on prefix and suffix
static int _extract_index_from_filename(const std::string& filename)
{
    std::regex pattern(_file_prefix + R"((\d{3}))" + _file_suffix);
    std::smatch match;

    if (std::regex_match(filename, match, pattern)) {
        return std::stoi(match[1]);
    }

    return -1;  // return -1 if filename does not match format
}

// get existing indices in the directory
static std::vector<int> _get_existing_indices(const char* dir_path)
{
    std::vector<int> indices;

    if (!_directory_exists(dir_path)) {
        spdlog::error("directory does not exist: {}", dir_path);
        return indices;
    }

    DIR* dir = opendir(dir_path);
    if (dir == nullptr) {
        spdlog::error("unable to open directory: {}", dir_path);
        return indices;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        int file_index       = _extract_index_from_filename(filename);
        if (file_index != -1) {
            indices.push_back(file_index);
        }
    }
    closedir(dir);

    return indices;
}

// get file content by index
static std::string _get_file_content_by_index(const char* dir_path, int index)
{
    std::ostringstream filename_stream;
    filename_stream << _file_prefix << std::setw(3) << std::setfill('0') << index << _file_suffix;
    std::string file_path = std::string(dir_path) + "/" + filename_stream.str();

    std::ifstream file(file_path);
    if (!file.is_open()) {
        spdlog::error("unable to open file: {}", file_path);
        return "";
    }

    std::ostringstream content_stream;
    content_stream << file.rdbuf();
    file.close();

    // spdlog::info("read file: {}", file_path);
    return content_stream.str();
}

// save trigger configuration content to a new file
static std::string _save_trigger_config(const std::string& content)
{
    // const char* dir_path = "/sdcard/trigger_relay_config";
    int max_file_index = -1;

    // check if directory exists, if not, create it
    if (!_directory_exists(_config_dir_path)) {
        if (!_create_directory(_config_dir_path)) {
            spdlog::error("unable to create directory: {}", _config_dir_path);
            return "";
        }
    }

    // get existing file indices in the directory
    std::vector<int> indices = _get_existing_indices(_config_dir_path);
    for (int index : indices) {
        if (index > max_file_index) {
            max_file_index = index;
        }
    }

    // generate the next file name
    int next_file_index = max_file_index + 1;
    if (next_file_index > 999) {
        spdlog::error("file count limit reached, cannot create new file");
        return "";
    }

    std::ostringstream filename_stream;
    filename_stream << _file_prefix << std::setw(3) << std::setfill('0') << next_file_index << _file_suffix;
    std::string new_file_name = filename_stream.str();
    std::string new_file_path = std::string(_config_dir_path) + "/" + new_file_name;

    // save content to the new file
    FILE* out_file = fopen(new_file_path.c_str(), "w");
    if (out_file == nullptr) {
        spdlog::error("unable to create file: {}", new_file_path);
        return "";
    }
    fprintf(out_file, "%s", content.c_str());
    fclose(out_file);

    // return the new file's path
    spdlog::info("configuration file saved: {}", new_file_path);
    return new_file_path;
}

// delete file by index
static bool _delete_file_by_index(const char* dir_path, int index)
{
    std::ostringstream filename_stream;
    filename_stream << _file_prefix << std::setw(3) << std::setfill('0') << index << _file_suffix;
    std::string file_path = std::string(dir_path) + "/" + filename_stream.str();

    // Attempt to delete the file
    if (remove(file_path.c_str()) != 0) {
        spdlog::error("unable to delete file: {}", file_path);
        return false;
    }

    spdlog::info("file deleted: {}", file_path);
    return true;
}

std::string TriggerRelay::SaveConfigToFs(const std::string configJson)
{
    return _save_trigger_config(configJson);
}

std::vector<TriggerRelay::Config_t> TriggerRelay::LoadConfigsFromFs()
{
    std::vector<TriggerRelay::Config_t> ret;

    auto config_file_index_list = _get_existing_indices(_config_dir_path);
    for (const auto& i : config_file_index_list) {
        ret.push_back(TriggerRelay::ParseConfigJson(_get_file_content_by_index(_config_dir_path, i)));
    }

    return ret;
}

std::vector<int> TriggerRelay::LoadExistingConfigsIndicesFromFs()
{
    return _get_existing_indices(_config_dir_path);
}

TriggerRelay::Config_t TriggerRelay::LoadConfigFromFsByIndex(int index)
{
    return TriggerRelay::ParseConfigJson(_get_file_content_by_index(_config_dir_path, index));
}

bool TriggerRelay::DeleteConfigByIndex(int index)
{
    return _delete_file_by_index(_config_dir_path, index);
}
