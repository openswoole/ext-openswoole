/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

#define OSW_LOG_BUFFER_SIZE (OSW_ERROR_MSG_SIZE + 256)
#define OSW_LOG_DATE_STRLEN 128
#define OSW_LOG_DEFAULT_DATE_FORMAT "%F %T"

enum swLogLevel {
    OSW_LOG_DEBUG = 0,
    OSW_LOG_TRACE,
    OSW_LOG_INFO,
    OSW_LOG_NOTICE,
    OSW_LOG_WARNING,
    OSW_LOG_ERROR,
    OSW_LOG_NONE,
};

enum swLogRotationType {
    OSW_LOG_ROTATION_SINGLE = 0,
    OSW_LOG_ROTATION_MONTHLY,
    OSW_LOG_ROTATION_DAILY,
    OSW_LOG_ROTATION_HOURLY,
    OSW_LOG_ROTATION_EVERY_MINUTE,
};

namespace openswoole {
class Logger {
  private:
    bool opened = false;
    // Redirect stdin and stdout to log_fd
    bool redirected = false;
    bool display_backtrace_ = false;
    int stdout_fd = -1;
    int stderr_fd = -1;
    int log_fd = STDOUT_FILENO;
    int log_level = OSW_LOG_INFO;
    bool date_with_microseconds = false;
    std::string date_format = OSW_LOG_DEFAULT_DATE_FORMAT;
    std::string log_file = "";
    std::string log_real_file;
    int log_rotation = OSW_LOG_ROTATION_SINGLE;

  public:
    bool open(const char *logfile);
    void put(int level, const char *content, size_t length);
    void reopen();
    void close(void);
    void reset();
    void set_level(int lv);
    int get_level();
    bool set_date_format(const char *format);
    void set_rotation(int rotation);
    const char *get_real_file();
    const char *get_file();
    bool is_opened();
    bool redirect_stdout_and_stderr(int enable);
    void set_date_with_microseconds(bool enable);
    std::string gen_real_file(const std::string &file);
    static std::string get_pretty_name(const std::string &prettyFunction, bool strip = true);

    void display_backtrace() {
        display_backtrace_ = true;
    }
};
}  // namespace openswoole

openswoole::Logger *osw_logger();
#define __SW_FUNC__ (openswoole::Logger::get_pretty_name(__PRETTY_FUNCTION__).c_str())

#define openswoole_info(str, ...)                                                                                          \
    if (OSW_LOG_INFO >= osw_logger()->get_level()) {                                                                     \
        size_t _sw_error_len = osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, str, ##__VA_ARGS__);                           \
        osw_logger()->put(OSW_LOG_INFO, osw_error, _sw_error_len);                                                        \
    }

#define openswoole_notice(str, ...)                                                                                        \
    if (OSW_LOG_NOTICE >= osw_logger()->get_level()) {                                                                   \
        size_t _sw_error_len = osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, str, ##__VA_ARGS__);                           \
        osw_logger()->put(OSW_LOG_NOTICE, osw_error, _sw_error_len);                                                      \
    }

#define openswoole_sys_notice(str, ...)                                                                                    \
    do {                                                                                                               \
        openswoole_set_last_error(errno);                                                                                  \
        if (OSW_LOG_ERROR >= osw_logger()->get_level()) {                                                                \
            size_t _sw_error_len = osw_snprintf(osw_error,                                                               \
                                               OSW_ERROR_MSG_SIZE,                                                      \
                                               "%s(:%d): " str ", Error: %s[%d]",                                      \
                                               __SW_FUNC__,                                                            \
                                               __LINE__,                                                               \
                                               ##__VA_ARGS__,                                                          \
                                               openswoole_strerror(errno),                                                 \
                                               errno);                                                                 \
            osw_logger()->put(OSW_LOG_NOTICE, osw_error, _sw_error_len);                                                  \
        }                                                                                                              \
    } while (0)

#define openswoole_warning(str, ...)                                                                                       \
    do {                                                                                                               \
        if (OSW_LOG_WARNING >= osw_logger()->get_level()) {                                                              \
            size_t _sw_error_len = osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, "%s(): " str, __SW_FUNC__, ##__VA_ARGS__); \
            osw_logger()->put(OSW_LOG_WARNING, osw_error, _sw_error_len);                                                 \
        }                                                                                                              \
    } while (0)

#define openswoole_sys_warning(str, ...)                                                                                   \
    do {                                                                                                               \
        openswoole_set_last_error(errno);                                                                                  \
        if (OSW_LOG_ERROR >= osw_logger()->get_level()) {                                                                \
            size_t _sw_error_len = osw_snprintf(osw_error,                                                               \
                                               OSW_ERROR_MSG_SIZE,                                                      \
                                               "%s(): " str ", Error: %s[%d]",                                         \
                                               __SW_FUNC__,                                                            \
                                               ##__VA_ARGS__,                                                          \
                                               openswoole_strerror(errno),                                                 \
                                               errno);                                                                 \
            osw_logger()->put(OSW_LOG_WARNING, osw_error, _sw_error_len);                                                 \
        }                                                                                                              \
    } while (0)

#define openswoole_error(str, ...)                                                                                         \
    do {                                                                                                               \
        size_t _sw_error_len = osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, str, ##__VA_ARGS__);                           \
        osw_logger()->put(OSW_LOG_ERROR, osw_error, _sw_error_len);                                                       \
        exit(1);                                                                                                       \
    } while (0)

