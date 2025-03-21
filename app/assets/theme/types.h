/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                   Colors                                   */
/* -------------------------------------------------------------------------- */
struct ColorPool_t {
    /* ----------------------- Add your theme define here ----------------------- */

    struct Misc_t {
        uint32_t bgPopFatalError = 0x0078d7;
        uint32_t bgPopWarning    = 0xFE8B00;
        uint32_t bgPopError      = 0xF45050;
        uint32_t bgPopSuccess    = 0x009653;
    };
    Misc_t Misc;
};
