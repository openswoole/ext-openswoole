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
  |         Twosee  <twose@qq.com>                                       |
  +----------------------------------------------------------------------+
*/

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#elif defined(ENABLE_PHP_OPENSWOOLE)
#include "php_config.h"
#endif

#ifdef OSW_USE_ASM_CONTEXT
#define USE_BOOST_V2 1
#endif

#ifdef __cplusplus
#define OSW_EXTERN_C_BEGIN extern "C" {
#define OSW_EXTERN_C_END }
#else
#define OSW_EXTERN_C_BEGIN
#define OSW_EXTERN_C_END
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _PTHREAD_PSHARED
#define _PTHREAD_PSHARED
#endif

/*--- C standard library ---*/
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sched.h> /* sched_yield() */
#include <pthread.h>

#include <sys/utsname.h>
#include <sys/time.h>

#include <memory>
#include <functional>

typedef unsigned long ulong_t;

#ifndef PRId64
#define PRId64 "lld"
#endif

#ifndef PRIu64
#define PRIu64 "llu"
#endif

#ifndef PRIx64
#define PRIx64 "llx"
#endif

#if defined(__GNUC__)
#if __GNUC__ >= 3
#define osw_inline inline __attribute__((always_inline))
#else
#define osw_inline inline
#endif
#elif defined(_MSC_VER)
#define osw_inline __forceinline
#else
#define osw_inline inline
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
#define OSW_API __attribute__((visibility("default")))
#else
#define OSW_API
#endif

#if !defined(__GNUC__) || __GNUC__ < 3
#define __builtin_expect(x, expected_value) (x)
#endif

#define osw_likely(x) __builtin_expect(!!(x), 1)
#define osw_unlikely(x) __builtin_expect(!!(x), 0)

#define OSW_START_LINE "-------------------------START----------------------------"
#define OSW_END_LINE "--------------------------END-----------------------------"
#define OSW_ECHO_RED "\e[31m%s\e[0m"
#define OSW_ECHO_GREEN "\e[32m%s\e[0m"
#define OSW_ECHO_YELLOW "\e[33m%s\e[0m"
#define OSW_ECHO_BLUE "\e[34m%s\e[0m"
#define OSW_ECHO_MAGENTA "\e[35m%s\e[0m"
#define OSW_ECHO_CYAN "\e[36m%s\e[0m"
#define OSW_ECHO_WHITE "\e[37m%s\e[0m"
#define OSW_COLOR_RED 1
#define OSW_COLOR_GREEN 2
#define OSW_COLOR_YELLOW 3
#define OSW_COLOR_BLUE 4
#define OSW_COLOR_MAGENTA 5
#define OSW_COLOR_CYAN 6
#define OSW_COLOR_WHITE 7

#define OSW_SPACE ' '
#define OSW_CRLF "\r\n"
#define OSW_CRLF_LEN 2
#define OSW_ASCII_CODE_0 64
#define OSW_ASCII_CODE_Z 106
/*----------------------------------------------------------------------------*/

#include "openswoole_version.h"
#include "openswoole_config.h"
#include "openswoole_log.h"
#include "openswoole_atomic.h"
#include "openswoole_error.h"

#define OSW_MAX(A, B) ((A) > (B) ? (A) : (B))
#define OSW_MIN(A, B) ((A) < (B) ? (A) : (B))
#define OSW_LOOP_N(n) for (decltype(n) i = 0; i < n; i++)
#define OSW_LOOP for (;;)

#ifndef MAX
#define MAX(A, B) OSW_MAX(A, B)
#endif
#ifndef MIN
#define MIN(A, B) OSW_MIN(A, B)
#endif

#define OSW_NUM_BILLION (1000 * 1000 * 1000)
#define OSW_NUM_MILLION (1000 * 1000)

#ifdef OSW_DEBUG
#define OSW_ASSERT(e) assert(e)
#define OSW_ASSERT_1BYTE(v)                                                                                             \
    do {                                                                                                               \
        size_t i = 0, n = 0;                                                                                           \
        for (; i < sizeof(v); i++) {                                                                                   \
            n += ((v >> i) & 1) ? 1 : 0;                                                                               \
        }                                                                                                              \
        assert(n == 1);                                                                                                \
    } while (0)
