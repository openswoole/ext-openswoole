#include "php_swoole_cxx.h"

#include "swoole_coroutine_system_arginfo.h"

#include "ext/standard/file.h"
#include <sys/file.h>

#include <string>

using swoole::Coroutine;
using swoole::Event;
using swoole::PHPCoroutine;
using swoole::Reactor;
using swoole::String;
using swoole::TimerNode;
using swoole::coroutine::Socket;
using swoole::coroutine::System;

#include "swoole_socket.h"

#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using swoole::Timer;

static zend_class_entry *swoole_coroutine_system_ce;

struct DNSCacheEntity {
    char address[INET6_ADDRSTRLEN];
    time_t update_time;
};

static std::unordered_map<std::string, DNSCacheEntity *> request_cache_map;

void php_swoole_async_coro_rshutdown() {
    for (auto i = request_cache_map.begin(); i != request_cache_map.end(); i++) {
        efree(i->second);
    }
}

SW_EXTERN_C_BEGIN
PHP_METHOD(swoole_coroutine_system, exec);
PHP_METHOD(swoole_coroutine_system, sleep);
PHP_METHOD(swoole_coroutine_system, usleep);
PHP_METHOD(swoole_coroutine_system, fread);
PHP_METHOD(swoole_coroutine_system, fgets);
PHP_METHOD(swoole_coroutine_system, fwrite);
PHP_METHOD(swoole_coroutine_system, statvfs);
PHP_METHOD(swoole_coroutine_system, getaddrinfo);
PHP_METHOD(swoole_coroutine_system, readFile);
PHP_METHOD(swoole_coroutine_system, writeFile);
PHP_METHOD(swoole_coroutine_system, wait);
PHP_METHOD(swoole_coroutine_system, waitPid);
PHP_METHOD(swoole_coroutine_system, waitSignal);
PHP_METHOD(swoole_coroutine_system, waitEvent);
PHP_METHOD(swoole_coroutine_system, dnsLookup);
PHP_METHOD(swoole_coroutine_system, clearDNSCache);
SW_EXTERN_C_END

// clang-format off

