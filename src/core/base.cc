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

#include "openswoole.h"

#include <stdarg.h>
#include <assert.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/resource.h>

#ifdef __MACH__
#include <sys/syslimits.h>
#endif

#include <algorithm>
#include <list>
#include <set>
#include <unordered_map>

#include "openswoole_api.h"
#include "openswoole_string.h"
#include "openswoole_signal.h"
#include "openswoole_memory.h"
#include "openswoole_protocol.h"
#include "openswoole_util.h"
#include "openswoole_async.h"
#include "openswoole_c_api.h"
#include "openswoole_coroutine_c_api.h"
#include "openswoole_ssl.h"

using openswoole::String;

#ifdef HAVE_GETRANDOM
#include <sys/random.h>
#else
static ssize_t getrandom(void *buffer, size_t size, unsigned int __flags) {
#ifdef HAVE_ARC4RANDOM
    arc4random_buf(buffer, size);
    return size;
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    size_t read_bytes;
    ssize_t n;
    for (read_bytes = 0; read_bytes < size; read_bytes += (size_t) n) {
        n = read(fd, (char *) buffer + read_bytes, size - read_bytes);
        if (n <= 0) {
            break;
        }
    }

    close(fd);

    return read_bytes;
#endif
}
#endif

openswoole::Global OpenSwooleG = {};
#ifdef __MACH__
thread_local openswoole::ThreadGlobal OpenSwooleTG;
#else
__thread openswoole::ThreadGlobal OpenSwooleTG;
#endif

static std::unordered_map<std::string, void *> functions;
static openswoole::Logger *g_logger_instance = nullptr;

#ifdef __MACH__
static __thread char _sw_error_buf[OSW_ERROR_MSG_SIZE];
char *osw_error_() {
    return _sw_error_buf;
}
#else
__thread char osw_error[OSW_ERROR_MSG_SIZE];
#endif

static void openswoole_fatal_error_impl(int code, const char *format, ...);

openswoole::Logger *osw_logger() {
    return g_logger_instance;
}

void *osw_malloc(size_t size) {
    return OpenSwooleG.std_allocator.malloc(size);
}

void osw_free(void *ptr) {
    return OpenSwooleG.std_allocator.free(ptr);
}

void *osw_calloc(size_t nmemb, size_t size) {
    return OpenSwooleG.std_allocator.calloc(nmemb, size);
}

void *osw_realloc(void *ptr, size_t size) {
    return OpenSwooleG.std_allocator.realloc(ptr, size);
}

static void bug_report_message_init() {
    OpenSwooleG.bug_report_message += "\n" + std::string(OPENSWOOLE_BUG_REPORT) + "\n";

    struct utsname u;
    if (uname(&u) != -1) {
        OpenSwooleG.bug_report_message +=
            openswoole::std_string::format("OS: %s %s %s %s\n", u.sysname, u.release, u.version, u.machine);
    }

#ifdef __VERSION__
    OpenSwooleG.bug_report_message += openswoole::std_string::format("GCC_VERSION: %s\n", __VERSION__);
#endif

#ifdef OSW_USE_OPENSSL
    OpenSwooleG.bug_report_message += openswoole_ssl_get_version_message();

#endif
}
void openswoole_init(void) {
    if (OpenSwooleG.init) {
        return;
    }

    OpenSwooleG = {};
    osw_memset_zero(osw_error, OSW_ERROR_MSG_SIZE);

    OpenSwooleG.running = 1;
    OpenSwooleG.init = 1;
    OpenSwooleG.std_allocator = {malloc, calloc, realloc, free};
    OpenSwooleG.fatal_error = openswoole_fatal_error_impl;
    OpenSwooleG.cpu_num = OSW_MAX(1, sysconf(_SC_NPROCESSORS_ONLN));
    OpenSwooleG.pagesize = getpagesize();

    // DNS options
    OpenSwooleG.dns_tries = 1;
    OpenSwooleG.dns_resolvconf_path = OSW_DNS_RESOLV_CONF;

    // get system uname
    uname(&OpenSwooleG.uname);
    // random seed
    srandom(time(nullptr));

    OpenSwooleG.pid = getpid();

    g_logger_instance = new openswoole::Logger;

#ifdef OSW_DEBUG
    osw_logger()->set_level(0);
    OpenSwooleG.trace_flags = 0x7fffffff;
#else
    osw_logger()->set_level(OSW_LOG_INFO);
#endif

    // init global shared memory
    OpenSwooleG.memory_pool = new openswoole::GlobalMemory(OSW_GLOBAL_MEMORY_PAGESIZE, true);
    OpenSwooleG.max_sockets = OSW_MAX_SOCKETS_DEFAULT;
    OpenSwooleG.max_concurrency = 0;
    OpenSwooleG.enable_server_token = false;
    struct rlimit rlmt;
    if (getrlimit(RLIMIT_NOFILE, &rlmt) < 0) {
        openswoole_sys_warning("getrlimit() failed");
    } else {
        OpenSwooleG.max_sockets = OSW_MAX((uint32_t) rlmt.rlim_cur, OSW_MAX_SOCKETS_DEFAULT);
        OpenSwooleG.max_sockets = OSW_MIN((uint32_t) rlmt.rlim_cur, OSW_SESSION_LIST_SIZE);
    }

    OpenSwooleTG.buffer_stack = new openswoole::String(OSW_STACK_BUFFER_SIZE);

    if (!openswoole_set_task_tmpdir(OSW_TASK_TMP_DIR)) {
        exit(4);
    }

    // init signalfd
#ifdef HAVE_SIGNALFD
    openswoole_signalfd_init();
    OpenSwooleG.use_signalfd = 1;
    OpenSwooleG.enable_signalfd = 1;
#endif

    // init bug report message
    bug_report_message_init();
}