#else
#define OSW_ASSERT(e)
#define OSW_ASSERT_1BYTE(v)
#endif
#define OSW_START_SLEEP usleep(100000)  // sleep 1s,wait fork and pthread_create

/*-----------------------------------Memory------------------------------------*/
void *osw_malloc(size_t size);
void osw_free(void *ptr);
void *osw_calloc(size_t nmemb, size_t size);
void *osw_realloc(void *ptr, size_t size);

// Evaluates to the number of elements in 'array'
#define OSW_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define OSW_DEFAULT_ALIGNMENT sizeof(unsigned long)
#define OSW_MEM_ALIGNED_SIZE(size) OSW_MEM_ALIGNED_SIZE_EX(size, OSW_DEFAULT_ALIGNMENT)
#define OSW_MEM_ALIGNED_SIZE_EX(size, alignment) (((size) + ((alignment) -1LL)) & ~((alignment) -1LL))

/*-------------------------------Declare Struct--------------------------------*/
namespace openswoole {
class MemoryPool;
class Reactor;
class String;
class Timer;
struct TimerNode;
struct Event;
class Pipe;
namespace network {
struct Socket;
struct Address;
}  // namespace network
class AsyncThreads;
class IoUringEngine;
namespace async {
class ThreadPool;
}
struct Protocol;
struct EventData;
struct DataHead;
typedef int (*ReactorHandler)(Reactor *reactor, Event *event);
typedef std::function<void(void *)> Callback;
typedef std::function<void(Timer *, TimerNode *)> TimerCallback;
}  // namespace openswoole

typedef openswoole::Reactor oswReactor;
typedef openswoole::String oswString;
typedef openswoole::Timer oswTimer;
typedef openswoole::network::Socket oswSocket;
typedef openswoole::Protocol oswProtocol;
typedef openswoole::EventData oswEventData;
typedef openswoole::DataHead oswDataHead;
typedef openswoole::Event oswEvent;

/*----------------------------------String-------------------------------------*/

#define OSW_STRS(s) s, sizeof(s)
#define OSW_STRL(s) s, sizeof(s) - 1

#define OSW_STREQ(str, len, const_str) openswoole_streq(str, len, OSW_STRL(const_str))
#define OSW_STRCASEEQ(str, len, const_str) openswoole_strcaseeq(str, len, OSW_STRL(const_str))

/* string contain */
#define OSW_STRCT(str, len, const_sub_str) openswoole_strct(str, len, OSW_STRL(const_sub_str))
#define OSW_STRCASECT(str, len, const_sub_str) openswoole_strcasect(str, len, OSW_STRL(const_sub_str))

#if defined(OSW_USE_JEMALLOC) || defined(OSW_USE_TCMALLOC)
#define osw_strdup openswoole_strdup
#define osw_strndup openswoole_strndup
#else
#define osw_strdup strdup
#define osw_strndup strndup
#endif

/** always return less than size, zero termination  */
size_t osw_snprintf(char *buf, size_t size, const char *format, ...) __attribute__((format(printf, 3, 4)));
size_t osw_vsnprintf(char *buf, size_t size, const char *format, va_list args);

#define osw_memset_zero(s, n) memset(s, '\0', n)

static osw_inline int osw_mem_equal(const void *v1, size_t s1, const void *v2, size_t s2) {
    return s1 == s2 && memcmp(v1, v2, s2) == 0;
}

static inline size_t openswoole_strlcpy(char *dest, const char *src, size_t size) {
    const size_t len = strlen(src);
    if (size != 0) {
        const size_t n = std::min(len, size - 1);
        memcpy(dest, src, n);
        dest[n] = '\0';
    }
    return len;
}

static inline char *openswoole_strdup(const char *s) {
    size_t l = strlen(s) + 1;
    char *p = (char *) osw_malloc(l);
    if (osw_likely(p)) {
        memcpy(p, s, l);
    }
    return p;
}

