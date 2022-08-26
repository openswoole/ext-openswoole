/*
  +----------------------------------------------------------------------+
  | Open Swoole                                                          |
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
#include "php_swoole_cxx.h"
#include "php_swoole_process.h"

#if (HAVE_PCRE || HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
#include "ext/pcre/php_pcre.h"
#endif
#include "zend_exceptions.h"

BEGIN_EXTERN_C()
#ifdef SW_USE_JSON
#include "ext/json/php_json.h"
#endif
END_EXTERN_C()

#include "swoole_mime_type.h"
#include "swoole_server.h"
#include "swoole_util.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>

#if defined(__MACH__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <net/if_dl.h>
#endif

#ifdef SW_HAVE_ZLIB
#include <zlib.h>
#endif
#ifdef SW_HAVE_BROTLI
#include <brotli/encode.h>
#include <brotli/decode.h>
#endif

#ifdef SW_USE_CARES
#include <ares.h>
#endif

using swoole::Server;
using swoole::network::Socket;

ZEND_DECLARE_MODULE_GLOBALS(openswoole)

extern sapi_module_struct sapi_module;

static swoole::CallbackManager rshutdown_callbacks;

SW_EXTERN_C_BEGIN
static PHP_FUNCTION(swoole_version);
static PHP_FUNCTION(swoole_cpu_num);
static PHP_FUNCTION(swoole_strerror);
static PHP_FUNCTION(swoole_clear_error);
static PHP_FUNCTION(swoole_errno);
static PHP_FUNCTION(swoole_error_log);
static PHP_FUNCTION(swoole_get_local_ip);
static PHP_FUNCTION(swoole_get_local_mac);
static PHP_FUNCTION(swoole_hashcode);
static PHP_FUNCTION(swoole_mime_type_add);
static PHP_FUNCTION(swoole_mime_type_set);
static PHP_FUNCTION(swoole_mime_type_delete);
static PHP_FUNCTION(swoole_mime_type_get);
static PHP_FUNCTION(swoole_mime_type_exists);
static PHP_FUNCTION(swoole_mime_type_list);
static PHP_FUNCTION(swoole_substr_unserialize);
#ifdef SW_USE_JSON
static PHP_FUNCTION(swoole_substr_json_decode);
#endif
static PHP_FUNCTION(swoole_internal_call_user_shutdown_begin);
SW_EXTERN_C_END

// clang-format off

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_async_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_async_dns_lookup_coro, 0, 0, 1)
    ZEND_ARG_INFO(0, domain_name)
    ZEND_ARG_INFO(0, timeout)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
    ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_coroutine_defer, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_coroutine_socketpair, 0, 0, 3)
    ZEND_ARG_INFO(0, domain)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_test_kernel_coroutine, 0, 0, 0)
    ZEND_ARG_INFO(0, count)
    ZEND_ARG_INFO(0, sleep_time)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_client_select, 0, 0, 3)
    ZEND_ARG_INFO(1, read_array)
    ZEND_ARG_INFO(1, write_array)
    ZEND_ARG_INFO(1, error_array)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_substr_unserialize, 0, 0, 2)
    ZEND_ARG_INFO(0, str)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

#ifdef SW_USE_JSON
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_substr_json_decode, 0, 0, 2)
    ZEND_ARG_INFO(0, json)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, length)
    ZEND_ARG_INFO(0, associative)
    ZEND_ARG_INFO(0, depth)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_set_process_name, 0, 0, 1)
    ZEND_ARG_INFO(0, process_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_strerror, 0, 0, 1)
    ZEND_ARG_INFO(0, errno)
    ZEND_ARG_INFO(0, error_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_error_log, 0, 0, 2)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, msg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_hashcode, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

/* add/set */
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_mime_type_write, 0, 0, 2)
    ZEND_ARG_INFO(0, suffix)
    ZEND_ARG_INFO(0, mime_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_mime_type_delete, 0, 0, 1)
    ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

/* get/exists */
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_mime_type_read, 0, 0, 1)
    ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

const zend_function_entry swoole_functions[] = {
    PHP_FE(swoole_version, arginfo_swoole_void)
    PHP_FE(swoole_cpu_num, arginfo_swoole_void)
    PHP_FE(swoole_last_error, arginfo_swoole_void)
    /*------swoole_async_io------*/
    PHP_FE(swoole_async_dns_lookup_coro, arginfo_swoole_async_dns_lookup_coro)
    PHP_FE(swoole_async_set, arginfo_swoole_async_set)
    /*------swoole_coroutine------*/
    PHP_FE(swoole_coroutine_create, arginfo_swoole_coroutine_create)
    PHP_FE(swoole_coroutine_defer, arginfo_swoole_coroutine_defer)
    PHP_FE(swoole_coroutine_socketpair, arginfo_swoole_coroutine_socketpair)
    PHP_FE(swoole_test_kernel_coroutine, arginfo_swoole_test_kernel_coroutine)
    /*------other-----*/
    PHP_FE(swoole_client_select, arginfo_swoole_client_select)
    PHP_FALIAS(swoole_select, swoole_client_select, arginfo_swoole_client_select)
    PHP_FE(swoole_set_process_name, arginfo_swoole_set_process_name)
    PHP_FE(swoole_get_local_ip, arginfo_swoole_void)
    PHP_FE(swoole_get_local_mac, arginfo_swoole_void)
    PHP_FE(swoole_strerror, arginfo_swoole_strerror)
    PHP_FE(swoole_errno, arginfo_swoole_void)
    PHP_FE(swoole_clear_error, arginfo_swoole_void)
    PHP_FE(swoole_error_log, arginfo_swoole_error_log)
    PHP_FE(swoole_hashcode, arginfo_swoole_hashcode)
    PHP_FE(swoole_mime_type_add, arginfo_swoole_mime_type_write)
    PHP_FE(swoole_mime_type_set, arginfo_swoole_mime_type_write)
    PHP_FE(swoole_mime_type_delete, arginfo_swoole_mime_type_delete)
    PHP_FE(swoole_mime_type_get, arginfo_swoole_mime_type_read)
    PHP_FALIAS(swoole_get_mime_type, swoole_mime_type_get, arginfo_swoole_mime_type_read)
    PHP_FE(swoole_mime_type_exists, arginfo_swoole_mime_type_read)
    PHP_FE(swoole_mime_type_list, arginfo_swoole_void)
    PHP_FE(swoole_clear_dns_cache, arginfo_swoole_void)
    PHP_FE(swoole_substr_unserialize, arginfo_swoole_substr_unserialize)
#ifdef SW_USE_JSON
    PHP_FE(swoole_substr_json_decode, arginfo_swoole_substr_json_decode)
#endif
    PHP_FE(swoole_internal_call_user_shutdown_begin, arginfo_swoole_void)
    PHP_FE_END /* Must be the last line in swoole_functions[] */
};

static const zend_module_dep swoole_deps[] = {
    ZEND_MOD_CONFLICTS("swoole")
#ifdef SW_USE_JSON
    ZEND_MOD_REQUIRED("json")
#endif
#ifdef SW_USE_MYSQLND
    ZEND_MOD_REQUIRED("mysqlnd")
#endif
#ifdef SW_SOCKETS
    ZEND_MOD_REQUIRED("sockets")
#endif
#ifdef SW_USE_CURL
    ZEND_MOD_REQUIRED("curl")
#endif
    ZEND_MOD_END
};

zend_module_entry openswoole_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    nullptr,
    swoole_deps,
    "openswoole",
    swoole_functions,
    PHP_MINIT(openswoole),
    PHP_MSHUTDOWN(openswoole),
    PHP_RINIT(openswoole),     //RINIT
    PHP_RSHUTDOWN(openswoole), //RSHUTDOWN
    PHP_MINFO(openswoole),
    PHP_SWOOLE_VERSION,
    STANDARD_MODULE_PROPERTIES
};
// clang-format on

zend_class_entry *swoole_exception_ce;
zend_object_handlers swoole_exception_handlers;

zend_class_entry *swoole_error_ce;
zend_object_handlers swoole_error_handlers;

zend_class_entry *openswoole_constants_ce;
zend_object_handlers openswoole_constants_handlers;