static const zend_function_entry swoole_coroutine_system_methods[] =
{
    ZEND_FENTRY(gethostbyname, ZEND_FN(swoole_coroutine_gethostbyname), arginfo_class_Swoole_Coroutine_System_gethostbyname, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, exec, arginfo_class_Swoole_Coroutine_System_exec, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, sleep, arginfo_class_Swoole_Coroutine_System_sleep, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, usleep, arginfo_class_Swoole_Coroutine_System_usleep, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, getaddrinfo, arginfo_class_Swoole_Coroutine_System_getaddrinfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, statvfs, arginfo_class_Swoole_Coroutine_System_statvfs, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, readFile, arginfo_class_Swoole_Coroutine_System_readFile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, writeFile, arginfo_class_Swoole_Coroutine_System_writeFile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, wait, arginfo_class_Swoole_Coroutine_System_wait, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, waitPid, arginfo_class_Swoole_Coroutine_System_waitPid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, waitSignal, arginfo_class_Swoole_Coroutine_System_waitSignal, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, waitEvent, arginfo_class_Swoole_Coroutine_System_waitEvent, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, dnsLookup, arginfo_class_Swoole_Coroutine_System_dnsLookup, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(swoole_coroutine_system, clearDNSCache, arginfo_class_Swoole_Coroutine_System_clearDNSCache, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

// clang-format on

void php_swoole_coroutine_system_minit(int module_number) {
    SW_INIT_CLASS_ENTRY_BASE(swoole_coroutine_system,
                             "Swoole\\Coroutine\\System",
                             nullptr,
                             nullptr,
                             swoole_coroutine_system_methods,
                             nullptr);
    SW_SET_CLASS_CREATE(swoole_coroutine_system, sw_zend_create_object_deny);
}

PHP_METHOD(swoole_coroutine_system, sleep) {
    zend_long seconds;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(seconds)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (UNEXPECTED(seconds < 0)) {
        php_swoole_fatal_error(E_WARNING, "Number of seconds must be greater than or equal to " ZEND_TOSTR(0));
        RETURN_FALSE;
    }
    RETURN_BOOL(System::sleep(seconds) == 0);
}

PHP_METHOD(swoole_coroutine_system, usleep) {
    zend_long microseconds;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(microseconds)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (UNEXPECTED(microseconds < 0)) {
        php_swoole_fatal_error(E_WARNING, "Number of microseconds must be greater than or equal to " ZEND_TOSTR(0));
        RETURN_FALSE;
    }
    RETURN_BOOL(System::usleep(microseconds) == 0);
}

static void co_socket_read(int fd, zend_long length, INTERNAL_FUNCTION_PARAMETERS) {
    php_swoole_check_reactor();
    Socket _socket(fd, SW_SOCK_RAW);

    zend_string *buf = zend_string_alloc(length + 1, 0);
    size_t nbytes = length <= 0 ? SW_BUFFER_SIZE_STD : length;
    ssize_t n = _socket.read(ZSTR_VAL(buf), nbytes);
    if (n < 0) {
        ZVAL_FALSE(return_value);
        zend_string_free(buf);
    } else if (n == 0) {
        ZVAL_EMPTY_STRING(return_value);
        zend_string_free(buf);
    } else {
        ZSTR_VAL(buf)[n] = 0;
        ZSTR_LEN(buf) = n;
        ZVAL_STR(return_value, buf);
    }
    _socket.move_fd();
}

static void co_socket_write(int fd, char *str, size_t l_str, INTERNAL_FUNCTION_PARAMETERS) {
    php_swoole_check_reactor();
    Socket _socket(fd, SW_SOCK_RAW);

    ssize_t n = _socket.write(str, l_str);
    if (n < 0) {
        swoole_set_last_error(errno);
        ZVAL_FALSE(return_value);
    } else {
        ZVAL_LONG(return_value, n);
    }
    _socket.move_fd();
}

PHP_METHOD(swoole_coroutine_system, readFile) {
    char *filename;
    size_t l_filename;
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(filename, l_filename)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto result = System::read_file(filename, flags & LOCK_EX);
    if (result == nullptr) {
        RETURN_FALSE;
    } else {
        RETVAL_STRINGL(result->str, result->length);
    }
}

PHP_METHOD(swoole_coroutine_system, writeFile) {
    char *filename;
    size_t l_filename;
    char *data;
    size_t l_data;
    zend_long flags = 0;

    ZEND_PARSE_PARAMETERS_START(2, 3)
    Z_PARAM_STRING(filename, l_filename)
    Z_PARAM_STRING(data, l_data)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(flags)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    int _flags = 0;
    if (flags & PHP_FILE_APPEND) {
        _flags |= O_APPEND;
    } else {
        _flags |= O_TRUNC;
    }

    ssize_t retval = System::write_file(filename, data, l_data, flags & LOCK_EX, _flags);
    if (retval < 0) {
        RETURN_FALSE;
    } else {
        RETURN_LONG(retval);
    }
}

PHP_FUNCTION(swoole_coroutine_gethostbyname) {
    Coroutine::get_current_safe();

    char *domain_name;
    size_t l_domain_name;
    zend_long family = AF_INET;
    double timeout = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ld", &domain_name, &l_domain_name, &family, &timeout) == FAILURE) {
        RETURN_FALSE;
    }

    if (l_domain_name == 0) {
        php_swoole_fatal_error(E_WARNING, "domain name is empty");
        RETURN_FALSE;
    }

    if (family != AF_INET && family != AF_INET6) {
        php_swoole_fatal_error(E_WARNING, "unknown protocol family, must be AF_INET or AF_INET6");
        RETURN_FALSE;
    }

    std::string address = System::gethostbyname(std::string(domain_name, l_domain_name), family, timeout);
    if (address.empty()) {
        RETURN_FALSE;
    } else {
        RETURN_STRINGL(address.c_str(), address.length());
    }
}

PHP_METHOD(swoole_coroutine_system, clearDNSCache) {
    System::clear_dns_cache();
}

PHP_METHOD(swoole_coroutine_system, getaddrinfo) {
    char *hostname;
    size_t l_hostname;
    zend_long family = AF_INET;
    zend_long socktype = SOCK_STREAM;
    zend_long protocol = IPPROTO_TCP;
    char *service = nullptr;
    size_t l_service = 0;
    double timeout = -1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(),
                              "s|lllsd",
                              &hostname,
                              &l_hostname,
                              &family,
                              &socktype,
                              &protocol,
                              &service,
                              &l_service,
                              &timeout) == FAILURE) {
        RETURN_FALSE;
    }

    if (l_hostname == 0) {
        php_swoole_fatal_error(E_WARNING, "hostname is empty");
        RETURN_FALSE;
    }

    if (family != AF_INET && family != AF_INET6) {
        php_swoole_fatal_error(E_WARNING, "unknown protocol family, must be AF_INET or AF_INET6");
        RETURN_FALSE;
    }

    std::string str_service(service ? service : "");
    std::vector<std::string> result = System::getaddrinfo(hostname, family, socktype, protocol, str_service, timeout);

    if (result.empty()) {
        RETURN_FALSE;
    }

    array_init(return_value);
    for (auto i = result.begin(); i != result.end(); i++) {
        add_next_index_stringl(return_value, i->c_str(), i->length());
    }
}