static inline char *openswoole_strndup(const char *s, size_t n) {
    char *p = (char *) osw_malloc(n + 1);
    if (osw_likely(p)) {
        strncpy(p, s, n)[n] = '\0';
    }
    return p;
}

/* string equal */
static inline unsigned int openswoole_streq(const char *str1, size_t len1, const char *str2, size_t len2) {
    return (len1 == len2) && (strncmp(str1, str2, len1) == 0);
}

static inline unsigned int openswoole_strcaseeq(const char *str1, size_t len1, const char *str2, size_t len2) {
    return (len1 == len2) && (strncasecmp(str1, str2, len1) == 0);
}

static inline unsigned int openswoole_strct(const char *pstr, size_t plen, const char *sstr, size_t slen) {
    return (plen >= slen) && (strncmp(pstr, sstr, slen) == 0);
}

static inline unsigned int openswoole_strcasect(const char *pstr, size_t plen, const char *sstr, size_t slen) {
    return (plen >= slen) && (strncasecmp(pstr, sstr, slen) == 0);
}

static inline const char *openswoole_strnstr(const char *haystack,
                                         uint32_t haystack_length,
                                         const char *needle,
                                         uint32_t needle_length) {
    assert(needle_length > 0);
    uint32_t i;

    if (osw_likely(needle_length <= haystack_length)) {
        for (i = 0; i < haystack_length - needle_length + 1; i++) {
            if ((haystack[0] == needle[0]) && (0 == memcmp(haystack, needle, needle_length))) {
                return haystack;
            }
            haystack++;
        }
    }

    return NULL;
}

static inline ssize_t openswoole_strnpos(const char *haystack,
                                     uint32_t haystack_length,
                                     const char *needle,
                                     uint32_t needle_length) {
    assert(needle_length > 0);
    const char *pos;

    pos = openswoole_strnstr(haystack, haystack_length, needle, needle_length);
    return pos == NULL ? -1 : pos - haystack;
}

static inline ssize_t openswoole_strrnpos(const char *haystack, const char *needle, uint32_t length) {
    uint32_t needle_length = strlen(needle);
    assert(needle_length > 0);
    uint32_t i;
    haystack += (length - needle_length);

    for (i = length - needle_length; i > 0; i--) {
        if ((haystack[0] == needle[0]) && (0 == memcmp(haystack, needle, needle_length))) {
            return i;
        }
        haystack--;
    }

    return -1;
}

static inline void openswoole_strtolower(char *str, int length) {
    char *c, *e;

    c = str;
    e = c + length;

    while (c < e) {
        *c = tolower(*c);
        c++;
    }
}

/*--------------------------------Constants------------------------------------*/
enum oswResultCode {
    OSW_OK = 0,
    OSW_ERR = -1,
};

enum oswReturnCode {
    OSW_CONTINUE = 1,
    OSW_WAIT = 2,
    OSW_CLOSE = 3,
    OSW_ERROR = 4,
    OSW_READY = 5,
    OSW_INVALID = 6,
};

enum oswFdType {
    OSW_FD_SESSION,        // server stream session
    OSW_FD_STREAM_SERVER,  // server stream port
    OSW_FD_DGRAM_SERVER,   // server dgram port
    OSW_FD_PIPE,
    OSW_FD_STREAM,
    OSW_FD_AIO,
    /**
     * Coroutine Socket
     */
    OSW_FD_CORO_SOCKET,
    /**
     * socket poll fd [coroutine::socket_poll]
     */
    OSW_FD_CORO_POLL,
    /**
     * event waiter
     */
    OSW_FD_CORO_EVENT,
    /**
     * signalfd
     */
    OSW_FD_SIGNAL,
    OSW_FD_DNS_RESOLVER,
    OSW_FD_CARES,
    OSW_FD_IO_URING,
    /**
     * OSW_FD_USER or OSW_FD_USER+n: for custom event
     */
    OSW_FD_USER = 16,
    OSW_FD_STREAM_CLIENT,
    OSW_FD_DGRAM_CLIENT,
};

