/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include "../utils/daemon_control/daemon_control.h"
#include <shared/shared.h>
#include <assets/assets.h>
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PsychicHttp.h>

static PsychicHttpServer* _web_server = nullptr;

static std::function<void(const std::string&)> _ap_event_emit;
static bool _ap_config_done = false;

void _ap_server_task(void* param)
{
    // Wait ap client connect
    while (1) {
        if (WiFi.softAPgetStationNum() > 0) {
            _ap_event_emit("sta_connected");
            break;
        }
        vTaskDelay(100);
    }

    // Wait config done
    while (1) {
        if (_ap_config_done) {
            _ap_event_emit("config_done");
            break;
        }
        vTaskDelay(100);
    }

    vTaskDelete(NULL);
}

std::vector<std::string> _get_wifi_list()
{
    std::vector<std::string> wifi_list;

    // W (39415) wifi:sta_scan: STA is connecting, scan are not allowed!
    if (HAL::GetWifiCurrentStatus() == "connecting") {
        spdlog::error("wifi is connecting, scan are not allowed!");
        return wifi_list;
    }

    // Scan
    spdlog::info("start scan");
    int wifi_num = WiFi.scanNetworks();
    wifi_list.resize(wifi_num);
    for (int i = 0; i < wifi_num; i++) {
        wifi_list[i] = WiFi.SSID(i).c_str();
    }
    WiFi.scanDelete();
    spdlog::info("scan done");

    return wifi_list;
}

class MyChunkResponse : public PsychicResponse {
private:
    uint8_t* _src = nullptr;
    size_t _size  = 0;

public:
    MyChunkResponse(PsychicRequest* request, const String& contentType, uint8_t* src, size_t size)
        : PsychicResponse(request), _src(src), _size(size)
    {
        setContentType(contentType.c_str());
    }

    esp_err_t send()
    {
        esp_err_t err = ESP_OK;

        // just send small files directly
        if (_size < FILE_CHUNK_SIZE) {
            this->setContent(_src, _size);
            err = PsychicResponse::send();
        } else {
            /* Retrieve the pointer to scratch buffer for temporary storage */
            char* chunk = (char*)malloc(FILE_CHUNK_SIZE);
            if (chunk == NULL) {
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(this->_request->request(), HTTPD_500_INTERNAL_SERVER_ERROR,
                                    "Unable to allocate memory.");
                return ESP_FAIL;
            }

            this->sendHeaders();

            size_t chunk_index = 0;
            size_t chunk_size  = FILE_CHUNK_SIZE;
            while (1) {
                // Send chunk
                err = this->sendChunk(_src + chunk_index, chunk_size);
                if (err != ESP_OK) {
                    break;
                }
                chunk_index += chunk_size;
                if (chunk_index + FILE_CHUNK_SIZE > _size) {
                    chunk_size = _size - chunk_index;
                } else {
                    chunk_size = FILE_CHUNK_SIZE;
                }
                if (chunk_size == 0) {
                    break;
                }
            }

            // keep track of our memory
            free(chunk);

            if (err == ESP_OK) {
                ESP_LOGI(PH_TAG, "File sending complete");
                this->finishChunking();
            }
        }

        return err;
    }
};

void HAL_StamPLC::startWifiConfigApServer(std::function<void(const std::string&)> onEvent)
{
    spdlog::info("start wifi config ap server");

    spdlog::info("start ap mode");
    WiFi.disconnect();
    WiFi.softAP(getWifiConfigApSsid().c_str(), emptyString, 1, 0, 4, false);

    _web_server = new PsychicHttpServer();
    _web_server->listen(80);

    _web_server->on("/", [&](PsychicRequest* request) { return request->redirect("/syscfg"); });

    _web_server->on("/syscfg", [&](PsychicRequest* request) {
        MyChunkResponse response(request, "text/html", (uint8_t*)AssetPool::GetWebPage().syscfg,
                                 sizeof(AssetPool::GetWebPage().syscfg));
        return response.send();
    });

    _web_server->on("/favicon.ico", [&](PsychicRequest* request) {
        MyChunkResponse response(request, "image/x-icon", (uint8_t*)AssetPool::GetWebPage().favicon,
                                 sizeof(AssetPool::GetWebPage().favicon));
        return response.send();
    });

    _web_server->on("/api/set_syscfg", HTTP_POST, [&](PsychicRequest* request) {
        spdlog::info("handle set config");

        // Parse
        {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, request->body().c_str());
            if (error != DeserializationError::Ok) {
                spdlog::error("json parse failed");
                spdlog::error("get:\n{}", request->body().c_str());
                return request->reply(500, "application/json", "{\"msg\":\"json parse failed\"}");
            }

            // Copy
            std::string string_buffer;
            string_buffer = doc["wifiSsid"].as<std::string>();
            if (string_buffer != "null") {
                _data.config.wifiSsid = string_buffer;
            }
            string_buffer = doc["wifiPassword"].as<std::string>();
            if (string_buffer != "null") {
                _data.config.wifiPassword = string_buffer;
            }

            _ap_config_done = true;
        }

        saveSystemConfig();

        return request->reply(200, "application/json", "{\"msg\":\"ok\"}");
    });

    _web_server->on("/api/get_syscfg", [&](PsychicRequest* request) {
        std::string string_buffer = _create_config_json();
        return request->reply(string_buffer.c_str());
    });

    _web_server->on("/api/get_wifi_list", [&](PsychicRequest* request) {
        std::string string_buffer;

        auto wifi_list = _get_wifi_list();

        // Encode
        JsonDocument doc;
        for (int i = 0; i < wifi_list.size(); i++) {
            doc["wifiList"][i] = wifi_list[i];
        }
        serializeJson(doc, string_buffer);

        return request->reply(string_buffer.c_str());
    });

    xTaskCreate(_ap_server_task, "ap", 4096, NULL, 5, NULL);
    _ap_event_emit = onEvent;
}
