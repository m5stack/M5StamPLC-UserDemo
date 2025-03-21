/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
// https://docs.espressif.com/projects/esp-idf/zh_CN/v5.1.4/esp32s3/api-reference/peripherals/twai.html
// https://github.com/espressif/esp-idf/blob/v5.1.4/examples/peripherals/twai/twai_network/twai_network_master/main/twai_network_example_master_main.c
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/twai.h>
#include <shared/shared.h>
#include "../utils/daemon_control/daemon_control.h"

void HAL_StamPLC::_can_init()
{
    spdlog::info("bus can init");

    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config =
        TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)HAL_PIN_CAN_TX, (gpio_num_t)HAL_PIN_CAN_RX, TWAI_MODE_NORMAL);
    // twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t ret;
    ret = twai_driver_install(&g_config, &t_config, &f_config);
    spdlog::info("can driver install {}", ret);

    ret = twai_start();
    spdlog::info("can start {}", ret);

    if (ret != ESP_OK) {
        popFatalError("bus can init failed");
    }

    // xTaskCreate(_bus_can_daemon, "can", 4000, NULL, 5, NULL);

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // _can_test();
}

void HAL_StamPLC::_can_test()
{
    spdlog::info("can test");

    uint32_t sending_time_count = millis();
    while (1) {
        feedTheDog();
        delay(100);

        // Send
        if (millis() - sending_time_count > 1000) {
            // Configure message to transmit
            twai_message_t message;
            message.identifier       = 0xAAAA;
            message.extd             = 1;
            message.data_length_code = 4;
            for (int i = 0; i < 4; i++) {
                message.data[i] = 0;
            }

            if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
                spdlog::info("Message queued for transmission\n");
            } else {
                spdlog::info("Failed to queue message for transmission\n");
            }

            sending_time_count = millis();
        }

        // Wait for message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
            spdlog::info("Message received\n");

            // Process received message
            if (message.extd) {
                spdlog::info("Message is in Extended Format\n");
            } else {
                spdlog::info("Message is in Standard Format\n");
            }
            spdlog::info("ID is %d\n", message.identifier);
            if (!(message.rtr)) {
                for (int i = 0; i < message.data_length_code; i++) {
                    printf("Data byte %d = %d\n", i, message.data[i]);
                }
            }
        } else {
            spdlog::info("Failed to receive message\n");
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                 Log monitor                                */
/* -------------------------------------------------------------------------- */
static DaemonControl_t* _daemon_control = nullptr;

static void _can_lm_daemon(void* param)
{
    spdlog::info("start can lm daemon");

    SharedData::BorrowData();
    SharedData::Console().clear();
    SharedData::Console().log("> Listening PORT-PwrCAN\n> Baudrate: 1MBITS ");
    SharedData::ReturnData();

    while (1) {
        vTaskDelay(pdTICKS_TO_MS(100));

        if (_daemon_control->CheckKillSignal()) {
            break;
        }

        // Wait for message to be received
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
            spdlog::info("bus can get msg, id: {}, {} format, data:", message.identifier,
                         message.extd ? "extd" : "std");

            if (!(message.rtr)) {
                SharedData::BorrowData();
                SharedData::Console().setAutoNewLine(false);
                SharedData::Console().log("\n> Get msg, id: {}, {} fmt\n> Data: ", message.identifier,
                                          message.extd ? "extd" : "std");

                for (int i = 0; i < message.data_length_code; i++) {
                    // printf("Data byte %d = %d\n", i, message.data[i]);
                    printf("%02X ", message.data[i]);
                    SharedData::Console().log("{:02X} ", message.data[i]);
                }
                printf("\n");

                SharedData::Console().setAutoNewLine(true);
                SharedData::ReturnData();

                // Send back
                if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
                    spdlog::info("can msg send back ok");
                } else {
                    spdlog::info("can msg send back failed");
                }
            }
        }
    }

    _daemon_control->DaemonGone();
    vTaskDelete(NULL);
}

void HAL_StamPLC::_create_can_lm_daemon()
{
    if (_daemon_control != nullptr) {
        spdlog::warn("can lm daemon already exist");
        return;
    }

    _daemon_control = new DaemonControl_t;
    xTaskCreate(_can_lm_daemon, "canlm", 4000, NULL, 10, NULL);
}

void HAL_StamPLC::_destroy_can_lm_daemon()
{
    if (_daemon_control == nullptr) {
        spdlog::warn("can lm daemon not exist");
        return;
    }

    _daemon_control->SendKillSignalAndWait();
    delay(20);

    delete _daemon_control;
    _daemon_control = nullptr;
}
