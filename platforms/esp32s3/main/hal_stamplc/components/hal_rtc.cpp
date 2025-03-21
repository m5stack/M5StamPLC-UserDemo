/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_stamplc.h"
#include "../hal_config.h"
#include "../utils/rx8130/rx8130.h"
#include <mooncake.h>

static RX8130_Class* _rtc = nullptr;

void HAL_StamPLC::_rtc_init()
{
    spdlog::info("rtc init");

    _rtc = new RX8130_Class();
    if (!_rtc->begin()) {
        spdlog::error("init failed!");
        delete _rtc;
        _rtc = nullptr;
    } else {
        _rtc->disableIrq();
        _rtc->clearIrqFlags();
    }

    _adjust_sys_time();
}

void HAL_StamPLC::setSystemTime(tm dateTime)
{
    // spdlog::info("set rtc time to {}.{}.{} {}:{}", dateTime.tm_year + 1900, dateTime.tm_mon + 1, dateTime.tm_mday,
    //              dateTime.tm_hour, dateTime.tm_min);

    if (_rtc == nullptr) {
        spdlog::warn("null rtc");
        return;
    }
    _rtc->setTime(&dateTime);
    // delay(20);
    // {
    //     struct tm time;
    //     _rtc->getTime(&time);
    //     spdlog::info("rtc time: {}.{}.{} {}:{}", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour,
    //                  time.tm_min);
    // }
    // _adjust_sys_time();
}

void HAL_StamPLC::_adjust_sys_time()
{
    if (_rtc == nullptr) {
        spdlog::warn("null rtc");
        return;
    }

    spdlog::info("adjust system time");

    struct tm tm;
    _rtc->getTime(&tm);
    spdlog::info("rtc time: {}.{}.{} {}:{}", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);

    struct timeval now;
    now.tv_sec  = mktime(&tm);
    now.tv_usec = 0;

    settimeofday(&now, NULL);
}
