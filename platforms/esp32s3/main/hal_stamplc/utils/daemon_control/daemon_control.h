/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <thread>
#include <mutex>

/**
 * @brief A class for daemon control
 *
 */
class DaemonControl_t {
public:
    struct Data_t {
        std::mutex mutex;
        bool time_2_go = false;
        bool is_gone   = false;
    };

    virtual ~DaemonControl_t()
    {
    }

    Data_t& Borrow()
    {
        _data.mutex.lock();
        return _data;
    }

    void Return()
    {
        _data.mutex.unlock();
    }

    void SendKillSignal()
    {
        Borrow().time_2_go = true;
        Return();
    }

    bool CheckKillSignal()
    {
        bool ret = Borrow().time_2_go;
        Return();
        return ret;
    }

    void DaemonGone()
    {
        Borrow().is_gone = true;
        Return();
    }

    bool DaemonIsKilled()
    {
        bool ret = Borrow().is_gone;
        Return();
        return ret;
    }

    void SendKillSignalAndWait(const TickType_t xTicksToDelay = pdMS_TO_TICKS(100))
    {
        SendKillSignal();
        while (1) {
            vTaskDelay(xTicksToDelay);
            if (DaemonIsKilled()) break;
        }
    }

protected:
    Data_t _data;
};
