/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include "../utils/daemon_control/daemon_control.h"
#include <shared/shared.h>
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../utils/arduino-mqtt/src/MQTTClient.h"

class EzdataDaemonControl_t : public DaemonControl_t {
public:
    bool is_register_ok             = false;
    bool is_wifi_connected          = false;
    bool is_ezdata_connected        = false;
    std::string current_wifi_status = "not config";
};
static EzdataDaemonControl_t* _daemon_control = nullptr;
static std::string _ezdata_mqtt_token;
static std::string _ezdata_mqtt_topic_up;
static std::string _ezdata_mqtt_topic_down;
static int8_t _ntp_time_zone         = 8;
static WiFiClient* _wifi_client      = nullptr;
static MQTTClient* _mqtt_client      = nullptr;
static const char* _ezdata_mqtt_host = "uiflow2.m5stack.com";

static bool _connect_wifi()
{
    bool ret = false;

    spdlog::info("[wifi] try connect wifi");
    spdlog::info("[wifi] ssid: {}", HAL::GetSystemConfig().wifiSsid);
    spdlog::info("[wifi] pass: {}", HAL::GetSystemConfig().wifiPassword);
    WiFi.begin(HAL::GetSystemConfig().wifiSsid.c_str(), HAL::GetSystemConfig().wifiPassword.c_str());

    _daemon_control->Borrow();
    _daemon_control->current_wifi_status = "connecting";
    _daemon_control->Return();

    // Wait for the WiFi event
    uint32_t log_count = 0;
    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            spdlog::info("[wifi] wifi connected");
            spdlog::info("[wifi] ip address: {}", WiFi.localIP().toString().c_str());
            ret = true;
            break;
        }

        delay(1000);
        log_count++;
        if (log_count > 5) {
            spdlog::warn("[wifi] still trying to connect wifi..");
            spdlog::info("[wifi] ssid: {}", HAL::GetSystemConfig().wifiSsid);
            spdlog::info("[wifi] pass: {}", HAL::GetSystemConfig().wifiPassword);
            log_count = 0;
        }
    }

    _daemon_control->Borrow();
    _daemon_control->is_wifi_connected   = ret;
    _daemon_control->current_wifi_status = ret ? "connected" : "disconnected";
    _daemon_control->Return();

    return ret;
}

static void _start_ntp()
{
    const char* ntpServer        = "pool.ntp.org";
    const long gmtOffset_sec     = 3600 * _ntp_time_zone;
    const int daylightOffset_sec = 0;

    spdlog::info("[ntp] try start ntp..");

    // 配置 NTP 服务器
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

static void _update_rtc_time()
{
    // 获取当前时间
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        HAL::SetSystemTime(timeinfo);
    } else {
        spdlog::warn("[ntp] get time failed");
    }
}

