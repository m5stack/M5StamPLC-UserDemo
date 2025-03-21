/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                    Fonts                                   */
/* -------------------------------------------------------------------------- */
struct FontPool_t {
    /* ------------------------ Add your font define here ----------------------- */

    // uint8_t* montserrat_semibolditalic_24 = nullptr;
    // or
    // uint8_t montserrat_semibolditalic_24[81463];

    uint8_t montserrat_semibold_italic_10[23018];
    uint8_t montserrat_semibold_italic_12[28256];
    uint8_t montserrat_semibold_italic_16[42744];
    uint8_t montserrat_semibold_italic_24[56523];
};