enum oswSocketFlag {
    OSW_SOCK_NONBLOCK = 1 << 2,
    OSW_SOCK_CLOEXEC = 1 << 3,
    OSW_SOCK_SSL = (1u << 9),
};

enum oswSocketType {
    OSW_SOCK_TCP = 1,
    OSW_SOCK_UDP = 2,
    OSW_SOCK_TCP6 = 3,
    OSW_SOCK_UDP6 = 4,
    OSW_SOCK_UNIX_STREAM = 5,  // unix sock stream
    OSW_SOCK_UNIX_DGRAM = 6,   // unix sock dgram
    OSW_SOCK_RAW = 7,
};

enum oswEventType {
    OSW_EVENT_NULL = 0,
    OSW_EVENT_DEAULT = 1u << 8,
    OSW_EVENT_READ = 1u << 9,
    OSW_EVENT_WRITE = 1u << 10,
    OSW_EVENT_RDWR = OSW_EVENT_READ | OSW_EVENT_WRITE,
    OSW_EVENT_ERROR = 1u << 11,
    OSW_EVENT_ONCE = 1u << 12,
};

enum oswForkType {
    OSW_FORK_SPAWN = 0,
    OSW_FORK_EXEC = 1 << 1,
    OSW_FORK_DAEMON = 1 << 2,
    OSW_FORK_PRECHECK = 1 << 3,
};

//-------------------------------------------------------------------------------
#define osw_yield() sched_yield()

//------------------------------Base--------------------------------
#ifndef uchar
typedef unsigned char uchar;
#endif

#define openswoole_tolower(c) (uchar)((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define openswoole_toupper(c) (uchar)((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

void openswoole_random_string(char *buf, size_t size);
size_t openswoole_random_bytes(char *buf, size_t size);

static osw_inline char *openswoole_strlchr(char *p, char *last, char c) {
    while (p < last) {
        if (*p == c) {
            return p;
        }
        p++;
    }
    return NULL;
}

static osw_inline size_t openswoole_size_align(size_t size, int pagesize) {
    return size + (pagesize - (size % pagesize));
}

//------------------------------Base--------------------------------
enum oswEventDataFlag {
    OSW_EVENT_DATA_NORMAL,
    OSW_EVENT_DATA_PTR = 1u << 1,
    OSW_EVENT_DATA_CHUNK = 1u << 2,
    OSW_EVENT_DATA_BEGIN = 1u << 3,
    OSW_EVENT_DATA_END = 1u << 4,
    OSW_EVENT_DATA_OBJ_PTR = 1u << 5,
    OSW_EVENT_DATA_POP_PTR = 1u << 6,
};

/**
 * Use server_fd field to store flag of the server task
 */
#define OSW_TASK_TYPE(task) ((task)->info.server_fd)

enum oswTaskType {
    OSW_TASK_TMPFILE = 1,     // tmp file
    OSW_TASK_SERIALIZE = 2,   // php serialize
    OSW_TASK_NONBLOCK = 4,    // task
    OSW_TASK_CALLBACK = 8,    // callback
    OSW_TASK_WAITALL = 16,    // for taskWaitAll
    OSW_TASK_COROUTINE = 32,  // coroutine
    OSW_TASK_PEEK = 64,       // peek
    OSW_TASK_NOREPLY = 128,   // don't reply
};

enum oswDNSLookupFlag {
    OSW_DNS_LOOKUP_RANDOM = (1u << 11),
};

#ifdef __MACH__
char *osw_error_();
#define osw_error osw_error_()
#else
extern __thread char osw_error[OSW_ERROR_MSG_SIZE];
#endif

enum oswProcessType {
    OSW_PROCESS_MASTER = 1,
    OSW_PROCESS_WORKER = 2,
    OSW_PROCESS_MANAGER = 3,
    OSW_PROCESS_TASKWORKER = 4,
    OSW_PROCESS_USERWORKER = 5,
};