static bool _check_ezdata_token()
{
    _ezdata_mqtt_token.clear();
    spdlog::info("[ezdata] check ezdata token");

    const char* api_url = "https://ezdata2.m5stack.com/api/v2/device/registerMac";
    spdlog::info("[ezdata] try get device token");
    spdlog::info("[ezdata] api url: {}", api_url);

    HTTPClient http;
    http.begin(api_url);
    http.addHeader("Content-Type", "application/json");

    String json_content;
    {
        uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
        WiFi.macAddress(mac);
        char mac_str[18] = {0};
        sprintf(mac_str, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        JsonDocument doc;
        doc["deviceType"] = "stamplc";
        doc["mac"]        = mac_str;
        serializeJson(doc, json_content);
    }
    spdlog::info("[ezdata] post json:\n{}", json_content.c_str());

    int response_code = http.POST(json_content);
    spdlog::info("[ezdata] get response code: {}", response_code);

    if (response_code <= 0) {
        return false;
    }

    String response = http.getString();
    spdlog::info("[ezdata] get response:\n{}", response.c_str());

    if (response_code != HTTP_CODE_OK) {
        return false;
    }

    JsonDocument doc;
    deserializeJson(doc, response);
    if (doc["code"].as<int>() != 200) {
        spdlog::error("invalid code: {}", doc["code"].as<int>());
        return false;
    }
    if (doc["data"].isNull()) {
        spdlog::error("empty token");
        return false;
    }
    _ezdata_mqtt_token = doc["data"].as<std::string>();
    spdlog::info("[ezdata] get token: {}", _ezdata_mqtt_token);

    http.end();
    return true;
}

static bool _connect_ezdata_mqtt()
{
    bool ret = false;
    spdlog::info("[ezdata] try connect ezdata mqtt..");

    // Get client id
    std::string client_id = "m5";
    {
        uint8_t mac[6]   = {0};
        char mac_str[18] = {0};
        WiFi.macAddress(mac);
        sprintf(mac_str, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        client_id += mac_str;
    }
    client_id += "m5";

    spdlog::info("[ezdata] username: {}", _ezdata_mqtt_token);
    spdlog::info("[ezdata] client_id: {}", client_id);

    // Try connect
    spdlog::info("[ezdata] try connect mqtt..");
    _mqtt_client->begin(_ezdata_mqtt_host, *_wifi_client);
    while (1) {
        if (_mqtt_client->connect(client_id.c_str(), _ezdata_mqtt_token.c_str())) {
            spdlog::info("[ezdata] mqtt connected");
            ret = true;
            break;
        }
        spdlog::warn("[ezdata] mqtt connect failed, retry in 1s");
        delay(1000);
    }

    _daemon_control->Borrow();
    _daemon_control->is_ezdata_connected = ret;
    _daemon_control->Return();

    return ret;
}

static void _handle_ctrl_msg(JsonDocument& doc)
{
    spdlog::info("[ezdata] handle ctrl msg");

    if (doc["method"].isNull()) {
        // Relays
        if (!doc["relay"].isNull()) {
            for (int i = 0; i < doc["relay"].size(); i++) {
                if (i > 3) {
                    break;
                }
                HAL::SetPlcRelayState(i, doc["relay"][i].as<bool>());
            }
        }

        // Status light
        if (!doc["statusLight"].isNull()) {
            HAL::SetStatusLightHex(doc["statusLight"].as<std::string>());
        }
    }

    // Relays
    if (!doc["value"]["relay"].isNull()) {
        for (int i = 0; i < doc["value"]["relay"].size(); i++) {
            if (i > 3) {
                break;
            }
            HAL::SetPlcRelayState(i, doc["value"]["relay"][i].as<bool>());
        }
    }

    // Status light
    if (!doc["value"]["statusLight"].isNull()) {
        // Convert hex string to rgb
        uint8_t r, g, b;
        std::sscanf(doc["value"]["statusLight"].as<std::string>().c_str(), "#%2hhx%2hhx%2hhx", &r, &g, &b);
        HAL::SetStatusLight(r, g, b);
    }
}

static bool _is_first_msg   = true;
static bool _was_controlled = false;
static void _on_mqtt_msg(String& topic, String& payload)
{
    spdlog::info("[ezdata] <<< on mqtt msg:\ntopic: {}\npayload: {}\n", topic.c_str(), payload.c_str());

    JsonDocument doc;
    if (deserializeJson(doc, payload) != DeserializationError::Ok) {
        spdlog::warn("[ezdata] parse mqtt msg json failed");
        return;
    }

    if (_is_first_msg) {
        _is_first_msg = false;
        spdlog::info("[ezdata] first msg");
        _handle_ctrl_msg(doc);
    } else {
        if (!doc["value"]["lastModifiedBy"].isNull()) {
            if (doc["value"]["lastModifiedBy"].as<std::string>() != "stamplc") {
                _handle_ctrl_msg(doc);
                _was_controlled = true;
            }
        }
    }
}

static void _upload_stamplc_status()
{
    JsonDocument doc;

    // IOs
    for (int i = 0; i < 8; i++) {
        doc["input"][i] = HAL::GetPlcInput(i);
    }
    for (int i = 0; i < 4; i++) {
        doc["relay"][i] = HAL::GetPlcRelayState(i);
    }

    // Sensors
    HAL::UpdatePowerMonitor();
    HAL::UpdateTemp();
    doc["powerVoltage"] = HAL::GetPowerVoltage();
    doc["extCurrent"]   = HAL::GetExpanderOutputCurrent();
    doc["deviceTemp"]   = HAL::GetTemp();

    doc["sdCard"]      = HAL::IsSdCardAvailable(false);
    doc["statusLight"] = HAL::GetStatusLightHex();

    // Modified source for distinguish
    doc["lastModifiedBy"] = "stamplc";

    std::string status_json;
    serializeJson(doc, status_json);

    // Create upload msg
    std::string upload_msg = fmt::format("set({},status,{},dict)", _ezdata_mqtt_token, status_json);
    spdlog::info("[ezdata] >>> upload msg:\n{}\n", upload_msg);
    _mqtt_client->publish(_ezdata_mqtt_topic_up.c_str(), upload_msg.c_str());
}

static bool _check_input_changed()
{
    bool is_changed                             = false;
    static std::array<bool, 8> last_input_value = {false};
    for (int i = 0; i < last_input_value.size(); i++) {
        if (HAL::GetPlcInput(i) != last_input_value[i]) {
            is_changed = true;
        }
        last_input_value[i] = HAL::GetPlcInput(i);
    }
    return is_changed;
}

static bool _setup_ezdata_mqtt()
{
    bool ret = false;

    // Get topic names
    _ezdata_mqtt_topic_up = "$ezdata/";
    _ezdata_mqtt_topic_up += _ezdata_mqtt_token;
    _ezdata_mqtt_topic_up += "/up";
    _ezdata_mqtt_topic_down = "$ezdata/";
    _ezdata_mqtt_topic_down += _ezdata_mqtt_token;
    _ezdata_mqtt_topic_down += "/down";
    spdlog::info("[ezdata] get mqtt topic names:\n{}\n{}", _ezdata_mqtt_topic_up, _ezdata_mqtt_topic_down);

    // Subscribe down topic
    _mqtt_client->subscribe(_ezdata_mqtt_topic_down.c_str());
    _mqtt_client->onMessage(_on_mqtt_msg);

    // Send get status msg
    std::string upload_msg = fmt::format("get({},status)", _ezdata_mqtt_token);
    spdlog::info("[ezdata] >>> upload msg:\n{}\n", upload_msg);
    _mqtt_client->publish(_ezdata_mqtt_topic_up.c_str(), upload_msg.c_str());

    return ret;
}

static void _ezdata_daemon(void* param)
{
    spdlog::info("ezdata daemon start");

    while (!HAL::Check()) {
        spdlog::info("wait hal valid..");
        delay(1000);
    }

    // Try connect wifi
    if (!_connect_wifi()) {
        _daemon_control->DaemonGone();
        vTaskDelete(NULL);
    }

    // NTP setup
    _start_ntp();

    // Check token
    while (!_check_ezdata_token()) {
        spdlog::error("ezdata token invalid, retry in 10s");
        delay(10000);
    }

    _wifi_client = new WiFiClient;
    _mqtt_client = new MQTTClient(4096, 512);

    uint32_t time_count              = millis();
    uint32_t input_update_time_count = millis();
    while (1) {
        // If not connected
        if (!_mqtt_client->connected()) {
            if (_connect_ezdata_mqtt()) {
                _setup_ezdata_mqtt();
                time_count = millis();
            }
        } else {
            // Upload if was controlled
            if (_was_controlled) {
                _was_controlled = false;
                _upload_stamplc_status();
                _update_rtc_time();
                time_count              = millis();
                input_update_time_count = millis();
            }

            // Upload if input changed
            if (millis() - input_update_time_count > 3000) {
                if (_check_input_changed()) {
                    spdlog::info("[ezdata] input changed, upload status");
                    _upload_stamplc_status();
                    _update_rtc_time();
                    time_count = millis();
                }
                input_update_time_count = millis();
            }

            // Upload status every 10s
            if (millis() - time_count > 10000) {
                _upload_stamplc_status();
                _update_rtc_time();
                time_count = millis();
            }
        }

        _mqtt_client->loop();

        delay(200);
    }

    vTaskDelete(NULL);
}

void HAL_StamPLC::ezdata_init()
{
    spdlog::info("ezdata init");

    _ntp_time_zone  = _data.config.ntpTimeZone;
    _daemon_control = new EzdataDaemonControl_t;

    if (_data.config.wifiSsid.empty()) {
        spdlog::warn("empty wifi config, return");
        return;
    }

    xTaskCreate(_ezdata_daemon, "ezdata", 4096, NULL, 15, NULL);
}

bool HAL_StamPLC::isWifiConnected()
{
    bool ret = false;
    if (_daemon_control == nullptr) {
        return ret;
    }
    _daemon_control->Borrow();
    ret = _daemon_control->is_wifi_connected;
    _daemon_control->Return();
    return ret;
}

bool HAL_StamPLC::isEzdataConnected()
{
    bool ret = false;
    if (_daemon_control == nullptr) {
        return ret;
    }
    _daemon_control->Borrow();
    ret = _daemon_control->is_ezdata_connected;
    _daemon_control->Return();
    return ret;
}

std::string HAL_StamPLC::getWifiCurrentStatus()
{
    std::string ret = "not config";
    if (_daemon_control == nullptr) {
        return ret;
    }
    _daemon_control->Borrow();
    ret = _daemon_control->current_wifi_status;
    _daemon_control->Return();
    return ret;
}

std::string HAL_StamPLC::getEzdataStatusUrl()
{
    return fmt::format("https://ezdata-stamplc.m5stack.com/{}", _ezdata_mqtt_token);
}

bool HAL_StamPLC::isEadataTokenValid()
{
    spdlog::info("[ezdata] check ezdata token valid");
    spdlog::info("[ezdata] token: {}", _ezdata_mqtt_token);
    if (_ezdata_mqtt_token.empty()) {
        spdlog::error("[ezdata] empty token");
        return false;
    }
    return true;
}

bool HAL_StamPLC::clearEzdataMonitoringUser()
{
    spdlog::info("[ezdata] clear ezdata monitoring user");
    const char* api_url = "https://ezdata2.m5stack.com/api/v2/device/releaseUser/";
    std::string url     = fmt::format("{}{}", api_url, _ezdata_mqtt_token);
    spdlog::info("[ezdata] api url: {}", url);

    HTTPClient http;
    http.begin(url.c_str());

    int response_code = http.POST("");
    spdlog::info("[ezdata] get response code: {}", response_code);

    if (response_code <= 0) {
        return false;
    }

    String response = http.getString();
    spdlog::info("[ezdata] get response:\n{}", response.c_str());

    if (response_code != HTTP_CODE_OK) {
        return false;
    }

    http.end();
    return true;
}