#ifdef COMPILE_DL_OPENSWOOLE
ZEND_GET_MODULE(openswoole)
#endif

// clang-format off
/* {{{ PHP_INI
 */

PHP_INI_BEGIN()
/**
 * enable swoole coroutine
 */
STD_ZEND_INI_BOOLEAN("swoole.enable_coroutine", "On", PHP_INI_ALL, OnUpdateBool, enable_coroutine, zend_openswoole_globals, openswoole_globals)
/**
 * enable swoole coroutine epreemptive scheduler
 */
STD_ZEND_INI_BOOLEAN("swoole.enable_preemptive_scheduler", "Off", PHP_INI_ALL, OnUpdateBool, enable_preemptive_scheduler, zend_openswoole_globals, openswoole_globals)
/**
 * display error
 */
STD_ZEND_INI_BOOLEAN("swoole.display_errors", "On", PHP_INI_ALL, OnUpdateBool, display_errors, zend_openswoole_globals, openswoole_globals)
/**
 * use short class name
 */
STD_ZEND_INI_BOOLEAN("swoole.use_shortname", "On", PHP_INI_SYSTEM, OnUpdateBool, use_shortname, zend_openswoole_globals, openswoole_globals)
/**
 * unix socket buffer size
 */
STD_PHP_INI_ENTRY("swoole.unixsock_buffer_size", ZEND_TOSTR(SW_SOCKET_BUFFER_SIZE), PHP_INI_ALL, OnUpdateLong, socket_buffer_size, zend_openswoole_globals, openswoole_globals)
PHP_INI_END()
// clang-format on

static void php_swoole_init_globals(zend_openswoole_globals *openswoole_globals) {
    openswoole_globals->enable_coroutine = 1;
    openswoole_globals->enable_preemptive_scheduler = 0;
    openswoole_globals->socket_buffer_size = SW_SOCKET_BUFFER_SIZE;
    openswoole_globals->display_errors = 1;
    openswoole_globals->use_shortname = 1;
}

void php_swoole_register_shutdown_function(const char *function) {
    php_shutdown_function_entry shutdown_function_entry;
#if PHP_VERSION_ID >= 80100
    zval function_name;
    ZVAL_STRING(&function_name, function);
    zend_fcall_info_init(
        &function_name, 0, &shutdown_function_entry.fci, &shutdown_function_entry.fci_cache, NULL, NULL);
    register_user_shutdown_function(Z_STRVAL(function_name), Z_STRLEN(function_name), &shutdown_function_entry);
#else
    zval *function_name;
#if PHP_VERSION_ID >= 80000
    shutdown_function_entry.arg_count = 0;
    shutdown_function_entry.arguments = NULL;
    function_name = &shutdown_function_entry.function_name;
#else
    shutdown_function_entry.arg_count = 1;
    shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), 1, 0);
    function_name = &shutdown_function_entry.arguments[0];
#endif
    ZVAL_STRING(function_name, function);
    register_user_shutdown_function(Z_STRVAL_P(function_name), Z_STRLEN_P(function_name), &shutdown_function_entry);
#endif
}

void php_swoole_set_global_option(HashTable *vht) {
    zval *ztmp;

#ifdef SW_DEBUG
    if (php_swoole_array_get_value(vht, "debug_mode", ztmp) && zval_is_true(ztmp)) {
        sw_logger()->set_level(0);
    }
#endif
    if (php_swoole_array_get_value(vht, "trace_flags", ztmp)) {
        SwooleG.trace_flags = (uint32_t) SW_MAX(0, zval_get_long(ztmp));
    }
    if (php_swoole_array_get_value(vht, "log_file", ztmp)) {
        sw_logger()->open(zend::String(ztmp).val());
    }
    if (php_swoole_array_get_value(vht, "log_level", ztmp)) {
        sw_logger()->set_level(zval_get_long(ztmp));
    }
    if (php_swoole_array_get_value(vht, "log_date_format", ztmp)) {
        sw_logger()->set_date_format(zend::String(ztmp).val());
    }
    if (php_swoole_array_get_value(vht, "log_date_with_microseconds", ztmp)) {
        sw_logger()->set_date_with_microseconds(zval_is_true(ztmp));
    }
    if (php_swoole_array_get_value(vht, "log_rotation", ztmp)) {
        sw_logger()->set_rotation(zval_get_long(ztmp));
    }
    if (php_swoole_array_get_value(vht, "display_errors", ztmp)) {
        SWOOLE_G(display_errors) = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "dns_server", ztmp)) {
        swoole_set_dns_server(zend::String(ztmp).to_std_string());
    }

    auto timeout_format = [](zval *v) -> double {
        double timeout = zval_get_double(v);
        if (timeout <= 0 || timeout > INT_MAX) {
            return INT_MAX;
        } else {
            return timeout;
        }
    };

    if (php_swoole_array_get_value(vht, "socket_dns_timeout", ztmp)) {
        Socket::default_dns_timeout = timeout_format(ztmp);
    }
    if (php_swoole_array_get_value(vht, "socket_connect_timeout", ztmp)) {
        Socket::default_connect_timeout = timeout_format(ztmp);
    }
    if (php_swoole_array_get_value(vht, "socket_write_timeout", ztmp) ||
        php_swoole_array_get_value(vht, "socket_send_timeout", ztmp)) {
        Socket::default_write_timeout = timeout_format(ztmp);
    }
    if (php_swoole_array_get_value(vht, "socket_read_timeout", ztmp) ||
        php_swoole_array_get_value(vht, "socket_recv_timeout", ztmp)) {
        Socket::default_read_timeout = timeout_format(ztmp);
    }
    if (php_swoole_array_get_value(vht, "socket_buffer_size", ztmp)) {
        Socket::default_buffer_size = zval_get_long(ztmp);
    }
    if (php_swoole_array_get_value(vht, "socket_timeout", ztmp)) {
        Socket::default_read_timeout = Socket::default_write_timeout = timeout_format(ztmp);
    }
    if (php_swoole_array_get_value(vht, "max_concurrency", ztmp)) {
        SwooleG.max_concurrency = (uint32_t) SW_MAX(0, zval_get_long(ztmp));
    }
    if (php_swoole_array_get_value(vht, "enable_server_token", ztmp) && zval_is_true(ztmp)) {
        SwooleG.enable_server_token = zval_is_true(ztmp);
    }
}

void php_swoole_register_rshutdown_callback(swoole::Callback cb, void *private_data) {
    rshutdown_callbacks.append(cb, private_data);
}

SW_API bool php_swoole_is_enable_coroutine() {
    if (sw_server()) {
        return sw_server()->is_enable_coroutine();
    } else {
        return SWOOLE_G(enable_coroutine);
    }
}

static void fatal_error(int code, const char *format, ...) {
    va_list args;
    va_start(args, format);
    zend_object *exception =
        zend_throw_exception(swoole_error_ce, swoole::std_string::vformat(format, args).c_str(), code);
    va_end(args);

    zend_try {
        zend_exception_error(exception, E_ERROR);
    }
    zend_catch {
        exit(255);
    }
    zend_end_try();
}