enum oswPipeType {
    OSW_PIPE_WORKER = 0,
    OSW_PIPE_MASTER = 1,
    OSW_PIPE_READ = 0,
    OSW_PIPE_WRITE = 1,
    OSW_PIPE_NONBLOCK = 2,
};

//----------------------Tool Function---------------------
uint32_t openswoole_common_multiple(uint32_t u, uint32_t v);
uint32_t openswoole_common_divisor(uint32_t u, uint32_t v);

int openswoole_itoa(char *buf, long value);
bool openswoole_mkdir_recursive(const std::string &dir);

int openswoole_rand(int min, int max);
int openswoole_system_random(int min, int max);

int openswoole_version_compare(const char *version1, const char *version2);
void openswoole_print_backtrace(void);
char *openswoole_string_format(size_t n, const char *format, ...);
bool openswoole_get_env(const char *name, int *value);
int openswoole_get_systemd_listen_fds();

void openswoole_init(void);
void openswoole_clean(void);
pid_t openswoole_fork(int flags);
void openswoole_redirect_stdout(int new_fd);
int openswoole_shell_exec(const char *command, pid_t *pid, bool get_error_stream);
int openswoole_daemon(int nochdir, int noclose);
bool openswoole_set_task_tmpdir(const std::string &dir);
int openswoole_tmpfile(char *filename);

#ifdef HAVE_CPU_AFFINITY
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/cpuset.h>
#include <pthread_np.h>
typedef cpuset_t cpu_set_t;
#endif
int openswoole_set_cpu_affinity(cpu_set_t *set);
#endif

#ifdef HAVE_CLOCK_GETTIME
#define openswoole_clock_gettime clock_gettime
#else
int openswoole_clock_gettime(int which_clock, struct timespec *t);
#endif

static inline struct timespec openswoole_time_until(int milliseconds) {
    struct timespec t;
    openswoole_clock_gettime(CLOCK_REALTIME, &t);

    int sec = milliseconds / 1000;
    int msec = milliseconds - (sec * 1000);

    t.tv_sec += sec;
    t.tv_nsec += msec * 1000 * 1000;

    if (t.tv_nsec > OSW_NUM_BILLION) {
        int _sec = t.tv_nsec / OSW_NUM_BILLION;
        t.tv_sec += _sec;
        t.tv_nsec -= _sec * OSW_NUM_BILLION;
    }

    return t;
}

namespace openswoole {

typedef long SessionId;
typedef long TaskId;
typedef uint8_t ReactorId;
typedef uint32_t WorkerId;
typedef enum oswEventType EventType;
typedef enum oswSocketType SocketType;
typedef enum oswFdType FdType;

struct Event {
    int fd;
    int16_t reactor_id;
    FdType type;
    network::Socket *socket;
};

struct DataHead {
    SessionId fd;
    uint64_t msg_id;
    uint32_t len;
    int16_t reactor_id;
    uint8_t type;
    uint8_t flags;
    uint16_t server_fd;
    uint16_t ext_flags;
    double time;
    size_t dump(char *buf, size_t len);
};

struct EventData {
    DataHead info;
    char data[OSW_IPC_BUFFER_SIZE];
    bool pack(const void *data, size_t data_len);
    bool unpack(String *buffer);
};

struct ThreadGlobal {
    uint16_t id;
    uint8_t type;
    uint8_t update_time;
    String *buffer_stack;
    Reactor *reactor;
    Timer *timer;
    AsyncThreads *async_threads;
    IoUringEngine *io_uring_engine;
    uint32_t signal_listener_num;
    uint32_t co_signal_listener_num;
    int error;
};

struct Allocator {
    void *(*malloc)(size_t size);
    void *(*calloc)(size_t nmemb, size_t size);
    void *(*realloc)(void *ptr, size_t size);
    void (*free)(void *ptr);
};

struct Global {
    uchar init : 1;
    uchar running : 1;
    uchar use_signalfd : 1;
    uchar wait_signal : 1;
    uchar enable_signalfd : 1;
    uchar socket_dontwait : 1;
    uchar dns_lookup_random : 1;
    uchar use_async_resolver : 1;