OSW_EXTERN_C_BEGIN

OSW_API int openswoole_add_function(const char *name, void *func) {
    std::string _name(name);
    auto iter = functions.find(_name);
    if (iter != functions.end()) {
        openswoole_warning("Function '%s' has already been added", name);
        return OSW_ERR;
    } else {
        functions.emplace(std::make_pair(_name, func));
        return OSW_OK;
    }
}

OSW_API void *openswoole_get_function(const char *name, uint32_t length) {
    auto iter = functions.find(std::string(name, length));
    if (iter != functions.end()) {
        return iter->second;
    } else {
        return nullptr;
    }
}

OSW_API int openswoole_add_hook(enum swGlobalHookType type, swHookFunc func, int push_back) {
    return openswoole::hook_add(OpenSwooleG.hooks, type, func, push_back);
}

OSW_API void openswoole_call_hook(enum swGlobalHookType type, void *arg) {
    openswoole::hook_call(OpenSwooleG.hooks, type, arg);
}

OSW_API bool openswoole_isset_hook(enum swGlobalHookType type) {
    return OpenSwooleG.hooks[type] != nullptr;
}

OSW_API const char *openswoole_version(void) {
    return OPENSWOOLE_VERSION;
}

OSW_API int openswoole_version_id(void) {
    return OPENSWOOLE_VERSION_ID;
}

OSW_API int openswoole_api_version_id(void) {
    return OPENSWOOLE_API_VERSION_ID;
}

OSW_EXTERN_C_END

void openswoole_clean(void) {
    if (OpenSwooleTG.timer) {
        openswoole_timer_free();
    }
    if (OpenSwooleTG.reactor) {
        openswoole_event_free();
    }
    if (OpenSwooleG.memory_pool != nullptr) {
        delete OpenSwooleG.memory_pool;
    }
    if (g_logger_instance) {
        delete g_logger_instance;
        g_logger_instance = nullptr;
    }
    if (OpenSwooleTG.buffer_stack) {
        delete OpenSwooleTG.buffer_stack;
        OpenSwooleTG.buffer_stack = nullptr;
    }
    OpenSwooleG = {};
}

OSW_API void openswoole_set_log_level(int level) {
    if (osw_logger()) {
        osw_logger()->set_level(level);
    }
}

OSW_API void openswoole_set_trace_flags(int flags) {
    OpenSwooleG.trace_flags = flags;
}

OSW_API void openswoole_set_dns_server(const std::string &server) {
    char *_port;
    int dns_server_port = OSW_DNS_SERVER_PORT;
    char dns_server_host[32];
    strcpy(dns_server_host, server.c_str());
    if ((_port = strchr((char *) server.c_str(), ':'))) {
        dns_server_port = atoi(_port + 1);
        if (dns_server_port <= 0 || dns_server_port > 65535) {
            dns_server_port = OSW_DNS_SERVER_PORT;
        }
        dns_server_host[_port - server.c_str()] = '\0';
    }
    OpenSwooleG.dns_server_host = dns_server_host;
    OpenSwooleG.dns_server_port = dns_server_port;
}