static void bug_report_message_init() {
    SwooleG.bug_report_message += swoole::std_string::format("PHP_VERSION : %s\n", PHP_VERSION);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(openswoole) {
    ZEND_INIT_MODULE_GLOBALS(openswoole, php_swoole_init_globals, nullptr);
    REGISTER_INI_ENTRIES();

    // clang-format off
    // MUST be on the same line for the inspection tool to recognize correctly
    SW_REGISTER_STRING_CONSTANT("SWOOLE_VERSION", SWOOLE_VERSION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_VERSION_ID", SWOOLE_VERSION_ID);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_MAJOR_VERSION", SWOOLE_MAJOR_VERSION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_MINOR_VERSION", SWOOLE_MINOR_VERSION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_RELEASE_VERSION", SWOOLE_RELEASE_VERSION);
    SW_REGISTER_STRING_CONSTANT("SWOOLE_EXTRA_VERSION", SWOOLE_EXTRA_VERSION);
#ifndef SW_DEBUG
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_DEBUG", 0);
#else
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_DEBUG", 1);
#endif

#ifdef SW_HAVE_COMPRESSION
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_HAVE_COMPRESSION", 1);
#endif
#ifdef SW_HAVE_ZLIB
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_HAVE_ZLIB", 1);
#endif
#ifdef SW_HAVE_BROTLI
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_HAVE_BROTLI", 1);
#endif
#ifdef SW_USE_HTTP2
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_USE_HTTP2", 1);
#endif
#ifdef SW_USE_POSTGRES
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_USE_POSTGRES", 1);
#endif

    SW_REGISTER_BOOL_CONSTANT("SWOOLE_USE_SHORTNAME", SWOOLE_G(use_shortname));

    /**
     * socket type
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_TCP", SW_SOCK_TCP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_TCP6", SW_SOCK_TCP6);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_UDP", SW_SOCK_UDP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_UDP6", SW_SOCK_UDP6);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_UNIX_DGRAM", SW_SOCK_UNIX_DGRAM);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SOCK_UNIX_STREAM", SW_SOCK_UNIX_STREAM);

    /**
     * simple socket type alias
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TCP", SW_SOCK_TCP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TCP6", SW_SOCK_TCP6);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_UDP", SW_SOCK_UDP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_UDP6", SW_SOCK_UDP6);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_UNIX_DGRAM", SW_SOCK_UNIX_DGRAM);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_UNIX_STREAM", SW_SOCK_UNIX_STREAM);

    /**
     * simple api
     */
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_SOCK_SYNC", 0);
    SW_REGISTER_BOOL_CONSTANT("SWOOLE_SOCK_ASYNC", 1);

    SW_REGISTER_LONG_CONSTANT("SWOOLE_SYNC", SW_FLAG_SYNC);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ASYNC", SW_FLAG_ASYNC);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_KEEP", SW_FLAG_KEEP);

#ifdef SW_USE_OPENSSL
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL", SW_SOCK_SSL);

    /**
     * SSL methods
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv3_METHOD", SW_SSLv3_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv3_SERVER_METHOD", SW_SSLv3_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv3_CLIENT_METHOD", SW_SSLv3_CLIENT_METHOD);

    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_METHOD", SW_TLSv1_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_SERVER_METHOD", SW_TLSv1_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_CLIENT_METHOD", SW_TLSv1_CLIENT_METHOD);

#ifdef TLS1_1_VERSION
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_1_METHOD", SW_TLSv1_1_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_1_SERVER_METHOD", SW_TLSv1_1_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_1_CLIENT_METHOD", SW_TLSv1_1_CLIENT_METHOD);
#endif

#ifdef TLS1_2_VERSION
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_2_METHOD", SW_TLSv1_2_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_2_SERVER_METHOD", SW_TLSv1_2_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLSv1_2_CLIENT_METHOD", SW_TLSv1_2_CLIENT_METHOD);
#endif

#ifdef SW_SUPPORT_DTLS
    SW_REGISTER_LONG_CONSTANT("SWOOLE_DTLS_SERVER_METHOD", SW_DTLS_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_DTLS_CLIENT_METHOD", SW_DTLS_CLIENT_METHOD);
#endif

    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv23_METHOD", SW_SSLv23_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv23_SERVER_METHOD", SW_SSLv23_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSLv23_CLIENT_METHOD", SW_SSLv23_CLIENT_METHOD);
    /* SSLv23_method have been renamed to TLS_method */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLS_METHOD", SW_SSLv23_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLS_SERVER_METHOD", SW_SSLv23_SERVER_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TLS_CLIENT_METHOD", SW_SSLv23_CLIENT_METHOD);

    /**
     * SSL protocols
     */
#ifdef HAVE_SSL3
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_SSLv3", SW_SSL_SSLv3);
#endif
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_TLSv1", SW_SSL_TLSv1);

#ifdef TLS1_1_VERSION
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_TLSv1_1", SW_SSL_TLSv1_1);
#endif

#ifdef TLS1_2_VERSION
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_TLSv1_2", SW_SSL_TLSv1_2);
#endif

#ifdef TLS1_3_VERSION
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_TLSv1_3", SW_SSL_TLSv1_3);
#endif

#ifdef SW_SUPPORT_DTLS
    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_DTLS", SW_SSL_DTLS);
#endif

    SW_REGISTER_LONG_CONSTANT("SWOOLE_SSL_SSLv2", SW_SSL_SSLv2);
#endif

    SW_REGISTER_LONG_CONSTANT("SWOOLE_EVENT_READ", SW_EVENT_READ);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_EVENT_WRITE", SW_EVENT_WRITE);

    /**
     * Register ERROR types
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_STRERROR_SYSTEM", SW_STRERROR_SYSTEM);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_STRERROR_GAI", SW_STRERROR_GAI);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_STRERROR_DNS", SW_STRERROR_DNS);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_STRERROR_SWOOLE", SW_STRERROR_SWOOLE);

    /**
     * Register ERROR constants
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_MALLOC_FAIL", SW_ERROR_MALLOC_FAIL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SYSTEM_CALL_FAIL", SW_ERROR_SYSTEM_CALL_FAIL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_PHP_FATAL_ERROR", SW_ERROR_PHP_FATAL_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_NAME_TOO_LONG", SW_ERROR_NAME_TOO_LONG);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_INVALID_PARAMS", SW_ERROR_INVALID_PARAMS);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_QUEUE_FULL", SW_ERROR_QUEUE_FULL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_OPERATION_NOT_SUPPORT", SW_ERROR_OPERATION_NOT_SUPPORT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_PROTOCOL_ERROR", SW_ERROR_PROTOCOL_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WRONG_OPERATION", SW_ERROR_WRONG_OPERATION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_FILE_NOT_EXIST", SW_ERROR_FILE_NOT_EXIST);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_FILE_TOO_LARGE", SW_ERROR_FILE_TOO_LARGE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_FILE_EMPTY", SW_ERROR_FILE_EMPTY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DNSLOOKUP_DUPLICATE_REQUEST", SW_ERROR_DNSLOOKUP_DUPLICATE_REQUEST);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED", SW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT", SW_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DNSLOOKUP_UNSUPPORTED", SW_ERROR_DNSLOOKUP_UNSUPPORTED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DNSLOOKUP_NO_SERVER", SW_ERROR_DNSLOOKUP_NO_SERVER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_BAD_IPV6_ADDRESS", SW_ERROR_BAD_IPV6_ADDRESS);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_UNREGISTERED_SIGNAL", SW_ERROR_UNREGISTERED_SIGNAL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_EVENT_SOCKET_REMOVED", SW_ERROR_EVENT_SOCKET_REMOVED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_CLOSED_BY_SERVER", SW_ERROR_SESSION_CLOSED_BY_SERVER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_CLOSED_BY_CLIENT", SW_ERROR_SESSION_CLOSED_BY_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_CLOSING", SW_ERROR_SESSION_CLOSING);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_CLOSED", SW_ERROR_SESSION_CLOSED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_NOT_EXIST", SW_ERROR_SESSION_NOT_EXIST);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_INVALID_ID", SW_ERROR_SESSION_INVALID_ID);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_DISCARD_TIMEOUT_DATA", SW_ERROR_SESSION_DISCARD_TIMEOUT_DATA);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SESSION_DISCARD_DATA", SW_ERROR_SESSION_DISCARD_DATA);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_OUTPUT_BUFFER_OVERFLOW", SW_ERROR_OUTPUT_BUFFER_OVERFLOW);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_OUTPUT_SEND_YIELD", SW_ERROR_OUTPUT_SEND_YIELD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_NOT_READY", SW_ERROR_SSL_NOT_READY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_CANNOT_USE_SENFILE", SW_ERROR_SSL_CANNOT_USE_SENFILE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_EMPTY_PEER_CERTIFICATE", SW_ERROR_SSL_EMPTY_PEER_CERTIFICATE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_VERIFY_FAILED", SW_ERROR_SSL_VERIFY_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_BAD_CLIENT", SW_ERROR_SSL_BAD_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_BAD_PROTOCOL", SW_ERROR_SSL_BAD_PROTOCOL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_RESET", SW_ERROR_SSL_RESET);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SSL_HANDSHAKE_FAILED", SW_ERROR_SSL_HANDSHAKE_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_PACKAGE_LENGTH_TOO_LARGE", SW_ERROR_PACKAGE_LENGTH_TOO_LARGE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_PACKAGE_LENGTH_NOT_FOUND", SW_ERROR_PACKAGE_LENGTH_NOT_FOUND);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_DATA_LENGTH_TOO_LARGE", SW_ERROR_DATA_LENGTH_TOO_LARGE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_TASK_PACKAGE_TOO_BIG", SW_ERROR_TASK_PACKAGE_TOO_BIG);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_TASK_DISPATCH_FAIL", SW_ERROR_TASK_DISPATCH_FAIL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_TASK_TIMEOUT", SW_ERROR_TASK_TIMEOUT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP2_STREAM_ID_TOO_BIG", SW_ERROR_HTTP2_STREAM_ID_TOO_BIG);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP2_STREAM_NO_HEADER", SW_ERROR_HTTP2_STREAM_NO_HEADER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP2_STREAM_NOT_FOUND", SW_ERROR_HTTP2_STREAM_NOT_FOUND);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP2_STREAM_IGNORE", SW_ERROR_HTTP2_STREAM_IGNORE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_AIO_BAD_REQUEST", SW_ERROR_AIO_BAD_REQUEST);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_AIO_CANCELED", SW_ERROR_AIO_CANCELED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_AIO_TIMEOUT", SW_ERROR_AIO_TIMEOUT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CLIENT_NO_CONNECTION", SW_ERROR_CLIENT_NO_CONNECTION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKET_CLOSED", SW_ERROR_SOCKET_CLOSED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKET_POLL_TIMEOUT", SW_ERROR_SOCKET_POLL_TIMEOUT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKS5_UNSUPPORT_VERSION", SW_ERROR_SOCKS5_UNSUPPORT_VERSION);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKS5_UNSUPPORT_METHOD", SW_ERROR_SOCKS5_UNSUPPORT_METHOD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKS5_AUTH_FAILED", SW_ERROR_SOCKS5_AUTH_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKS5_SERVER_ERROR", SW_ERROR_SOCKS5_SERVER_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SOCKS5_HANDSHAKE_FAILED", SW_ERROR_SOCKS5_HANDSHAKE_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP_PROXY_HANDSHAKE_ERROR", SW_ERROR_HTTP_PROXY_HANDSHAKE_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP_INVALID_PROTOCOL", SW_ERROR_HTTP_INVALID_PROTOCOL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP_PROXY_HANDSHAKE_FAILED", SW_ERROR_HTTP_PROXY_HANDSHAKE_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_HTTP_PROXY_BAD_RESPONSE", SW_ERROR_HTTP_PROXY_BAD_RESPONSE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WEBSOCKET_BAD_CLIENT", SW_ERROR_WEBSOCKET_BAD_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WEBSOCKET_BAD_OPCODE", SW_ERROR_WEBSOCKET_BAD_OPCODE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WEBSOCKET_UNCONNECTED", SW_ERROR_WEBSOCKET_UNCONNECTED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WEBSOCKET_HANDSHAKE_FAILED", SW_ERROR_WEBSOCKET_HANDSHAKE_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_WEBSOCKET_PACK_FAILED", SW_ERROR_WEBSOCKET_PACK_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT", SW_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_TOO_MANY_SOCKET", SW_ERROR_SERVER_TOO_MANY_SOCKET);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_WORKER_TERMINATED", SW_ERROR_SERVER_WORKER_TERMINATED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_INVALID_LISTEN_PORT", SW_ERROR_SERVER_INVALID_LISTEN_PORT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_TOO_MANY_LISTEN_PORT", SW_ERROR_SERVER_TOO_MANY_LISTEN_PORT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_PIPE_BUFFER_FULL", SW_ERROR_SERVER_PIPE_BUFFER_FULL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_NO_IDLE_WORKER", SW_ERROR_SERVER_NO_IDLE_WORKER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_ONLY_START_ONE", SW_ERROR_SERVER_ONLY_START_ONE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_SEND_IN_MASTER", SW_ERROR_SERVER_SEND_IN_MASTER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_INVALID_REQUEST", SW_ERROR_SERVER_INVALID_REQUEST);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_CONNECT_FAIL", SW_ERROR_SERVER_CONNECT_FAIL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_WORKER_EXIT_TIMEOUT", SW_ERROR_SERVER_WORKER_EXIT_TIMEOUT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA", SW_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_SERVER_WORKER_UNPROCESSED_DATA", SW_ERROR_SERVER_WORKER_UNPROCESSED_DATA);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_OUT_OF_COROUTINE", SW_ERROR_CO_OUT_OF_COROUTINE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_HAS_BEEN_BOUND", SW_ERROR_CO_HAS_BEEN_BOUND);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_HAS_BEEN_DISCARDED", SW_ERROR_CO_HAS_BEEN_DISCARDED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_MUTEX_DOUBLE_UNLOCK", SW_ERROR_CO_MUTEX_DOUBLE_UNLOCK);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_BLOCK_OBJECT_LOCKED", SW_ERROR_CO_BLOCK_OBJECT_LOCKED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_BLOCK_OBJECT_WAITING", SW_ERROR_CO_BLOCK_OBJECT_WAITING);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_YIELD_FAILED", SW_ERROR_CO_YIELD_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_GETCONTEXT_FAILED", SW_ERROR_CO_GETCONTEXT_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_SWAPCONTEXT_FAILED", SW_ERROR_CO_SWAPCONTEXT_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_MAKECONTEXT_FAILED", SW_ERROR_CO_MAKECONTEXT_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_IOCPINIT_FAILED", SW_ERROR_CO_IOCPINIT_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_PROTECT_STACK_FAILED", SW_ERROR_CO_PROTECT_STACK_FAILED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_STD_THREAD_LINK_ERROR", SW_ERROR_CO_STD_THREAD_LINK_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_DISABLED_MULTI_THREAD", SW_ERROR_CO_DISABLED_MULTI_THREAD);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_CANNOT_CANCEL", SW_ERROR_CO_CANNOT_CANCEL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_NOT_EXISTS", SW_ERROR_CO_NOT_EXISTS);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_CANCELED", SW_ERROR_CO_CANCELED);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_ERROR_CO_TIMEDOUT", SW_ERROR_CO_TIMEDOUT);

    /**
     * trace log
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_SERVER", SW_TRACE_SERVER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CLIENT", SW_TRACE_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_BUFFER", SW_TRACE_BUFFER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CONN", SW_TRACE_CONN);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_EVENT", SW_TRACE_EVENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_WORKER", SW_TRACE_WORKER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_MEMORY", SW_TRACE_MEMORY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_REACTOR", SW_TRACE_REACTOR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_PHP", SW_TRACE_PHP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_HTTP", SW_TRACE_HTTP);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_HTTP2", SW_TRACE_HTTP2);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_EOF_PROTOCOL", SW_TRACE_EOF_PROTOCOL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_LENGTH_PROTOCOL", SW_TRACE_LENGTH_PROTOCOL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CLOSE", SW_TRACE_CLOSE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_WEBSOCKET", SW_TRACE_WEBSOCKET);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_REDIS_CLIENT", SW_TRACE_REDIS_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_MYSQL_CLIENT", SW_TRACE_MYSQL_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_HTTP_CLIENT", SW_TRACE_HTTP_CLIENT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_AIO", SW_TRACE_AIO);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_SSL", SW_TRACE_SSL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_NORMAL", SW_TRACE_NORMAL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CHANNEL", SW_TRACE_CHANNEL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_TIMER", SW_TRACE_TIMER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_SOCKET", SW_TRACE_SOCKET);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_COROUTINE", SW_TRACE_COROUTINE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CONTEXT", SW_TRACE_CONTEXT);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CO_HTTP_SERVER", SW_TRACE_CO_HTTP_SERVER);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_TABLE", SW_TRACE_TABLE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CO_CURL", SW_TRACE_CO_CURL);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_CARES", SW_TRACE_CARES);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_TRACE_ALL", SW_TRACE_ALL);

    /**
     * log level
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_DEBUG", SW_LOG_DEBUG);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_TRACE", SW_LOG_TRACE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_INFO", SW_LOG_INFO);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_NOTICE", SW_LOG_NOTICE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_WARNING", SW_LOG_WARNING);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ERROR", SW_LOG_ERROR);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_NONE", SW_LOG_NONE);

    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ROTATION_SINGLE", SW_LOG_ROTATION_SINGLE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ROTATION_MONTHLY", SW_LOG_ROTATION_MONTHLY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ROTATION_DAILY", SW_LOG_ROTATION_DAILY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ROTATION_HOURLY", SW_LOG_ROTATION_HOURLY);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_LOG_ROTATION_EVERY_MINUTE", SW_LOG_ROTATION_EVERY_MINUTE);

    SW_REGISTER_LONG_CONSTANT("SWOOLE_IPC_NONE", SW_IPC_NONE);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_IPC_UNIXSOCK", SW_IPC_UNIXSOCK);
    SW_REGISTER_LONG_CONSTANT("SWOOLE_IPC_SOCKET", SW_IPC_SOCKET);

    /**
     * stats
     */
    SW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_DEFAULT", 0);
    SW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_JSON", 1);
    SW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_OPENMETRICS", 2);


    /**
     * limit
     */
    SW_REGISTER_LONG_CONSTANT("SWOOLE_IOV_MAX", IOV_MAX);

    // clang-format on

    if (SWOOLE_G(use_shortname)) {
        SW_FUNCTION_ALIAS(CG(function_table), "swoole_coroutine_create", CG(function_table), "go");
        SW_FUNCTION_ALIAS(CG(function_table), "swoole_coroutine_defer", CG(function_table), "defer");
    }

    SW_INIT_CLASS_ENTRY(openswoole_constants, "Swoole\\Constant", nullptr, nullptr, nullptr);
    openswoole_constants_ce->ce_flags |= ZEND_ACC_FINAL;

    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_START"), "start");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_SHUTDOWN"), "shutdown");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_WORKER_START"), "workerStart");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_WORKER_STOP"), "workerStop");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_BEFORE_RELOAD"), "beforeReload");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_AFTER_RELOAD"), "afterReload");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_TASK"), "task");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_FINISH"), "finish");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_WORKER_EXIT"), "workerExit");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_WORKER_ERROR"), "workerError");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_MANAGER_START"), "managerStart");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_MANAGER_STOP"), "managerStop");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_PIPE_MESSAGE"), "pipeMessage");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_CONNECT"), "connect");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_RECEIVE"), "receive");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_CLOSE"), "close");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_PACKET"), "packet");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_BUFFER_FULL"), "bufferFull");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_BUFFER_EMPTY"), "bufferEmpty");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_REQUEST"), "request");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_HANDSHAKE"), "handshake");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_OPEN"), "open");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_MESSAGE"), "message");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_DISCONNECT"), "disconnect");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("EVENT_ERROR"), "error");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DEBUG_MODE"), "debug_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TRACE_FLAGS"), "trace_flags");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_FILE"), "log_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_LEVEL"), "log_level");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_DATE_FORMAT"), "log_date_format");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_DATE_WITH_MICROSECONDS"), "log_date_with_microseconds");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_ROTATION"), "log_rotation");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPLAY_ERRORS"), "display_errors");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_SERVER"), "dns_server");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_DNS_TIMEOUT"), "socket_dns_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_CONNECT_TIMEOUT"), "socket_connect_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_WRITE_TIMEOUT"), "socket_write_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_SEND_TIMEOUT"), "socket_send_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_READ_TIMEOUT"), "socket_read_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_RECV_TIMEOUT"), "socket_recv_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_BUFFER_SIZE"), "socket_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_TIMEOUT"), "socket_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONCURRENCY"), "max_concurrency");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_SERVER_TOKEN"), "enable_server_token");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_AIO_CORE_WORKER_NUM"), "aio_core_worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_AIO_WORKER_NUM"), "aio_worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_AIO_MAX_WAIT_TIME"), "aio_max_wait_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_AIO_MAX_IDLE_TIME"), "aio_max_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_SIGNALFD"), "enable_signalfd");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WAIT_SIGNAL"), "wait_signal");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_REFRESH_TIME"), "dns_cache_refresh_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_THREAD_NUM"), "thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MIN_THREAD_NUM"), "min_thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_THREAD_NUM"), "max_thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_DONTWAIT"), "socket_dontwait");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_LOOKUP_RANDOM"), "dns_lookup_random");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_USE_ASYNC_RESOLVER"), "use_async_resolver");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_COROUTINE"), "enable_coroutine");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PROTOCOLS"), "ssl_protocols");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_COMPRESS"), "ssl_compress");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CERT_FILE"), "ssl_cert_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_KEY_FILE"), "ssl_key_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PASSPHRASE"), "ssl_passphrase");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_HOST_NAME"), "ssl_host_name");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_VERIFY_PEER"), "ssl_verify_peer");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_ALLOW_SELF_SIGNED"), "ssl_allow_self_signed");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CAFILE"), "ssl_cafile");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CAPATH"), "ssl_capath");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_VERIFY_DEPTH"), "ssl_verify_depth");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CIPHERS"), "ssl_ciphers");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_EOF_CHECK"), "open_eof_check");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_EOF_SPLIT"), "open_eof_split");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_EOF"), "package_eof");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_MQTT_PROTOCOL"), "open_mqtt_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_LENGTH_CHECK"), "open_length_check");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_TYPE"), "package_length_type");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_OFFSET"), "package_length_offset");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_BODY_OFFSET"), "package_body_offset");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_FUNC"), "package_length_func");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_MAX_LENGTH"), "package_max_length");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_HIGH_WATERMARK"), "buffer_high_watermark");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_LOW_WATERMARK"), "buffer_low_watermark");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BIND_PORT"), "bind_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BIND_ADDRESS"), "bind_address");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_TCP_NODELAY"), "open_tcp_nodelay");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_HOST"), "socks5_host");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_PORT"), "socks5_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_USERNAME"), "socks5_username");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_PASSWORD"), "socks5_password");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_HOST"), "http_proxy_host");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_PORT"), "http_proxy_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_USERNAME"), "http_proxy_username");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_USER"), "http_proxy_user");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_PASSWORD"), "http_proxy_password");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TIMEOUT"), "timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CONNECT_TIMEOUT"), "connect_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_READ_TIMEOUT"), "read_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WRITE_TIMEOUT"), "write_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_DISABLE_COMPRESSION"), "ssl_disable_compression");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CORO_NUM"), "max_coro_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_COROUTINE"), "max_coroutine");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_DEADLOCK_CHECK"), "enable_deadlock_check");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HOOK_FLAGS"), "hook_flags");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_PREEMPTIVE_SCHEDULER"), "enable_preemptive_scheduler");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_C_STACK_SIZE"), "c_stack_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_STACK_SIZE"), "stack_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_EXPIRE"), "dns_cache_expire");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_CAPACITY"), "dns_cache_capacity");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_HEADER_TABLE_SIZE"), "http2_header_table_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_INITIAL_WINDOW_SIZE"), "http2_initial_window_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_CONCURRENT_STREAMS"), "http2_max_concurrent_streams");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_FRAME_SIZE"), "http2_max_frame_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_HEADER_LIST_SIZE"), "http2_max_header_list_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_RECONNECT"), "reconnect");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DEFER"), "defer");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOWERCASE_HEADER"), "lowercase_header");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_KEEP_ALIVE"), "keep_alive");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_MASK"), "websocket_mask");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_COMPRESSION"), "http_compression");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BODY_DECOMPRESSION"), "body_decompression");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_COMPRESSION"), "websocket_compression");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_COOKIE"), "http_parse_cookie");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_POST"), "http_parse_post");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_FILES"), "http_parse_files");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_COMPRESSION_LEVEL"), "http_compression_level");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_GZIP_LEVEL"), "http_gzip_level");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_COMPRESSION_MIN_LENGTH"), "compression_min_length");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_UPLOAD_TMP_DIR"), "upload_tmp_dir");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HOST"), "host");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PORT"), "port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL"), "ssl");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_USER"), "user");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PASSWORD"), "password");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DATABASE"), "database");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CHARSET"), "charset");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_STRICT_TYPE"), "strict_type");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_FETCH_MODE"), "fetch_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SERIALIZE"), "serialize");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_COMPATIBILITY_MODE"), "compatibility_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CHROOT"), "chroot");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_GROUP"), "group");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DAEMONIZE"), "daemonize");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PID_FILE"), "pid_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_REACTOR_NUM"), "reactor_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SINGLE_THREAD"), "single_thread");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WORKER_NUM"), "worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_WAIT_TIME"), "max_wait_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_QUEUED_BYTES"), "max_queued_bytes");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SEND_TIMEOUT"), "send_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPATCH_MODE"), "dispatch_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SEND_YIELD"), "send_yield");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPATCH_FUNC"), "dispatch_func");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISCARD_TIMEOUT_REQUEST"), "discard_timeout_request");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_UNSAFE_EVENT"), "enable_unsafe_event");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_DELAY_RECEIVE"), "enable_delay_receive");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_REUSE_PORT"), "enable_reuse_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_USE_OBJECT"), "task_use_object");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_OBJECT"), "task_object");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_EVENT_OBJECT"), "event_object");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_ENABLE_COROUTINE"), "task_enable_coroutine");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_WORKER_NUM"), "task_worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_IPC_MODE"), "task_ipc_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_TMPDIR"), "task_tmpdir");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_MAX_REQUEST"), "task_max_request");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_MAX_REQUEST_GRACE"), "task_max_request_grace");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONNECTION"), "max_connection");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONN"), "max_conn");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_START_SESSION_ID"), "start_session_id");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HEARTBEAT_CHECK_INTERVAL"), "heartbeat_check_interval");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HEARTBEAT_IDLE_TIME"), "heartbeat_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST"), "max_request");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST_GRACE"), "max_request_grace");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST_EXECUTION_TIME"), "max_request_execution_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_RELOAD_ASYNC"), "reload_async");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_CPU_AFFINITY"), "open_cpu_affinity");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CPU_AFFINITY_IGNORE"), "cpu_affinity_ignore");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_STATIC_HANDLER"), "enable_static_handler");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DOCUMENT_ROOT"), "document_root");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_AUTOINDEX"), "http_autoindex");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_INDEX_FILES"), "http_index_files");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_STATIC_HANDLER_LOCATIONS"), "static_handler_locations");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_INPUT_BUFFER_SIZE"), "input_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_INPUT_SIZE"), "buffer_input_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OUTPUT_BUFFER_SIZE"), "output_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_OUTPUT_SIZE"), "buffer_output_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MESSAGE_QUEUE_KEY"), "message_queue_key");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BACKLOG"), "backlog");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_KERNEL_SOCKET_RECV_BUFFER_SIZE"), "kernel_socket_recv_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_KERNEL_SOCKET_SEND_BUFFER_SIZE"), "kernel_socket_send_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_DEFER_ACCEPT"), "tcp_defer_accept");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_TCP_KEEPALIVE"), "open_tcp_keepalive");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_HTTP_PROTOCOL"), "open_http_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PROTOCOL"), "open_websocket_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_SUBPROTOCOL"), "websocket_subprotocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_CLOSE_FRAME"), "open_websocket_close_frame");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PING_FRAME"), "open_websocket_ping_frame");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PONG_FRAME"), "open_websocket_pong_frame");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_HTTP2_PROTOCOL"), "open_http2_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_REDIS_PROTOCOL"), "open_redis_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_IDLE_TIME"), "max_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPIDLE"), "tcp_keepidle");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPINTERVAL"), "tcp_keepinterval");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPCOUNT"), "tcp_keepcount");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_USER_TIMEOUT"), "tcp_user_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_FASTOPEN"), "tcp_fastopen");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_BODY_START"), "package_body_start");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CLIENT_CERT_FILE"), "ssl_client_cert_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PREFER_SERVER_CIPHERS"), "ssl_prefer_server_ciphers");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_ECDH_CURVE"), "ssl_ecdh_curve");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_DHPARAM"), "ssl_dhparam");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_SNI_CERTS"), "ssl_sni_certs");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_SSL"), "open_ssl");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_FASTCGI_PROTOCOL"), "open_fastcgi_protocol");

    swoole_init();

    // init bug report message
    bug_report_message_init();
    if (strcmp("cli", sapi_module.name) == 0 || strcmp("phpdbg", sapi_module.name) == 0) {
        SWOOLE_G(cli) = 1;
    }

    SW_INIT_CLASS_ENTRY_EX2(swoole_exception,
                            "Swoole\\Exception",
                            "swoole_exception",
                            nullptr,
                            nullptr,
                            zend_ce_exception,
                            zend_get_std_object_handlers());

    SW_INIT_CLASS_ENTRY_EX2(
        swoole_error, "Swoole\\Error", "swoole_error", nullptr, nullptr, zend_ce_error, zend_get_std_object_handlers());

    /** <Sort by dependency> **/
    php_swoole_event_minit(module_number);
    // base
    php_swoole_atomic_minit(module_number);
    php_swoole_lock_minit(module_number);
    php_swoole_process_minit(module_number);
    php_swoole_process_pool_minit(module_number);
    php_swoole_table_minit(module_number);
    php_swoole_timer_minit(module_number);
    // coroutine
    php_swoole_coroutine_minit(module_number);
    php_swoole_coroutine_system_minit(module_number);
    php_swoole_coroutine_scheduler_minit(module_number);
    php_swoole_channel_coro_minit(module_number);
    php_swoole_runtime_minit(module_number);
    // client
    php_swoole_socket_coro_minit(module_number);
    php_swoole_client_minit(module_number);
    php_swoole_client_coro_minit(module_number);
    php_swoole_http_client_coro_minit(module_number);