#define openswoole_sys_error(str, ...)                                                                                     \
    do {                                                                                                               \
        size_t _sw_error_len = osw_snprintf(osw_error,                                                                   \
                                           OSW_ERROR_MSG_SIZE,                                                          \
                                           "%s(): " str ", Error: %s[%d]",                                             \
                                           __SW_FUNC__,                                                                \
                                           ##__VA_ARGS__,                                                              \
                                           openswoole_strerror(errno),                                                     \
                                           errno);                                                                     \
        osw_logger()->put(OSW_LOG_ERROR, osw_error, _sw_error_len);                                                       \
        exit(1);                                                                                                       \
    } while (0)

#define openswoole_fatal_error(code, str, ...)                                                                             \
    do {                                                                                                               \
        OpenSwooleG.fatal_error(code, str, ##__VA_ARGS__);                                                                 \
        exit(255);                                                                                                     \
    } while (0)

#define openswoole_error_log(level, error, str, ...)                                                                       \
    do {                                                                                                               \
        openswoole_set_last_error(error);                                                                                  \
        if (level >= osw_logger()->get_level()) {                                                                       \
            size_t _sw_error_len =                                                                                     \
                osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, "%s() (ERRNO %d): " str, __SW_FUNC__, error, ##__VA_ARGS__);  \
            osw_logger()->put(level, osw_error, _sw_error_len);                                                          \
        }                                                                                                              \
    } while (0)

#ifdef OSW_DEBUG
#define openswoole_debug(str, ...)                                                                                         \
    if (OSW_LOG_DEBUG >= osw_logger()->get_level()) {                                                                    \
        size_t _sw_error_len =                                                                                         \
            osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, "%s(:%d): " str, __SW_FUNC__, __LINE__, ##__VA_ARGS__);           \
        osw_logger()->put(OSW_LOG_DEBUG, osw_error, _sw_error_len);                                                       \
    }

#define openswoole_hex_dump(data, length)                                                                                  \
    do {                                                                                                               \
        const char *__data = (data);                                                                                   \
        size_t __length = (length);                                                                                    \
        openswoole_debug("+----------+------------+-----------+-----------+------------+------------------+");             \
        for (size_t of = 0; of < __length; of += 16) {                                                                 \
            char hex[16 * 3 + 1];                                                                                      \
            char str[16 + 1];                                                                                          \
            size_t i, hof = 0, sof = 0;                                                                                \
            for (i = of; i < of + 16 && i < __length; i++) {                                                           \
                hof += sprintf(hex + hof, "%02x ", (__data)[i] & 0xff);                                                \
                sof += sprintf(str + sof, "%c", isprint((int) (__data)[i]) ? (__data)[i] : '.');                       \
            }                                                                                                          \
            openswoole_debug("| %08x | %-48s| %-16s |", of, hex, str);                                                     \
        }                                                                                                              \
        openswoole_debug("+----------+------------+-----------+-----------+------------+------------------+");             \
    } while (0)
#else
#define openswoole_debug(str, ...)
#define openswoole_hex_dump(data, length)
#endif

enum swTrace_type {
    /**
     * Server
     */
    OSW_TRACE_SERVER = 1u << 1,
    OSW_TRACE_CLIENT = 1u << 2,
    OSW_TRACE_BUFFER = 1u << 3,
    OSW_TRACE_CONN = 1u << 4,
    OSW_TRACE_EVENT = 1u << 5,
    OSW_TRACE_WORKER = 1u << 6,
    OSW_TRACE_MEMORY = 1u << 7,
    OSW_TRACE_REACTOR = 1u << 8,
    OSW_TRACE_PHP = 1u << 9,
    OSW_TRACE_HTTP = 1u << 10,
    OSW_TRACE_HTTP2 = 1u << 11,
    OSW_TRACE_EOF_PROTOCOL = 1u << 12,
    OSW_TRACE_LENGTH_PROTOCOL = 1u << 13,
    OSW_TRACE_CLOSE = 1u << 14,
    OSW_TRACE_WEBSOCKET = 1u << 15,
    /**
     * Client
     */
    OSW_TRACE_REDIS_CLIENT = 1u << 16,
    OSW_TRACE_MYSQL_CLIENT = 1u << 17,
    OSW_TRACE_HTTP_CLIENT = 1u << 18,
    OSW_TRACE_AIO = 1u << 19,
    OSW_TRACE_SSL = 1u << 20,
    OSW_TRACE_NORMAL = 1u << 21,
    /**
     * Coroutine
     */
    OSW_TRACE_CHANNEL = 1u << 22,
    OSW_TRACE_TIMER = 1u << 23,
    OSW_TRACE_SOCKET = 1u << 24,
    OSW_TRACE_COROUTINE = 1u << 25,
    OSW_TRACE_CONTEXT = 1u << 26,
    OSW_TRACE_CO_HTTP_SERVER = 1u << 27,
    OSW_TRACE_TABLE = 1u << 28,
    OSW_TRACE_CO_CURL = 1u << 29,
    OSW_TRACE_CARES = 1u << 30,

    OSW_TRACE_ALL = 0x7fffffffffffffff
};

#ifdef OSW_LOG_TRACE_OPEN
#define openswoole_trace_log(what, str, ...)                                                                               \
    if (OSW_LOG_TRACE >= osw_logger()->get_level() && (what & OpenSwooleG.trace_flags)) {                                    \
        size_t _sw_error_len =                                                                                         \
            osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, "%s(:%d): " str, __SW_FUNC__, __LINE__, ##__VA_ARGS__);           \
        osw_logger()->put(OSW_LOG_TRACE, osw_error, _sw_error_len);                                                       \
    }
#else
#define openswoole_trace_log(what, str, ...)
#endif

#define openswoole_trace(str, ...) openswoole_trace_log(OSW_TRACE_NORMAL, str, ##__VA_ARGS__)
