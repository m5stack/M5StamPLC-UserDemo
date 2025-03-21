/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <smooth_ui_toolkit.h>
#include <M5GFX.h>
#include <mooncake.h>
#include <spdlog/fmt/bundled/format.h>

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */
#define APP_VERSION "V0.1"

/* -------------------------------------------------------------------------- */
/*                                   Console                                  */
/* -------------------------------------------------------------------------- */
namespace CONSOLE {
class ConsolePipe_t : public SmoothUIToolKit::RingBuffer<char, 50 * 1024> {
public:
    ConsolePipe_t() : _auto_newline(true), _is_enable(true), _clear(false), _enable_modbus_logging(false)
    {
    }

    void setAutoNewLine(bool autoNewLine)
    {
        _auto_newline = autoNewLine;
    }

    void setEnable(bool isEnable)
    {
        _is_enable = isEnable;
    }

    void clear()
    {
        SmoothUIToolKit::RingBuffer<char, 50 * 1024>::clear();
        _clear = true;
    }
    bool needClear()
    {
        return _clear;
    }
    void clearDone()
    {
        _clear = false;
    }

    bool isModbusLoggingEnable()
    {
        return _enable_modbus_logging;
    }
    void setModbusLoggingEnable(bool enable)
    {
        _enable_modbus_logging = enable;
    }

    template <typename... Args>
    void log(const char* msg, const Args&... args)
    {
        if (!_is_enable) {
            return;
        }

        // std::string formatted_msg = fmt::format(msg, args...);
        // Gcc sucks
        auto format_args          = fmt::make_format_args(args...);
        std::string formatted_msg = fmt::vformat(msg, format_args);

        spdlog::info(formatted_msg);

        // New line
        if (_auto_newline) {
            this->put('\n');
        }

        // this->put('>');
        // this->put(' ');
        for (const auto& i : formatted_msg) {
            this->put(i);
        }

        // // New line
        // if (_auto_newline)
        //     this->put('\n');
    }

private:
    bool _auto_newline;
    bool _is_enable;
    bool _clear;
    bool _enable_modbus_logging;
};
}  // namespace CONSOLE

namespace SHARED_DATA {
struct SharedData_t {
    LGFX_Sprite* console_canvas = nullptr;
    CONSOLE::ConsolePipe_t console_msg_pipe;
};
}  // namespace SHARED_DATA
