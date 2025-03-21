/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

/**
 * @brief Pin configs
 *
 */

// Display
#define HAL_PIN_LCD_MOSI 8
#define HAL_PIN_LCD_MISO 9
#define HAL_PIN_LCD_SCLK 7
#define HAL_PIN_LCD_DC   6
#define HAL_PIN_LCD_CS   12
#define HAL_PIN_LCD_RST  3
#define HAL_PIN_LCD_BUSY -1
#define HAL_PIN_LCD_BL   -1

// SD card
#define HAL_PIN_SD_CS   10
#define HAL_PIN_SD_MOSI 8
#define HAL_PIN_SD_MISO 9
#define HAL_PIN_SD_SCK  7

// I2C
#define HAL_PIN_I2C_INTER_SDA 13
#define HAL_PIN_I2C_INTER_SCL 15
#define HAL_PIN_I2C_INTER_IRQ 14

// Grove
#define HAL_PIN_GROVE_RED_SDA  2
#define HAL_PIN_GROVE_RED_SCL  1
#define HAL_PIN_GROVE_BLUE_SDA 5
#define HAL_PIN_GROVE_BLUE_SCL 4

// Buzz
#define HAL_PIN_BUZZ 44

// 485
#define HAL_PIN_485_TX  0
#define HAL_PIN_485_RX  39
#define HAL_PIN_485_DIR 46

// CAN
#define HAL_PIN_CAN_TX 42
#define HAL_PIN_CAN_RX 43

// Alert
#define HAL_PIN_ALERT_LED 21