#ifdef SW_USE_HTTP2
    php_swoole_http2_client_coro_minit(module_number);
#endif
    // server
    php_swoole_server_minit(module_number);
    php_swoole_server_port_minit(module_number);
    php_swoole_http_request_minit(module_number);
    php_swoole_http_response_minit(module_number);
    php_swoole_http_server_minit(module_number);
    php_swoole_websocket_server_minit(module_number);
#ifdef SW_USE_POSTGRES
    php_swoole_postgresql_coro_minit(module_number);
#endif

    SwooleG.fatal_error = fatal_error;
    Socket::default_buffer_size = SWOOLE_G(socket_buffer_size);
    SwooleG.dns_cache_refresh_time = 60;

    // enable pcre.jit and use swoole extension on MacOS will lead to coredump, disable it temporarily
#if defined(PHP_PCRE_VERSION) && defined(HAVE_PCRE_JIT_SUPPORT) && PHP_VERSION_ID >= 70300 && __MACH__ &&              \
    !defined(SW_DEBUG)
    PCRE_G(jit) = 0;
#endif

    zend::known_strings_init();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(openswoole) {
    swoole_clean();
    zend::known_strings_dtor();
    php_swoole_runtime_mshutdown();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(openswoole) {
    char buf[64];
    php_info_print_table_start();
    php_info_print_table_header(2, "Open Swoole", "enabled");
    php_info_print_table_row(2, "Author", "Open Swoole Group <hello@openswoole.com>");
    php_info_print_table_row(2, "Version", SWOOLE_VERSION);
    snprintf(buf, sizeof(buf), "%s %s", __DATE__, __TIME__);
    php_info_print_table_row(2, "Built", buf);
#if defined(SW_USE_THREAD_CONTEXT)
    php_info_print_table_row(2, "coroutine", "enabled with thread context");
#elif defined(SW_USE_ASM_CONTEXT)
    php_info_print_table_row(2, "coroutine", "enabled with boost asm context");
#else
    php_info_print_table_row(2, "coroutine", "enabled with ucontext");
#endif
#ifdef SW_DEBUG
    php_info_print_table_row(2, "debug", "enabled");
#endif
#ifdef SW_LOG_TRACE_OPEN
    php_info_print_table_row(2, "trace_log", "enabled");
#endif
#ifdef HAVE_EPOLL
    php_info_print_table_row(2, "epoll", "enabled");
#endif
#ifdef HAVE_EVENTFD
    php_info_print_table_row(2, "eventfd", "enabled");
#endif
#ifdef HAVE_KQUEUE
    php_info_print_table_row(2, "kqueue", "enabled");
#endif
#ifdef HAVE_SIGNALFD
    php_info_print_table_row(2, "signalfd", "enabled");
#endif
#ifdef SW_USE_ACCEPT4
    php_info_print_table_row(2, "accept4", "enabled");
#endif
#ifdef HAVE_CPU_AFFINITY
    php_info_print_table_row(2, "cpu_affinity", "enabled");
#endif
#ifdef HAVE_SPINLOCK
    php_info_print_table_row(2, "spinlock", "enabled");
#endif
#ifdef HAVE_RWLOCK
    php_info_print_table_row(2, "rwlock", "enabled");
#endif
#ifdef SW_SOCKETS
    php_info_print_table_row(2, "sockets", "enabled");
#endif
#ifdef SW_USE_OPENSSL
#ifdef OPENSSL_VERSION_TEXT
    php_info_print_table_row(2, "openssl", OPENSSL_VERSION_TEXT);
#else
    php_info_print_table_row(2, "openssl", "enabled");
#endif
#ifdef SW_SUPPORT_DTLS
    php_info_print_table_row(2, "dtls", "enabled");
#endif
#endif
#ifdef SW_USE_HTTP2
    php_info_print_table_row(2, "http2", "enabled");
#endif
#ifdef SW_USE_JSON
    php_info_print_table_row(2, "json", "enabled");
#endif
#ifdef SW_USE_CURL
    php_info_print_table_row(2, "curl-native", "enabled");
#endif
#ifdef HAVE_PCRE
    php_info_print_table_row(2, "pcre", "enabled");
#endif
#ifdef SW_USE_CARES
    php_info_print_table_row(2, "c-ares", ares_version(nullptr));
#endif
#ifdef SW_HAVE_ZLIB
#ifdef ZLIB_VERSION
    php_info_print_table_row(2, "zlib", ZLIB_VERSION);
#else
    php_info_print_table_row(2, "zlib", "enabled");
#endif
#endif
#ifdef SW_HAVE_BROTLI
    snprintf(buf, sizeof(buf), "E%u/D%u", BrotliEncoderVersion(), BrotliDecoderVersion());
    php_info_print_table_row(2, "brotli", buf);
#endif
#ifdef HAVE_MUTEX_TIMEDLOCK
    php_info_print_table_row(2, "mutex_timedlock", "enabled");
#endif
#ifdef HAVE_PTHREAD_BARRIER
    php_info_print_table_row(2, "pthread_barrier", "enabled");
#endif
#ifdef HAVE_FUTEX
    php_info_print_table_row(2, "futex", "enabled");
#endif
#ifdef SW_USE_MYSQLND
    php_info_print_table_row(2, "mysqlnd", "enabled");
#endif
#ifdef SW_USE_JEMALLOC
    php_info_print_table_row(2, "jemalloc", "enabled");
#endif
#ifdef SW_USE_TCMALLOC
    php_info_print_table_row(2, "tcmalloc", "enabled");
#endif
    php_info_print_table_row(2, "async_redis", "enabled");
#ifdef SW_USE_POSTGRES
    php_info_print_table_row(2, "postgresql", "enabled");
#endif
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

static void *_sw_emalloc(size_t size) {
    return emalloc(size);
}

static void *_sw_ecalloc(size_t nmemb, size_t size) {
    return ecalloc(nmemb, size);
}

static void *_sw_erealloc(void *address, size_t size) {
    return erealloc(address, size);
}

static void _sw_efree(void *address) {
    efree(address);
}

static void *_sw_zend_string_malloc(size_t size) {
    zend_string *str = zend_string_alloc(size, 0);
    if (str == nullptr) {
        return nullptr;
    }
    return str->val;
}

static void *_sw_zend_string_calloc(size_t nmemb, size_t size) {
    void *mem = _sw_zend_string_malloc(nmemb * size);
    if (mem) {
        sw_memset_zero(mem, size);
    }
    return mem;
}

static void *_sw_zend_string_realloc(void *address, size_t size) {
    zend_string *str = zend_string_realloc(zend::fetch_zend_string_by_val(address), size, 0);
    if (str == nullptr) {
        return nullptr;
    }
    return str->val;
}

static void _sw_zend_string_free(void *address) {
    zend_string_free(zend::fetch_zend_string_by_val(address));
}

static swoole::Allocator php_allocator{
    _sw_emalloc,
    _sw_ecalloc,
    _sw_erealloc,
    _sw_efree,
};

static swoole::Allocator zend_string_allocator{
    _sw_zend_string_malloc,
    _sw_zend_string_calloc,
    _sw_zend_string_realloc,
    _sw_zend_string_free,
};

const swoole::Allocator *sw_php_allocator() {
    return &php_allocator;
}

const swoole::Allocator *sw_zend_string_allocator() {
    return &zend_string_allocator;
}

PHP_RINIT_FUNCTION(openswoole) {
    if (!SWOOLE_G(cli)) {
        return SUCCESS;
    }

    SWOOLE_G(req_status) = PHP_SWOOLE_RINIT_BEGIN;
    SwooleG.running = 1;

    php_swoole_register_shutdown_function("swoole_internal_call_user_shutdown_begin");

#ifdef ZEND_SIGNALS
    /* Disable warning even in ZEND_DEBUG because we may register our own signal handlers  */
    SIGG(check) = 0;
#endif

    php_swoole_coroutine_rinit();
    php_swoole_runtime_rinit();

    SWOOLE_G(req_status) = PHP_SWOOLE_RINIT_END;

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(openswoole) {
    if (!SWOOLE_G(cli)) {
        return SUCCESS;
    }

    SWOOLE_G(req_status) = PHP_SWOOLE_RSHUTDOWN_BEGIN;

    rshutdown_callbacks.execute();

    swoole_event_free();

    php_swoole_server_rshutdown();
    php_swoole_async_coro_rshutdown();
    php_swoole_coroutine_rshutdown();
    php_swoole_runtime_rshutdown();

    php_swoole_process_clean();

    SwooleG.running = 0;
    SWOOLE_G(req_status) = PHP_SWOOLE_RSHUTDOWN_END;

#ifdef PHP_STREAM_FLAG_NO_CLOSE
    auto php_swoole_set_stdio_no_close = [](const char *name, size_t name_len) {
        zval *zstream;
        php_stream *stream;

        zstream = zend_get_constant_str(name, name_len);
        if (!zstream) {
            return;
        }
        stream =
            (php_stream *) zend_fetch_resource2_ex((zstream), "stream", php_file_le_stream(), php_file_le_pstream());
        if (!stream) {
            return;
        }
        stream->flags |= PHP_STREAM_FLAG_NO_CLOSE;
    };
    /* do not close the stdout and stderr */
    php_swoole_set_stdio_no_close(ZEND_STRL("STDOUT"));
    php_swoole_set_stdio_no_close(ZEND_STRL("STDERR"));
#endif

    return SUCCESS;
}

static PHP_FUNCTION(swoole_version) {
    RETURN_STRING(SWOOLE_VERSION);
}

static uint32_t hashkit_one_at_a_time(const char *key, size_t key_length) {
    const char *ptr = key;
    uint32_t value = 0;

    while (key_length--) {
        uint32_t val = (uint32_t) *ptr++;
        value += val;
        value += (value << 10);
        value ^= (value >> 6);
    }
    value += (value << 3);
    value ^= (value >> 11);
    value += (value << 15);

    return value;
}

static PHP_FUNCTION(swoole_hashcode) {
    char *data;
    size_t l_data;
    zend_long type = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(data, l_data)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    switch (type) {
    case 0:
        RETURN_LONG(zend_hash_func(data, l_data));
    case 1:
        RETURN_LONG(hashkit_one_at_a_time(data, l_data));
    default:
        RETURN_FALSE;
    }
}

static PHP_FUNCTION(swoole_clear_error) {
    swoole_set_last_error(0);
}

PHP_FUNCTION(swoole_last_error) {
    RETURN_LONG(swoole_get_last_error());
}

static PHP_FUNCTION(swoole_cpu_num) {
    RETURN_LONG(SW_CPU_NUM);
}

static PHP_FUNCTION(swoole_strerror) {
    zend_long swoole_errno;
    zend_long error_type = SW_STRERROR_SYSTEM;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_LONG(swoole_errno)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(error_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (error_type == SW_STRERROR_GAI) {
        RETURN_STRING(gai_strerror(swoole_errno));
    } else if (error_type == SW_STRERROR_DNS) {
        RETURN_STRING(hstrerror(swoole_errno));
    } else if (error_type == SW_STRERROR_SWOOLE || (swoole_errno > SW_ERROR_BEGIN && swoole_errno < SW_ERROR_END)) {
        RETURN_STRING(swoole_strerror(swoole_errno));
    } else {
        RETURN_STRING(strerror(swoole_errno));
    }
}

static PHP_FUNCTION(swoole_error_log) {
    char *msg;
    size_t l_msg;
    zend_long level = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(level)
    Z_PARAM_STRING(msg, l_msg)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    sw_logger()->put(level, msg, l_msg);
}

static PHP_FUNCTION(swoole_mime_type_add) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::add(ZSTR_VAL(suffix), ZSTR_VAL(mime_type)));
}

static PHP_FUNCTION(swoole_mime_type_set) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    swoole::mime_type::set(ZSTR_VAL(suffix), ZSTR_VAL(mime_type));
}

static PHP_FUNCTION(swoole_mime_type_delete) {
    zend_string *suffix;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(suffix)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::del(ZSTR_VAL(suffix)));
}