PHP_METHOD(swoole_coroutine_system, statvfs) {
    char *path;
    size_t l_path;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(path, l_path)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    struct statvfs _stat;
    swoole_coroutine_statvfs(path, &_stat);

    array_init(return_value);
    add_assoc_long(return_value, "bsize", _stat.f_bsize);
    add_assoc_long(return_value, "frsize", _stat.f_frsize);
    add_assoc_long(return_value, "blocks", _stat.f_blocks);
    add_assoc_long(return_value, "bfree", _stat.f_bfree);
    add_assoc_long(return_value, "bavail", _stat.f_bavail);
    add_assoc_long(return_value, "files", _stat.f_files);
    add_assoc_long(return_value, "ffree", _stat.f_ffree);
    add_assoc_long(return_value, "favail", _stat.f_favail);
    add_assoc_long(return_value, "fsid", _stat.f_fsid);
    add_assoc_long(return_value, "flag", _stat.f_flag);
    add_assoc_long(return_value, "namemax", _stat.f_namemax);
}

PHP_METHOD(swoole_coroutine_system, exec) {
    char *command;
    size_t command_len;
    zend_bool get_error_stream = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(command, command_len)
    Z_PARAM_OPTIONAL
    Z_PARAM_BOOL(get_error_stream)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (php_swoole_signal_isset_handler(SIGCHLD)) {
        php_swoole_error(E_WARNING, "The signal [SIGCHLD] is registered, cannot execute swoole_coroutine_exec");
        RETURN_FALSE;
    }

    Coroutine::get_current_safe();

    pid_t pid;
    int fd = swoole_shell_exec(command, &pid, get_error_stream);
    if (fd < 0) {
        php_swoole_error(E_WARNING, "Unable to execute '%s'", command);
        RETURN_FALSE;
    }

    String *buffer = new String(1024);
    Socket socket(fd, SW_SOCK_UNIX_STREAM);
    while (1) {
        ssize_t retval = socket.read(buffer->str + buffer->length, buffer->size - buffer->length);
        if (retval > 0) {
            buffer->length += retval;
            if (buffer->length == buffer->size) {
                if (!buffer->extend()) {
                    break;
                }
            }
        } else {
            break;
        }
    }
    socket.close();

    zval zdata;
    if (buffer->length == 0) {
        ZVAL_EMPTY_STRING(&zdata);
    } else {
        ZVAL_STRINGL(&zdata, buffer->str, buffer->length);
    }
    delete buffer;

    int status;
    pid_t _pid = swoole_coroutine_waitpid(pid, &status, 0);
    if (_pid > 0) {
        array_init(return_value);
        add_assoc_long(return_value, "code", WEXITSTATUS(status));
        add_assoc_long(return_value, "signal", WTERMSIG(status));
        add_assoc_zval(return_value, "output", &zdata);
    } else {
        zval_ptr_dtor(&zdata);
        RETVAL_FALSE;
    }
}

