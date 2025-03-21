/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "text_pool/text_pool_map.h"
#include "text_pool/text_pool_en.h"
#include "text_pool/text_pool_cn.h"
#include "text_pool/text_pool_jp.h"

/* -------------------------------------------------------------------------- */
/*                                 How to use                                 */
/* -------------------------------------------------------------------------- */
// 1) Edit localization.csv
// 2) python ./localization_2_type.py

/* -------------------------------------------------------------------------- */
/*                                  Text pool                                 */
/* -------------------------------------------------------------------------- */
struct TextPool_t {
    TextPool_en_t TextEN;
    TextPool_cn_t TextCN;
    TextPool_jp_t TextJP;
};