OSW_API std::pair<std::string, int> openswoole_get_dns_server() {
    std::pair<std::string, int> result;
    if (OpenSwooleG.dns_server_host.empty()) {
        result.first = "";
        result.second = 0;
    } else {
        result.first = OpenSwooleG.dns_server_host;
        result.second = OpenSwooleG.dns_server_port;
    }
    return result;
}

bool openswoole_set_task_tmpdir(const std::string &dir) {
    if (dir.at(0) != '/') {
        openswoole_warning("wrong absolute path '%s'", dir.c_str());
        return false;
    }

    if (access(dir.c_str(), R_OK) < 0 && !openswoole_mkdir_recursive(dir)) {
        openswoole_warning("create task tmp dir(%s) failed", dir.c_str());
        return false;
    }

    osw_tg_buffer()->format("%s/" OSW_TASK_TMP_FILE, dir.c_str());
    OpenSwooleG.task_tmpfile = osw_tg_buffer()->to_std_string();

    if (OpenSwooleG.task_tmpfile.length() >= OSW_TASK_TMP_PATH_SIZE) {
        openswoole_warning("task tmp_dir is too large, the max size is '%d'", OSW_TASK_TMP_PATH_SIZE - 1);
        return false;
    }

    return true;
}

pid_t openswoole_fork(int flags) {
    if (!(flags & OSW_FORK_EXEC)) {
        if (openswoole_coroutine_is_in()) {
            openswoole_fatal_error(OSW_ERROR_OPERATION_NOT_SUPPORT, "must be forked outside the coroutine");
        }
        if (OpenSwooleTG.async_threads) {
            openswoole_trace("aio_task_num=%d, reactor=%p", OpenSwooleTG.async_threads->task_num, osw_reactor());
            openswoole_fatal_error(OSW_ERROR_OPERATION_NOT_SUPPORT,
                               "can not create server after using async file operation");
        }
    }
    if (flags & OSW_FORK_PRECHECK) {
        return 0;
    }

    pid_t pid = fork();
    if (pid == 0) {
        OpenSwooleG.pid = getpid();
        if (flags & OSW_FORK_DAEMON) {
            return pid;
        }
        /**
         * [!!!] All timers and event loops must be cleaned up after fork
         */
        if (openswoole_timer_is_available()) {
            openswoole_timer_free();
        }
        if (OpenSwooleG.memory_pool) {
            delete OpenSwooleG.memory_pool;
        }
        if (!(flags & OSW_FORK_EXEC)) {
            // reset OpenSwooleG.memory_pool
            OpenSwooleG.memory_pool = new openswoole::GlobalMemory(OSW_GLOBAL_MEMORY_PAGESIZE, true);
            // reopen log file
            osw_logger()->reopen();
            // reset eventLoop
            if (openswoole_event_is_available()) {
                openswoole_event_free();
                openswoole_trace_log(OSW_TRACE_REACTOR, "reactor has been destroyed");
            }
        } else {
            /**
             * close log fd
             */
            osw_logger()->close();
        }
        /**
         * reset signal handler
         */
        openswoole_signal_clear();
    }

    return pid;
}

#ifdef OSW_DEBUG
void openswoole_dump_ascii(const char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%u ", (unsigned) data[i]);
    }
    printf("\n");
}

void openswoole_dump_bin(const char *data, char type, size_t size) {
    int i;
    int type_size = openswoole_type_size(type);
    if (type_size <= 0) {
        return;
    }
    int n = size / type_size;

    for (i = 0; i < n; i++) {
        printf("%d,", openswoole_unpack(type, data + type_size * i));
    }
    printf("\n");
}