static PHP_FUNCTION(swoole_mime_type_get) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_STRING(swoole::mime_type::get(ZSTR_VAL(filename)).c_str());
}

static PHP_FUNCTION(swoole_mime_type_exists) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::exists(ZSTR_VAL(filename)));
}

static PHP_FUNCTION(swoole_mime_type_list) {
    array_init(return_value);
    for (auto &i : swoole::mime_type::list()) {
        add_next_index_string(return_value, i.second.c_str());
    }
}

static PHP_FUNCTION(swoole_errno) {
    RETURN_LONG(errno);
}

PHP_FUNCTION(swoole_set_process_name) {
    zend_function *cli_set_process_title =
        (zend_function *) zend_hash_str_find_ptr(EG(function_table), ZEND_STRL("cli_set_process_title"));
    if (!cli_set_process_title) {
        php_swoole_fatal_error(E_WARNING, "swoole_set_process_name only support in CLI mode");
        RETURN_FALSE;
    }
    cli_set_process_title->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(swoole_get_local_ip) {
    struct sockaddr_in *s4;
    struct ifaddrs *ipaddrs, *ifa;
    void *in_addr;
    char ip[64];

    if (getifaddrs(&ipaddrs) != 0) {
        php_swoole_sys_error(E_WARNING, "getifaddrs() failed");
        RETURN_FALSE;
    }
    array_init(return_value);
    for (ifa = ipaddrs; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || !(ifa->ifa_flags & IFF_UP)) {
            continue;
        }

        switch (ifa->ifa_addr->sa_family) {
        case AF_INET:
            s4 = (struct sockaddr_in *) ifa->ifa_addr;
            in_addr = &s4->sin_addr;
            break;
        case AF_INET6:
            // struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            // in_addr = &s6->sin6_addr;
            continue;
        default:
            continue;
        }
        if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, ip, sizeof(ip))) {
            php_error_docref(nullptr, E_WARNING, "%s: inet_ntop failed", ifa->ifa_name);
        } else {
            // if (ifa->ifa_addr->sa_family == AF_INET && ntohl(((struct in_addr *) in_addr)->s_addr) ==
            // INADDR_LOOPBACK)
            if (strcmp(ip, "127.0.0.1") == 0) {
                continue;
            }
            add_assoc_string(return_value, ifa->ifa_name, ip);
        }
    }
    freeifaddrs(ipaddrs);
}

