/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <stdint.h>

void status_light_init(int pin);
void status_light_set(uint32_t red, uint32_t green, uint32_t blue);