void openswoole_dump_hex(const char *data, size_t outlen) {
    for (size_t i = 0; i < outlen; ++i) {
        if ((i & 0x0fu) == 0) {
            printf("%08zX: ", i);
        }
        printf("%02X ", data[i]);
        if (((i + 1) & 0x0fu) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
#endif

/**
 * Recursive directory creation
 */
bool openswoole_mkdir_recursive(const std::string &dir) {
    char tmp[PATH_MAX];
    size_t i, len = dir.length();

    // PATH_MAX limit includes string trailing null character
    if (len + 1 > PATH_MAX) {
        openswoole_warning("mkdir(%s) failed. Path exceeds the limit of %d characters", dir.c_str(), PATH_MAX - 1);
        return false;
    }
    openswoole_strlcpy(tmp, dir.c_str(), PATH_MAX);

    if (dir[len - 1] != '/') {
        strcat(tmp, "/");
    }

    len = strlen(tmp);
    for (i = 1; i < len; i++) {
        if (tmp[i] == '/') {
            tmp[i] = 0;
            if (access(tmp, R_OK) != 0) {
                if (mkdir(tmp, 0755) == -1) {
                    openswoole_sys_warning("mkdir(%s) failed", tmp);
                    return -1;
                }
            }
            tmp[i] = '/';
        }
    }

    return true;
}

int openswoole_type_size(char type) {
    switch (type) {
    case 'c':
    case 'C':
        return 1;
    case 's':
    case 'S':
    case 'n':
    case 'v':
        return 2;
    case 'l':
    case 'L':
    case 'N':
    case 'V':
        return 4;
    default:
        return 0;
    }
}

char *openswoole_dec2hex(ulong_t value, int base) {
    assert(base > 1 && base < 37);

    static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char buf[(sizeof(ulong_t) << 3) + 1];
    char *ptr, *end;

    end = ptr = buf + sizeof(buf) - 1;
    *ptr = '\0';

    do {
        *--ptr = digits[value % base];
        value /= base;
    } while (ptr > buf && value);

    return osw_strndup(ptr, end - ptr);
}

ulong_t openswoole_hex2dec(const char *hex, size_t *parsed_bytes) {
    size_t value = 0;
    *parsed_bytes = 0;
    const char *p = hex;

    if (strncasecmp(hex, "0x", 2) == 0) {
        p += 2;
    }

    while (1) {
        char c = *p;
        if ((c >= '0') && (c <= '9')) {
            value = value * 16 + (c - '0');
        } else {
            c = toupper(c);
            if ((c >= 'A') && (c <= 'Z')) {
                value = value * 16 + (c - 'A') + 10;
            } else {
                break;
            }
        }
        p++;
    }
    *parsed_bytes = p - hex;
    return value;
}

#ifndef RAND_MAX
#define RAND_MAX 2147483647
#endif

int openswoole_rand(int min, int max) {
    static int _seed = 0;
    assert(max > min);

    if (_seed == 0) {
        _seed = time(nullptr);
        srand(_seed);
    }

    int _rand = rand();
    _rand = min + (int) ((double) ((double) (max) - (min) + 1.0) * ((_rand) / ((RAND_MAX) + 1.0)));
    return _rand;
}

int openswoole_system_random(int min, int max) {
    static int dev_random_fd = -1;
    char *next_random_byte;
    int bytes_to_read;
    unsigned random_value;

    assert(max > min);

    if (dev_random_fd == -1) {
        dev_random_fd = open("/dev/urandom", O_RDONLY);
        if (dev_random_fd < 0) {
            return openswoole_rand(min, max);
        }
    }

    next_random_byte = (char *) &random_value;
    bytes_to_read = sizeof(random_value);

    if (read(dev_random_fd, next_random_byte, bytes_to_read) < bytes_to_read) {
        openswoole_sys_warning("read() from /dev/urandom failed");
        return OSW_ERR;
    }
    return min + (random_value % (max - min + 1));
}

void openswoole_redirect_stdout(int new_fd) {
    if (dup2(new_fd, STDOUT_FILENO) < 0) {
        openswoole_sys_warning("dup2(STDOUT_FILENO) failed");
    }
    if (dup2(new_fd, STDERR_FILENO) < 0) {
        openswoole_sys_warning("dup2(STDERR_FILENO) failed");
    }
}

int openswoole_version_compare(const char *version1, const char *version2) {
    int result = 0;

    while (result == 0) {
        char *tail1;
        char *tail2;

        unsigned long ver1 = strtoul(version1, &tail1, 10);
        unsigned long ver2 = strtoul(version2, &tail2, 10);

        if (ver1 < ver2) {
            result = -1;
        } else if (ver1 > ver2) {
            result = +1;
        } else {
            version1 = tail1;
            version2 = tail2;
            if (*version1 == '\0' && *version2 == '\0') {
                break;
            } else if (*version1 == '\0') {
                result = -1;
            } else if (*version2 == '\0') {
                result = +1;
            } else {
                version1++;
                version2++;
            }
        }
    }
    return result;
}

/**
 * Maximum common divisor
 */
uint32_t openswoole_common_divisor(uint32_t u, uint32_t v) {
    assert(u > 0);
    assert(v > 0);
    uint32_t t;
    while (u > 0) {
        if (u < v) {
            t = u;
            u = v;
            v = t;
        }
        u = u - v;
    }
    return v;
}

/**
 * The least common multiple
 */
uint32_t openswoole_common_multiple(uint32_t u, uint32_t v) {
    assert(u > 0);
    assert(v > 0);

    uint32_t m_cup = u;
    uint32_t n_cup = v;
    int res = m_cup % n_cup;

    while (res != 0) {
        m_cup = n_cup;
        n_cup = res;
        res = m_cup % n_cup;
    }
    return u * v / n_cup;
}

size_t osw_snprintf(char *buf, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int retval = vsnprintf(buf, size, format, args);
    va_end(args);

    if (size == 0) {
        return retval;
    } else if (osw_unlikely(retval < 0)) {
        retval = 0;
        buf[0] = '\0';
    } else if (osw_unlikely(retval >= (int) size)) {
        retval = size - 1;
        buf[retval] = '\0';
    }
    return retval;
}

size_t osw_vsnprintf(char *buf, size_t size, const char *format, va_list args) {
    int retval = vsnprintf(buf, size, format, args);
    if (osw_unlikely(retval < 0)) {
        retval = 0;
        buf[0] = '\0';
    } else if (osw_unlikely(retval >= (int) size)) {
        retval = size - 1;
        buf[retval] = '\0';
    }
    return retval;
}

int openswoole_itoa(char *buf, long value) {
    long i = 0, j;
    long sign_mask;
    unsigned long nn;

    sign_mask = value >> (sizeof(long) * 8 - 1);
    nn = (value + sign_mask) ^ sign_mask;
    do {
        buf[i++] = nn % 10 + '0';
    } while (nn /= 10);

    buf[i] = '-';
    i += sign_mask & 1;
    buf[i] = '\0';

    int s_len = i;
    char swap;

    for (i = 0, j = s_len - 1; i < j; ++i, --j) {
        swap = buf[i];
        buf[i] = buf[j];
        buf[j] = swap;
    }
    buf[s_len] = 0;
    return s_len;
}

int openswoole_shell_exec(const char *command, pid_t *pid, bool get_error_stream) {
    pid_t child_pid;
    int fds[2];
    if (pipe(fds) < 0) {
        return OSW_ERR;
    }

    if ((child_pid = fork()) == -1) {
        openswoole_sys_warning("fork() failed");
        close(fds[0]);
        close(fds[1]);
        return OSW_ERR;
    }

    if (child_pid == 0) {
        close(fds[OSW_PIPE_READ]);

        if (get_error_stream) {
            if (fds[OSW_PIPE_WRITE] == fileno(stdout)) {
                dup2(fds[OSW_PIPE_WRITE], fileno(stderr));
            } else if (fds[OSW_PIPE_WRITE] == fileno(stderr)) {
                dup2(fds[OSW_PIPE_WRITE], fileno(stdout));
            } else {
                dup2(fds[OSW_PIPE_WRITE], fileno(stdout));
                dup2(fds[OSW_PIPE_WRITE], fileno(stderr));
                close(fds[OSW_PIPE_WRITE]);
            }
        } else {
            if (fds[OSW_PIPE_WRITE] != fileno(stdout)) {
                dup2(fds[OSW_PIPE_WRITE], fileno(stdout));
                close(fds[OSW_PIPE_WRITE]);
            }
        }

        execl("/bin/sh", "sh", "-c", command, nullptr);
        exit(127);
    } else {
        *pid = child_pid;
        close(fds[OSW_PIPE_WRITE]);
    }
    return fds[OSW_PIPE_READ];
}

char *openswoole_string_format(size_t n, const char *format, ...) {
    char *buf = (char *) osw_malloc(n);
    if (!buf) {
        return nullptr;
    }

    int ret;
    va_list va_list;
    va_start(va_list, format);
    ret = vsnprintf(buf, n, format, va_list);
    va_end(va_list);
    if (ret >= 0) {
        return buf;
    }
    osw_free(buf);
    return nullptr;
}

void openswoole_random_string(char *buf, size_t size) {
    static char characters[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
        'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    };
    size_t i = 0;
    for (; i < size; i++) {
        buf[i] = characters[openswoole_rand(0, sizeof(characters) - 1)];
    }
    buf[i] = '\0';
}

size_t openswoole_random_bytes(char *buf, size_t size) {
    size_t read_bytes = 0;
    ssize_t n;

    while (read_bytes < size) {
        size_t amount_to_read = size - read_bytes;
        n = getrandom(buf + read_bytes, amount_to_read, 0);
        if (n == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                break;
            }
        }
        read_bytes += (size_t) n;
    }

    return read_bytes;
}

bool openswoole_get_env(const char *name, int *value) {
    const char *e = getenv(name);
    if (!e) {
        return false;
    }
    *value = std::stoi(e);
    return true;
}

int openswoole_get_systemd_listen_fds() {
    int ret;
    if (!openswoole_get_env("LISTEN_FDS", &ret)) {
        openswoole_warning("invalid LISTEN_FDS");
        return -1;
    } else if (ret >= OSW_MAX_LISTEN_PORT) {
        openswoole_error_log(OSW_LOG_ERROR, OSW_ERROR_SERVER_TOO_MANY_LISTEN_PORT, "LISTEN_FDS is too big");
        return -1;
    }
    return ret;
}

#ifdef HAVE_BOOST_STACKTRACE
#include <boost/stacktrace.hpp>
#include <iostream>
void openswoole_print_backtrace(void) {
    std::cout << boost::stacktrace::stacktrace();
}
#elif defined(HAVE_EXECINFO)
#include <execinfo.h>
void openswoole_print_backtrace(void) {
    int size = 16;
    void *array[16];
    int stack_num = backtrace(array, size);
    char **stacktrace = backtrace_symbols(array, stack_num);
    int i;

    for (i = 0; i < stack_num; ++i) {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);
}
#else
void openswoole_print_backtrace(void) {}
#endif

static void openswoole_fatal_error_impl(int code, const char *format, ...) {
    size_t retval = 0;
    va_list args;

    retval += osw_snprintf(osw_error, OSW_ERROR_MSG_SIZE, "(ERROR %d): ", code);
    va_start(args, format);
    retval += osw_vsnprintf(osw_error + retval, OSW_ERROR_MSG_SIZE - retval, format, args);
    va_end(args);
    osw_logger()->put(OSW_LOG_ERROR, osw_error, retval);
    exit(1);
}

namespace openswoole {
//-------------------------------------------------------------------------------
size_t DataHead::dump(char *_buf, size_t _len) {
    return osw_snprintf(_buf,
                       _len,
                       "swDataHead[%p]\n"
                       "{\n"
                       "    long fd = %ld;\n"
                       "    uint32_t len = %d;\n"
                       "    int16_t reactor_id = %d;\n"
                       "    uint8_t type = %d;\n"
                       "    uint8_t flags = %d;\n"
                       "    uint16_t server_fd = %d;\n"
                       "}\n",
                       this,
                       fd,
                       len,
                       reactor_id,
                       type,
                       flags,
                       server_fd);
}

std::string dirname(const std::string &file) {
    size_t index = file.find_last_of('/');
    if (index == std::string::npos) {
        return std::string();
    } else if (index == 0) {
        return "/";
    }
    return file.substr(0, index);
}

int hook_add(void **hooks, int type, const Callback &func, int push_back) {
    if (hooks[type] == nullptr) {
        hooks[type] = new std::list<Callback>;
    }

    std::list<Callback> *l = reinterpret_cast<std::list<Callback> *>(hooks[type]);
    if (push_back) {
        l->push_back(func);
    } else {
        l->push_front(func);
    }

    return OSW_OK;
}

void hook_call(void **hooks, int type, void *arg) {
    std::list<Callback> *l = reinterpret_cast<std::list<Callback> *>(hooks[type]);
    for (auto i = l->begin(); i != l->end(); i++) {
        (*i)(arg);
    }
}

/**
 * return the first file of the intersection, in order of vec1
 */
std::string intersection(std::vector<std::string> &vec1, std::set<std::string> &vec2) {
    std::string result = "";

    std::find_if(vec1.begin(), vec1.end(), [&](std::string &str) -> bool {
        auto iter = std::find(vec2.begin(), vec2.end(), str);
        if (iter != vec2.end()) {
            result = *iter;
            return true;
        }
        return false;
    });

    return result;
}

double microtime(void) {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return (double) t.tv_sec + ((double) t.tv_usec / 1000000);
}

//-------------------------------------------------------------------------------
};  // namespace openswoole