static void swoole_coroutine_system_wait(INTERNAL_FUNCTION_PARAMETERS, pid_t pid, double timeout) {
    int status;

    Coroutine::get_current_safe();

    if (pid < 0) {
        pid = System::wait(&status, timeout);
    } else {
        pid = System::waitpid(pid, &status, 0, timeout);
    }
    if (pid > 0) {
        array_init(return_value);
        add_assoc_long(return_value, "pid", pid);
        add_assoc_long(return_value, "code", WEXITSTATUS(status));
        add_assoc_long(return_value, "signal", WTERMSIG(status));
    } else {
        swoole_set_last_error(errno);
        RETURN_FALSE;
    }
}

PHP_METHOD(swoole_coroutine_system, wait) {
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    swoole_coroutine_system_wait(INTERNAL_FUNCTION_PARAM_PASSTHRU, -1, timeout);
}

PHP_METHOD(swoole_coroutine_system, waitPid) {
    zend_long pid;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_LONG(pid)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    swoole_coroutine_system_wait(INTERNAL_FUNCTION_PARAM_PASSTHRU, pid, timeout);
}

PHP_METHOD(swoole_coroutine_system, waitSignal) {
    zend_long signo;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_LONG(signo)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (!System::wait_signal(signo, timeout)) {
        if (swoole_get_last_error() == EBUSY) {
            php_swoole_fatal_error(E_WARNING, "Unable to wait signal, async signal listener has been registered");
        } else if (swoole_get_last_error() == EINVAL) {
            php_swoole_fatal_error(E_WARNING, "Invalid signal [" ZEND_LONG_FMT "]", signo);
        }
        errno = swoole_get_last_error();
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

PHP_METHOD(swoole_coroutine_system, waitEvent) {
    zval *zfd;
    zend_long events = SW_EVENT_READ;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 3)
    Z_PARAM_ZVAL(zfd)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(events)
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    int fd = php_swoole_convert_to_fd(zfd);
    if (fd < 0) {
        php_swoole_fatal_error(E_WARNING, "unknown fd type");
        RETURN_FALSE;
    }

    events = System::wait_event(fd, events, timeout);
    if (events < 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(events);
}

PHP_METHOD(swoole_coroutine_system, dnsLookup) {
    Coroutine::get_current_safe();

    zval *domain;
    long type = AF_INET;
    double timeout = swoole::network::Socket::default_dns_timeout;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|dl", &domain, &timeout, &type) == FAILURE) {
        RETURN_FALSE;
    }

    if (Z_TYPE_P(domain) != IS_STRING) {
        php_swoole_fatal_error(E_WARNING, "invalid domain name");
        RETURN_FALSE;
    }

    if (Z_STRLEN_P(domain) == 0) {
        php_swoole_fatal_error(E_WARNING, "domain name empty");
        RETURN_FALSE;
    }

    // find cache
    std::string key(Z_STRVAL_P(domain), Z_STRLEN_P(domain));
    DNSCacheEntity *cache;

    if (request_cache_map.find(key) != request_cache_map.end()) {
        cache = request_cache_map[key];
        if (cache->update_time > Timer::get_absolute_msec()) {
            RETURN_STRING(cache->address);
        }
    }

    php_swoole_check_reactor();

    vector<string> result = swoole::coroutine::dns_lookup(Z_STRVAL_P(domain), type, timeout);
    if (result.empty()) {
        swoole_set_last_error(SW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
        RETURN_FALSE;
    }

    if (SwooleG.dns_lookup_random) {
        RETVAL_STRING(result[rand() % result.size()].c_str());
    } else {
        RETVAL_STRING(result[0].c_str());
    }

    auto cache_iterator = request_cache_map.find(key);
    if (cache_iterator == request_cache_map.end()) {
        cache = (DNSCacheEntity *) emalloc(sizeof(DNSCacheEntity));
        request_cache_map[key] = cache;
    } else {
        cache = cache_iterator->second;
    }
    memcpy(cache->address, Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
    cache->address[Z_STRLEN_P(return_value)] = '\0';
    cache->update_time = Timer::get_absolute_msec() + (int64_t) (SwooleG.dns_cache_refresh_time * 1000);
}