    int process_type;
    uint32_t process_id;
    TaskId current_task_id;
    pid_t pid;

    int signal_fd;
    bool signal_alarm;

    long trace_flags;

    void (*fatal_error)(int code, const char *str, ...);

    //-----------------------[System]--------------------------
    uint16_t cpu_num;
    uint32_t pagesize;
    struct utsname uname;
    uint32_t max_sockets;
    uint32_t max_concurrency;
    bool enable_server_token;
    int reactor_type;
    //-----------------------[Memory]--------------------------
    MemoryPool *memory_pool;
    Allocator std_allocator;
    std::string task_tmpfile;
    //-----------------------[DNS]--------------------------
    std::string dns_server_host;
    int dns_server_port;
    double dns_cache_refresh_time;
    int dns_tries;
    std::string dns_resolvconf_path;
    std::string dns_hosts_path;
    //-----------------------[AIO]--------------------------
    uint32_t aio_core_worker_num;
    uint32_t aio_worker_num;
    double aio_max_wait_time;
    double aio_max_idle_time;
    network::Socket *aio_default_socket;
    //-----------------------[Hook]--------------------------
    void *hooks[OSW_MAX_HOOK_TYPE];
    std::function<bool(Reactor *reactor, size_t &event_num)> user_exit_condition;

    // bug report message
    std::string bug_report_message = "";
};

std::string dirname(const std::string &file);
int hook_add(void **hooks, int type, const Callback &func, int push_back);
void hook_call(void **hooks, int type, void *arg);
double microtime(void);
}  // namespace openswoole

extern openswoole::Global OpenSwooleG;  // Local Global Variable
#ifdef __MACH__
extern thread_local openswoole::ThreadGlobal OpenSwooleTG;  // Thread Global Variable
#else
extern __thread openswoole::ThreadGlobal OpenSwooleTG;  // Thread Global Variable
#endif

#define OSW_CPU_NUM (OpenSwooleG.cpu_num)

static inline void openswoole_set_last_error(int error) {
    OpenSwooleTG.error = error;
}

static inline int openswoole_get_last_error() {
    return OpenSwooleTG.error;
}

static inline int openswoole_get_thread_id() {
    return OpenSwooleTG.id;
}

static inline int openswoole_get_process_type() {
    return OpenSwooleG.process_type;
}

static inline int openswoole_get_process_id() {
    return OpenSwooleG.process_id;
}

OSW_API const char *openswoole_strerror(int code);
OSW_API void openswoole_throw_error(int code);
OSW_API void openswoole_set_log_level(int level);
OSW_API void openswoole_set_trace_flags(int flags);
OSW_API void openswoole_set_dns_server(const std::string &server);
OSW_API void openswoole_set_hosts_path(const std::string &hosts_file);
OSW_API std::pair<std::string, int> openswoole_get_dns_server();
OSW_API bool openswoole_load_resolv_conf();

//-----------------------------------------------
static osw_inline void osw_spinlock(osw_atomic_t *lock) {
    uint32_t i, n;
    while (1) {
        if (*lock == 0 && osw_atomic_cmp_set(lock, 0, 1)) {
            return;
        }
        if (OSW_CPU_NUM > 1) {
            for (n = 1; n < OSW_SPINLOCK_LOOP_N; n <<= 1) {
                for (i = 0; i < n; i++) {
                    osw_atomic_cpu_pause();
                }

                if (*lock == 0 && osw_atomic_cmp_set(lock, 0, 1)) {
                    return;
                }
            }
        }
        osw_yield();
    }
}

static osw_inline openswoole::String *osw_tg_buffer() {
    return OpenSwooleTG.buffer_stack;
}

static osw_inline openswoole::MemoryPool *osw_mem_pool() {
    return OpenSwooleG.memory_pool;
}

static osw_inline const openswoole::Allocator *osw_std_allocator() {
    return &OpenSwooleG.std_allocator;
}

/*
 * Backward-compatibility header: maps all legacy Swoole names to OpenSwoole.
 * Must be included AFTER all declarations above so the macros remap correctly.
 */
#include "openswoole_compat.h"