static PHP_FUNCTION(swoole_get_local_mac) {
    auto add_assoc_address = [](zval *zv, const char *name, const unsigned char *addr) {
        char buf[32];
        sw_snprintf(
            SW_STRS(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        add_assoc_string(zv, name, buf);
    };
#ifdef SIOCGIFHWADDR
    struct ifconf ifc;
    struct ifreq buf[16];

    int sock;
    int i = 0, num = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        php_swoole_sys_error(E_WARNING, "new socket failed");
        RETURN_FALSE;
    }
    array_init(return_value);

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (!ioctl(sock, SIOCGIFCONF, (char *) &ifc)) {
        num = ifc.ifc_len / sizeof(struct ifreq);
        while (i < num) {
            if (!(ioctl(sock, SIOCGIFHWADDR, (char *) &buf[i]))) {
                add_assoc_address(return_value, buf[i].ifr_name, (unsigned char *) buf[i].ifr_hwaddr.sa_data);
            }
            i++;
        }
    }
    close(sock);
#else
#ifdef LLADDR
    ifaddrs *ifas, *ifa;
    if (getifaddrs(&ifas) == 0) {
        array_init(return_value);
        for (ifa = ifas; ifa; ifa = ifa->ifa_next) {
            if ((ifa->ifa_addr->sa_family == AF_LINK) && ifa->ifa_addr) {
                add_assoc_address(
                    return_value, ifa->ifa_name, (unsigned char *) (LLADDR((struct sockaddr_dl *) ifa->ifa_addr)));
            }
        }
        freeifaddrs(ifas);
    }
#else
    php_error_docref(nullptr, E_WARNING, "swoole_get_local_mac is not supported");
    RETURN_FALSE;
#endif
#endif
}

static PHP_FUNCTION(swoole_internal_call_user_shutdown_begin) {
    if (SWOOLE_G(req_status) == PHP_SWOOLE_RINIT_END) {
        SWOOLE_G(req_status) = PHP_SWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN;
        RETURN_TRUE;
    } else {
        php_error_docref(nullptr, E_WARNING, "can not call this function in user level");
        RETURN_FALSE;
    }
}

static PHP_FUNCTION(swoole_substr_unserialize) {
    zend_long offset, length = 0;
    char *buf = NULL;
    size_t buf_len;
    zval *options = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 4)
    Z_PARAM_STRING(buf, buf_len)
    Z_PARAM_LONG(offset)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(length)
    Z_PARAM_ARRAY(options)
    ZEND_PARSE_PARAMETERS_END();

    if (buf_len == 0) {
        RETURN_FALSE;
    }
    if (offset < 0) {
        offset = buf_len + offset;
    }
    if ((zend_long) buf_len <= offset) {
        RETURN_FALSE;
    }
    if (length <= 0) {
        length = buf_len - offset;
    }
    zend::unserialize(return_value, buf + offset, length, options ? Z_ARRVAL_P(options) : NULL);
}

#ifdef SW_USE_JSON
static PHP_FUNCTION(swoole_substr_json_decode) {
    zend_long offset, length = 0;
    char *str;
    size_t str_len;
    zend_bool assoc = 0; /* return JS objects as PHP objects by default */
    zend_bool assoc_null = 1;
    zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
    zend_long options = 0;

    ZEND_PARSE_PARAMETERS_START(2, 6)
    Z_PARAM_STRING(str, str_len)
    Z_PARAM_LONG(offset)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(length)
    Z_PARAM_BOOL_EX(assoc, assoc_null, 1, 0)
    Z_PARAM_LONG(depth)
    Z_PARAM_LONG(options)
    ZEND_PARSE_PARAMETERS_END();

    if (str_len == 0) {
        RETURN_FALSE;
    }
    if (offset < 0) {
        offset = str_len + offset;
    }
    if ((zend_long) str_len <= offset) {
        RETURN_FALSE;
    }
    if (length <= 0) {
        length = str_len - offset;
    }
    /* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
    if (!assoc_null) {
        if (assoc) {
            options |= PHP_JSON_OBJECT_AS_ARRAY;
        } else {
            options &= ~PHP_JSON_OBJECT_AS_ARRAY;
        }
    }
    zend::json_decode(return_value, str + offset, length, options, depth);
}
#endif
