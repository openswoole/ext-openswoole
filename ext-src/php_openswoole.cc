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
#include "php_openswoole_cxx.h"
#include "php_openswoole_process.h"
#include "openswoole_reactor.h"

#if (HAVE_PCRE || HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
#include "ext/pcre/php_pcre.h"
#endif
#include "zend_exceptions.h"

#include "openswoole_server.h"
#include "openswoole_util.h"

#ifdef OSW_HAVE_ZLIB
#include <zlib.h>
#endif
#ifdef OSW_HAVE_BROTLI
#include <brotli/encode.h>
#include <brotli/decode.h>
#endif

#ifdef OSW_USE_CARES
#include <ares.h>
#endif

using openswoole::Server;
using openswoole::network::Socket;

ZEND_DECLARE_MODULE_GLOBALS(openswoole)

extern sapi_module_struct sapi_module;

static openswoole::CallbackManager rshutdown_callbacks;

OSW_EXTERN_C_BEGIN
static PHP_FUNCTION(openswoole_internal_call_user_shutdown_begin);
OSW_EXTERN_C_END

// clang-format off

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
    ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_coroutine_defer, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_coroutine_socketpair, 0, 0, 3)
    ZEND_ARG_INFO(0, domain)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_test_kernel_coroutine, 0, 0, 0)
    ZEND_ARG_INFO(0, count)
    ZEND_ARG_INFO(0, sleep_time)
ZEND_END_ARG_INFO()

const zend_function_entry openswoole_functions[] = {
    /*------openswoole_coroutine------*/
    PHP_FE(openswoole_coroutine_create, arginfo_openswoole_coroutine_create)
    PHP_FE(openswoole_coroutine_defer, arginfo_openswoole_coroutine_defer)
    PHP_FE(openswoole_coroutine_socketpair, arginfo_openswoole_coroutine_socketpair)
    PHP_FE(openswoole_test_kernel_coroutine, arginfo_openswoole_test_kernel_coroutine)
    PHP_FE(openswoole_internal_call_user_shutdown_begin, arginfo_openswoole_void)
    PHP_FE_END /* Must be the last line in openswoole_functions[] */
};

static const zend_module_dep openswoole_deps[] = {
    ZEND_MOD_CONFLICTS("swoole")
#ifdef OSW_USE_MYSQLND
    ZEND_MOD_REQUIRED("mysqlnd")
#endif
#ifdef OSW_SOCKETS
    ZEND_MOD_REQUIRED("sockets")
#endif
#ifdef OSW_USE_CURL
    ZEND_MOD_REQUIRED("curl")
#endif
    ZEND_MOD_END
};

zend_module_entry openswoole_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    nullptr,
    openswoole_deps,
    "openswoole",
    openswoole_functions,
    PHP_MINIT(openswoole),
    PHP_MSHUTDOWN(openswoole),
    PHP_RINIT(openswoole),
    PHP_RSHUTDOWN(openswoole),
    PHP_MINFO(openswoole),
    PHP_OPENSWOOLE_VERSION,
    STANDARD_MODULE_PROPERTIES
};
// clang-format on

zend_class_entry *openswoole_exception_ce;
zend_object_handlers openswoole_exception_handlers;

zend_class_entry *openswoole_error_ce;
zend_object_handlers openswoole_error_handlers;

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
STD_ZEND_INI_BOOLEAN("openswoole.enable_coroutine", "On", PHP_INI_ALL, OnUpdateBool, enable_coroutine, zend_openswoole_globals, openswoole_globals)
/**
 * enable swoole coroutine epreemptive scheduler
 */
STD_ZEND_INI_BOOLEAN("openswoole.enable_preemptive_scheduler", "Off", PHP_INI_ALL, OnUpdateBool, enable_preemptive_scheduler, zend_openswoole_globals, openswoole_globals)
/**
 * use PHP fiber context for coroutines (enables xdebug tracing)
 */
STD_ZEND_INI_BOOLEAN("openswoole.use_fiber_context", "Off", PHP_INI_ALL, OnUpdateBool, use_fiber_context, zend_openswoole_globals, openswoole_globals)
/**
 * display error
 */
STD_ZEND_INI_BOOLEAN("openswoole.display_errors", "On", PHP_INI_ALL, OnUpdateBool, display_errors, zend_openswoole_globals, openswoole_globals)
/**
 * unix socket buffer size
 */
STD_PHP_INI_ENTRY("openswoole.unixsock_buffer_size", ZEND_TOSTR(OSW_SOCKET_BUFFER_SIZE), PHP_INI_ALL, OnUpdateLong, socket_buffer_size, zend_openswoole_globals, openswoole_globals)
PHP_INI_END()
// clang-format on

static void php_openswoole_init_globals(zend_openswoole_globals *openswoole_globals) {
    openswoole_globals->enable_coroutine = 1;
    openswoole_globals->enable_preemptive_scheduler = 0;
    openswoole_globals->use_fiber_context = 0;
    openswoole_globals->socket_buffer_size = OSW_SOCKET_BUFFER_SIZE;
    openswoole_globals->display_errors = 1;
}

void php_openswoole_register_shutdown_function(const char *function) {
#if PHP_VERSION_ID >= 80500
    php_shutdown_function_entry shutdown_function_entry = {};
    zend_function *fn_entry = (zend_function *) zend_hash_str_find_ptr(CG(function_table), function, strlen(function));
    if (fn_entry) {
        shutdown_function_entry.fci_cache.function_handler = fn_entry;
        register_user_shutdown_function(function, strlen(function), &shutdown_function_entry);
    }
#else
    php_shutdown_function_entry shutdown_function_entry;
    zval function_name;
    ZVAL_STRING(&function_name, function);
    zend_fcall_info_init(
        &function_name, 0, &shutdown_function_entry.fci, &shutdown_function_entry.fci_cache, NULL, NULL);
    register_user_shutdown_function(Z_STRVAL(function_name), Z_STRLEN(function_name), &shutdown_function_entry);
#endif
}

void php_openswoole_set_global_option(HashTable *vht) {
    zval *ztmp;

#ifdef OSW_DEBUG
    if (php_openswoole_array_get_value(vht, "debug_mode", ztmp) && zval_is_true(ztmp)) {
        osw_logger()->set_level(0);
    }
#endif
    if (php_openswoole_array_get_value(vht, "trace_flags", ztmp)) {
        OpenSwooleG.trace_flags = (uint32_t) OSW_MAX(0, zval_get_long(ztmp));
    }
    if (php_openswoole_array_get_value(vht, "log_file", ztmp)) {
        osw_logger()->open(zend::String(ztmp).val());
    }
    if (php_openswoole_array_get_value(vht, "log_level", ztmp)) {
        osw_logger()->set_level(zval_get_long(ztmp));
    }
    if (php_openswoole_array_get_value(vht, "log_date_format", ztmp)) {
        osw_logger()->set_date_format(zend::String(ztmp).val());
    }
    if (php_openswoole_array_get_value(vht, "log_date_with_microseconds", ztmp)) {
        osw_logger()->set_date_with_microseconds(zval_is_true(ztmp));
    }
    if (php_openswoole_array_get_value(vht, "log_rotation", ztmp)) {
        osw_logger()->set_rotation(zval_get_long(ztmp));
    }
    if (php_openswoole_array_get_value(vht, "display_errors", ztmp)) {
        OPENSWOOLE_G(display_errors) = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "dns_server", ztmp)) {
        openswoole_set_dns_server(zend::String(ztmp).to_std_string());
    }

    auto timeout_format = [](zval *v) -> double {
        double timeout = zval_get_double(v);
        if (timeout <= 0 || timeout > INT_MAX) {
            return INT_MAX;
        } else {
            return timeout;
        }
    };

    if (php_openswoole_array_get_value(vht, "socket_dns_timeout", ztmp)) {
        Socket::default_dns_timeout = timeout_format(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "socket_connect_timeout", ztmp)) {
        Socket::default_connect_timeout = timeout_format(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "socket_write_timeout", ztmp) ||
        php_openswoole_array_get_value(vht, "socket_send_timeout", ztmp)) {
        Socket::default_write_timeout = timeout_format(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "socket_read_timeout", ztmp) ||
        php_openswoole_array_get_value(vht, "socket_recv_timeout", ztmp)) {
        Socket::default_read_timeout = timeout_format(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "socket_buffer_size", ztmp)) {
        Socket::default_buffer_size = zval_get_long(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "socket_timeout", ztmp)) {
        Socket::default_read_timeout = Socket::default_write_timeout = timeout_format(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "max_concurrency", ztmp)) {
        OpenSwooleG.max_concurrency = (uint32_t) OSW_MAX(0, zval_get_long(ztmp));
    }
    if (php_openswoole_array_get_value(vht, "enable_server_token", ztmp) && zval_is_true(ztmp)) {
        OpenSwooleG.enable_server_token = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "reactor_type", ztmp)) {
        int reactor_type = (int) zval_get_long(ztmp);
        switch (reactor_type) {
        case openswoole::Reactor::TYPE_AUTO:
            break;
#ifdef HAVE_IO_URING
        case openswoole::Reactor::TYPE_IO_URING:
            break;
#endif
#ifdef HAVE_EPOLL
        case openswoole::Reactor::TYPE_EPOLL:
            break;
#endif
#ifdef HAVE_KQUEUE
        case openswoole::Reactor::TYPE_KQUEUE:
            break;
#endif
#ifdef HAVE_POLL
        case openswoole::Reactor::TYPE_POLL:
            break;
#endif
        case openswoole::Reactor::TYPE_SELECT:
            break;
        default:
            php_openswoole_error(E_WARNING, "unsupported reactor_type %d", reactor_type);
            return;
        }
        OpenSwooleG.reactor_type = reactor_type;
    }
}

void php_openswoole_register_rshutdown_callback(openswoole::Callback cb, void *private_data) {
    rshutdown_callbacks.append(cb, private_data);
}

OSW_API bool php_openswoole_is_enable_coroutine() {
    if (osw_server()) {
        return osw_server()->is_enable_coroutine();
    } else {
        return OPENSWOOLE_G(enable_coroutine);
    }
}

static void fatal_error(int code, const char *format, ...) {
    va_list args;
    va_start(args, format);
    zend_object *exception =
        zend_throw_exception(openswoole_error_ce, openswoole::std_string::vformat(format, args).c_str(), code);
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
    OpenSwooleG.bug_report_message += openswoole::std_string::format("PHP_VERSION : %s\n", PHP_VERSION);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(openswoole) {
    ZEND_INIT_MODULE_GLOBALS(openswoole, php_openswoole_init_globals, nullptr);
    REGISTER_INI_ENTRIES();

    // clang-format off
    // MUST be on the same line for the inspection tool to recognize correctly
    OSW_REGISTER_STRING_CONSTANT("OPENSWOOLE_VERSION", OPENSWOOLE_VERSION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_VERSION_ID", OPENSWOOLE_VERSION_ID);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_MAJOR_VERSION", OPENSWOOLE_MAJOR_VERSION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_MINOR_VERSION", OPENSWOOLE_MINOR_VERSION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_RELEASE_VERSION", OPENSWOOLE_RELEASE_VERSION);
    OSW_REGISTER_STRING_CONSTANT("OPENSWOOLE_EXTRA_VERSION", OPENSWOOLE_EXTRA_VERSION);

    OSW_INIT_CLASS_ENTRY(openswoole_constants, "OpenSwoole\\Constant", nullptr, nullptr, nullptr);
    openswoole_constants_ce->ce_flags |= ZEND_ACC_FINAL;

#ifndef OSW_DEBUG
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("HAVE_DEBUG"), 0);
#else
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("HAVE_DEBUG"), 1);
#endif

#ifdef OSW_HAVE_COMPRESSION
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("HAVE_COMPRESSION"), 1);
#endif
#ifdef OSW_HAVE_ZLIB
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("HAVE_ZLIB"), 1);
#endif
#ifdef OSW_HAVE_BROTLI
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("HAVE_BROTLI"), 1);
#endif
#ifdef OSW_USE_HTTP2
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("USE_HTTP2"), 1);
#endif
#ifdef OSW_USE_POSTGRES
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("USE_POSTGRES"), 1);
#endif

    /**
     * socket type
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_TCP"), OSW_SOCK_TCP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_TCP6"), OSW_SOCK_TCP6);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_UDP"), OSW_SOCK_UDP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_UDP6"), OSW_SOCK_UDP6);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_UNIX_DGRAM"), OSW_SOCK_UNIX_DGRAM);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SOCK_UNIX_STREAM"), OSW_SOCK_UNIX_STREAM);

    /**
     * simple socket type alias
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TCP"), OSW_SOCK_TCP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TCP6"), OSW_SOCK_TCP6);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("UDP"), OSW_SOCK_UDP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("UDP6"), OSW_SOCK_UDP6);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("UNIX_DGRAM"), OSW_SOCK_UNIX_DGRAM);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("UNIX_STREAM"), OSW_SOCK_UNIX_STREAM);

    /**
     * simple api
     */
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("SOCK_SYNC"), 0);
    zend_declare_class_constant_bool(openswoole_constants_ce, ZEND_STRL("SOCK_ASYNC"), 1);

    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SYNC"), OSW_FLAG_SYNC);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ASYNC"), OSW_FLAG_ASYNC);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("KEEP"), OSW_FLAG_KEEP);

#ifdef OSW_USE_OPENSSL
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL"), OSW_SOCK_SSL);

    /**
     * SSL methods
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv3_METHOD"), OSW_SSLv3_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv3_SERVER_METHOD"), OSW_SSLv3_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv3_CLIENT_METHOD"), OSW_SSLv3_CLIENT_METHOD);

    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_METHOD"), OSW_TLSv1_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_SERVER_METHOD"), OSW_TLSv1_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_CLIENT_METHOD"), OSW_TLSv1_CLIENT_METHOD);

#ifdef TLS1_1_VERSION
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_1_METHOD"), OSW_TLSv1_1_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_1_SERVER_METHOD"), OSW_TLSv1_1_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_1_CLIENT_METHOD"), OSW_TLSv1_1_CLIENT_METHOD);
#endif

#ifdef TLS1_2_VERSION
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_2_METHOD"), OSW_TLSv1_2_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_2_SERVER_METHOD"), OSW_TLSv1_2_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLSv1_2_CLIENT_METHOD"), OSW_TLSv1_2_CLIENT_METHOD);
#endif

#ifdef OSW_SUPPORT_DTLS
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("DTLS_SERVER_METHOD"), OSW_DTLS_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("DTLS_CLIENT_METHOD"), OSW_DTLS_CLIENT_METHOD);
#endif

    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv23_METHOD"), OSW_SSLv23_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv23_SERVER_METHOD"), OSW_SSLv23_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSLv23_CLIENT_METHOD"), OSW_SSLv23_CLIENT_METHOD);
    /* SSLv23_method have been renamed to TLS_method */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLS_METHOD"), OSW_SSLv23_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLS_SERVER_METHOD"), OSW_SSLv23_SERVER_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TLS_CLIENT_METHOD"), OSW_SSLv23_CLIENT_METHOD);

    /**
     * SSL protocols
     */
#ifdef HAVE_SSL3
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_SSLv3"), OSW_SSL_SSLv3);
#endif
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_TLSv1"), OSW_SSL_TLSv1);

#ifdef TLS1_1_VERSION
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_TLSv1_1"), OSW_SSL_TLSv1_1);
#endif

#ifdef TLS1_2_VERSION
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_TLSv1_2"), OSW_SSL_TLSv1_2);
#endif

#ifdef TLS1_3_VERSION
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_TLSv1_3"), OSW_SSL_TLSv1_3);
#endif

#ifdef OSW_SUPPORT_DTLS
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_DTLS"), OSW_SSL_DTLS);
#endif

    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("SSL_SSLv2"), OSW_SSL_SSLv2);
#endif

    /**
     * Register ERROR types
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("STRERROR_SYSTEM"), OSW_STRERROR_SYSTEM);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("STRERROR_GAI"), OSW_STRERROR_GAI);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("STRERROR_DNS"), OSW_STRERROR_DNS);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("STRERROR_SWOOLE"), OSW_STRERROR_SWOOLE);

    /**
     * Register ERROR constants
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_MALLOC_FAIL"), OSW_ERROR_MALLOC_FAIL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SYSTEM_CALL_FAIL"), OSW_ERROR_SYSTEM_CALL_FAIL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_PHP_FATAL_ERROR"), OSW_ERROR_PHP_FATAL_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_NAME_TOO_LONG"), OSW_ERROR_NAME_TOO_LONG);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_INVALID_PARAMS"), OSW_ERROR_INVALID_PARAMS);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_QUEUE_FULL"), OSW_ERROR_QUEUE_FULL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_OPERATION_NOT_SUPPORT"), OSW_ERROR_OPERATION_NOT_SUPPORT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_PROTOCOL_ERROR"), OSW_ERROR_PROTOCOL_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WRONG_OPERATION"), OSW_ERROR_WRONG_OPERATION);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_FILE_NOT_EXIST"), OSW_ERROR_FILE_NOT_EXIST);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_FILE_TOO_LARGE"), OSW_ERROR_FILE_TOO_LARGE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_FILE_EMPTY"), OSW_ERROR_FILE_EMPTY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DNSLOOKUP_DUPLICATE_REQUEST"), OSW_ERROR_DNSLOOKUP_DUPLICATE_REQUEST);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DNSLOOKUP_RESOLVE_FAILED"), OSW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DNSLOOKUP_RESOLVE_TIMEOUT"), OSW_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DNSLOOKUP_UNSUPPORTED"), OSW_ERROR_DNSLOOKUP_UNSUPPORTED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DNSLOOKUP_NO_SERVER"), OSW_ERROR_DNSLOOKUP_NO_SERVER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_BAD_IPV6_ADDRESS"), OSW_ERROR_BAD_IPV6_ADDRESS);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_UNREGISTERED_SIGNAL"), OSW_ERROR_UNREGISTERED_SIGNAL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_EVENT_SOCKET_REMOVED"), OSW_ERROR_EVENT_SOCKET_REMOVED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_CLOSED_BY_SERVER"), OSW_ERROR_SESSION_CLOSED_BY_SERVER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_CLOSED_BY_CLIENT"), OSW_ERROR_SESSION_CLOSED_BY_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_CLOSING"), OSW_ERROR_SESSION_CLOSING);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_CLOSED"), OSW_ERROR_SESSION_CLOSED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_NOT_EXIST"), OSW_ERROR_SESSION_NOT_EXIST);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_INVALID_ID"), OSW_ERROR_SESSION_INVALID_ID);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_DISCARD_TIMEOUT_DATA"), OSW_ERROR_SESSION_DISCARD_TIMEOUT_DATA);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SESSION_DISCARD_DATA"), OSW_ERROR_SESSION_DISCARD_DATA);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_OUTPUT_BUFFER_OVERFLOW"), OSW_ERROR_OUTPUT_BUFFER_OVERFLOW);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_OUTPUT_SEND_YIELD"), OSW_ERROR_OUTPUT_SEND_YIELD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_NOT_READY"), OSW_ERROR_SSL_NOT_READY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_CANNOT_USE_SENFILE"), OSW_ERROR_SSL_CANNOT_USE_SENFILE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_EMPTY_PEER_CERTIFICATE"), OSW_ERROR_SSL_EMPTY_PEER_CERTIFICATE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_VERIFY_FAILED"), OSW_ERROR_SSL_VERIFY_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_BAD_CLIENT"), OSW_ERROR_SSL_BAD_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_BAD_PROTOCOL"), OSW_ERROR_SSL_BAD_PROTOCOL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_RESET"), OSW_ERROR_SSL_RESET);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SSL_HANDSHAKE_FAILED"), OSW_ERROR_SSL_HANDSHAKE_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_PACKAGE_LENGTH_TOO_LARGE"), OSW_ERROR_PACKAGE_LENGTH_TOO_LARGE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_PACKAGE_LENGTH_NOT_FOUND"), OSW_ERROR_PACKAGE_LENGTH_NOT_FOUND);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_DATA_LENGTH_TOO_LARGE"), OSW_ERROR_DATA_LENGTH_TOO_LARGE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_TASK_PACKAGE_TOO_BIG"), OSW_ERROR_TASK_PACKAGE_TOO_BIG);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_TASK_DISPATCH_FAIL"), OSW_ERROR_TASK_DISPATCH_FAIL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_TASK_TIMEOUT"), OSW_ERROR_TASK_TIMEOUT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP2_STREAM_ID_TOO_BIG"), OSW_ERROR_HTTP2_STREAM_ID_TOO_BIG);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP2_STREAM_NO_HEADER"), OSW_ERROR_HTTP2_STREAM_NO_HEADER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP2_STREAM_NOT_FOUND"), OSW_ERROR_HTTP2_STREAM_NOT_FOUND);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP2_STREAM_IGNORE"), OSW_ERROR_HTTP2_STREAM_IGNORE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_AIO_BAD_REQUEST"), OSW_ERROR_AIO_BAD_REQUEST);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_AIO_CANCELED"), OSW_ERROR_AIO_CANCELED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_AIO_TIMEOUT"), OSW_ERROR_AIO_TIMEOUT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CLIENT_NO_CONNECTION"), OSW_ERROR_CLIENT_NO_CONNECTION);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKET_CLOSED"), OSW_ERROR_SOCKET_CLOSED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKET_POLL_TIMEOUT"), OSW_ERROR_SOCKET_POLL_TIMEOUT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKS5_UNSUPPORT_VERSION"), OSW_ERROR_SOCKS5_UNSUPPORT_VERSION);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKS5_UNSUPPORT_METHOD"), OSW_ERROR_SOCKS5_UNSUPPORT_METHOD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKS5_AUTH_FAILED"), OSW_ERROR_SOCKS5_AUTH_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKS5_SERVER_ERROR"), OSW_ERROR_SOCKS5_SERVER_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SOCKS5_HANDSHAKE_FAILED"), OSW_ERROR_SOCKS5_HANDSHAKE_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP_PROXY_HANDSHAKE_ERROR"), OSW_ERROR_HTTP_PROXY_HANDSHAKE_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP_INVALID_PROTOCOL"), OSW_ERROR_HTTP_INVALID_PROTOCOL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP_PROXY_HANDSHAKE_FAILED"), OSW_ERROR_HTTP_PROXY_HANDSHAKE_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_HTTP_PROXY_BAD_RESPONSE"), OSW_ERROR_HTTP_PROXY_BAD_RESPONSE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WEBSOCKET_BAD_CLIENT"), OSW_ERROR_WEBSOCKET_BAD_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WEBSOCKET_BAD_OPCODE"), OSW_ERROR_WEBSOCKET_BAD_OPCODE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WEBSOCKET_UNCONNECTED"), OSW_ERROR_WEBSOCKET_UNCONNECTED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WEBSOCKET_HANDSHAKE_FAILED"), OSW_ERROR_WEBSOCKET_HANDSHAKE_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_WEBSOCKET_PACK_FAILED"), OSW_ERROR_WEBSOCKET_PACK_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT"), OSW_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_TOO_MANY_SOCKET"), OSW_ERROR_SERVER_TOO_MANY_SOCKET);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_WORKER_TERMINATED"), OSW_ERROR_SERVER_WORKER_TERMINATED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_INVALID_LISTEN_PORT"), OSW_ERROR_SERVER_INVALID_LISTEN_PORT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_TOO_MANY_LISTEN_PORT"), OSW_ERROR_SERVER_TOO_MANY_LISTEN_PORT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_PIPE_BUFFER_FULL"), OSW_ERROR_SERVER_PIPE_BUFFER_FULL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_NO_IDLE_WORKER"), OSW_ERROR_SERVER_NO_IDLE_WORKER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_ONLY_START_ONE"), OSW_ERROR_SERVER_ONLY_START_ONE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_SEND_IN_MASTER"), OSW_ERROR_SERVER_SEND_IN_MASTER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_INVALID_REQUEST"), OSW_ERROR_SERVER_INVALID_REQUEST);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_CONNECT_FAIL"), OSW_ERROR_SERVER_CONNECT_FAIL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_WORKER_EXIT_TIMEOUT"), OSW_ERROR_SERVER_WORKER_EXIT_TIMEOUT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA"), OSW_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_SERVER_WORKER_UNPROCESSED_DATA"), OSW_ERROR_SERVER_WORKER_UNPROCESSED_DATA);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_OUT_OF_COROUTINE"), OSW_ERROR_CO_OUT_OF_COROUTINE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_HAS_BEEN_BOUND"), OSW_ERROR_CO_HAS_BEEN_BOUND);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_HAS_BEEN_DISCARDED"), OSW_ERROR_CO_HAS_BEEN_DISCARDED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_MUTEX_DOUBLE_UNLOCK"), OSW_ERROR_CO_MUTEX_DOUBLE_UNLOCK);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_BLOCK_OBJECT_LOCKED"), OSW_ERROR_CO_BLOCK_OBJECT_LOCKED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_BLOCK_OBJECT_WAITING"), OSW_ERROR_CO_BLOCK_OBJECT_WAITING);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_YIELD_FAILED"), OSW_ERROR_CO_YIELD_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_GETCONTEXT_FAILED"), OSW_ERROR_CO_GETCONTEXT_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_SWAPCONTEXT_FAILED"), OSW_ERROR_CO_SWAPCONTEXT_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_MAKECONTEXT_FAILED"), OSW_ERROR_CO_MAKECONTEXT_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_IOCPINIT_FAILED"), OSW_ERROR_CO_IOCPINIT_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_PROTECT_STACK_FAILED"), OSW_ERROR_CO_PROTECT_STACK_FAILED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_STD_THREAD_LINK_ERROR"), OSW_ERROR_CO_STD_THREAD_LINK_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_DISABLED_MULTI_THREAD"), OSW_ERROR_CO_DISABLED_MULTI_THREAD);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_CANNOT_CANCEL"), OSW_ERROR_CO_CANNOT_CANCEL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_NOT_EXISTS"), OSW_ERROR_CO_NOT_EXISTS);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_CANCELED"), OSW_ERROR_CO_CANCELED);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("ERROR_CO_TIMEDOUT"), OSW_ERROR_CO_TIMEDOUT);

    /**
     * trace log
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_SERVER"), OSW_TRACE_SERVER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CLIENT"), OSW_TRACE_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_BUFFER"), OSW_TRACE_BUFFER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CONN"), OSW_TRACE_CONN);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_EVENT"), OSW_TRACE_EVENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_WORKER"), OSW_TRACE_WORKER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_MEMORY"), OSW_TRACE_MEMORY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_REACTOR"), OSW_TRACE_REACTOR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_PHP"), OSW_TRACE_PHP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_HTTP"), OSW_TRACE_HTTP);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_HTTP2"), OSW_TRACE_HTTP2);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_EOF_PROTOCOL"), OSW_TRACE_EOF_PROTOCOL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_LENGTH_PROTOCOL"), OSW_TRACE_LENGTH_PROTOCOL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CLOSE"), OSW_TRACE_CLOSE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_WEBSOCKET"), OSW_TRACE_WEBSOCKET);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_REDIS_CLIENT"), OSW_TRACE_REDIS_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_MYSQL_CLIENT"), OSW_TRACE_MYSQL_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_HTTP_CLIENT"), OSW_TRACE_HTTP_CLIENT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_AIO"), OSW_TRACE_AIO);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_SSL"), OSW_TRACE_SSL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_NORMAL"), OSW_TRACE_NORMAL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CHANNEL"), OSW_TRACE_CHANNEL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_TIMER"), OSW_TRACE_TIMER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_SOCKET"), OSW_TRACE_SOCKET);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_COROUTINE"), OSW_TRACE_COROUTINE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CONTEXT"), OSW_TRACE_CONTEXT);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CO_HTTP_SERVER"), OSW_TRACE_CO_HTTP_SERVER);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_TABLE"), OSW_TRACE_TABLE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CO_CURL"), OSW_TRACE_CO_CURL);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_CARES"), OSW_TRACE_CARES);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("TRACE_ALL"), OSW_TRACE_ALL);

    /**
     * log level
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_DEBUG"), OSW_LOG_DEBUG);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_TRACE"), OSW_LOG_TRACE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_INFO"), OSW_LOG_INFO);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_NOTICE"), OSW_LOG_NOTICE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_WARNING"), OSW_LOG_WARNING);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ERROR"), OSW_LOG_ERROR);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_NONE"), OSW_LOG_NONE);

    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ROTATION_SINGLE"), OSW_LOG_ROTATION_SINGLE);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ROTATION_MONTHLY"), OSW_LOG_ROTATION_MONTHLY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ROTATION_DAILY"), OSW_LOG_ROTATION_DAILY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ROTATION_HOURLY"), OSW_LOG_ROTATION_HOURLY);
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("LOG_ROTATION_EVERY_MINUTE"), OSW_LOG_ROTATION_EVERY_MINUTE);

    /**
     * limit
     */
    zend_declare_class_constant_long(openswoole_constants_ce, ZEND_STRL("IOV_MAX"), IOV_MAX);

    // clang-format on

    OSW_FUNCTION_ALIAS(CG(function_table), "openswoole_coroutine_create", CG(function_table), "go");
    OSW_FUNCTION_ALIAS(CG(function_table), "openswoole_coroutine_defer", CG(function_table), "defer");
    /* Backward-compat: swoole_coroutine_* function aliases */
    OSW_FUNCTION_ALIAS(CG(function_table), "openswoole_coroutine_create", CG(function_table), "swoole_coroutine_create");
    OSW_FUNCTION_ALIAS(CG(function_table), "openswoole_coroutine_defer", CG(function_table), "swoole_coroutine_defer");

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
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_LOG_DATE_WITH_MICROSECONDS"), "log_date_with_microseconds");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_LOG_ROTATION"), "log_rotation");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPLAY_ERRORS"), "display_errors");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DNS_SERVER"), "dns_server");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_DNS_TIMEOUT"), "socket_dns_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_CONNECT_TIMEOUT"), "socket_connect_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_WRITE_TIMEOUT"), "socket_write_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_SEND_TIMEOUT"), "socket_send_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_READ_TIMEOUT"), "socket_read_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_RECV_TIMEOUT"), "socket_recv_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_BUFFER_SIZE"), "socket_buffer_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_TIMEOUT"), "socket_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONCURRENCY"), "max_concurrency");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_SERVER_TOKEN"), "enable_server_token");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_REACTOR_TYPE"), "reactor_type");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_AIO_CORE_WORKER_NUM"), "aio_core_worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_AIO_WORKER_NUM"), "aio_worker_num");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_AIO_MAX_WAIT_TIME"), "aio_max_wait_time");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_AIO_MAX_IDLE_TIME"), "aio_max_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_SIGNALFD"), "enable_signalfd");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WAIT_SIGNAL"), "wait_signal");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_REFRESH_TIME"), "dns_cache_refresh_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_THREAD_NUM"), "thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MIN_THREAD_NUM"), "min_thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_THREAD_NUM"), "max_thread_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKET_DONTWAIT"), "socket_dontwait");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_DNS_LOOKUP_RANDOM"), "dns_lookup_random");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_USE_ASYNC_RESOLVER"), "use_async_resolver");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_COROUTINE"), "enable_coroutine");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PROTOCOLS"), "ssl_protocols");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_COMPRESS"), "ssl_compress");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CERT_FILE"), "ssl_cert_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_KEY_FILE"), "ssl_key_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PASSPHRASE"), "ssl_passphrase");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_HOST_NAME"), "ssl_host_name");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_VERIFY_PEER"), "ssl_verify_peer");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SSL_ALLOW_SELF_SIGNED"), "ssl_allow_self_signed");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CAFILE"), "ssl_cafile");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CAPATH"), "ssl_capath");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SSL_VERIFY_DEPTH"), "ssl_verify_depth");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CIPHERS"), "ssl_ciphers");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_EOF_CHECK"), "open_eof_check");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_EOF_SPLIT"), "open_eof_split");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_EOF"), "package_eof");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_MQTT_PROTOCOL"), "open_mqtt_protocol");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_LENGTH_CHECK"), "open_length_check");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_TYPE"), "package_length_type");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_OFFSET"), "package_length_offset");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_BODY_OFFSET"), "package_body_offset");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_LENGTH_FUNC"), "package_length_func");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_MAX_LENGTH"), "package_max_length");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_HIGH_WATERMARK"), "buffer_high_watermark");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_LOW_WATERMARK"), "buffer_low_watermark");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BIND_PORT"), "bind_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BIND_ADDRESS"), "bind_address");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_TCP_NODELAY"), "open_tcp_nodelay");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_HOST"), "socks5_host");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_PORT"), "socks5_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_USERNAME"), "socks5_username");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SOCKS5_PASSWORD"), "socks5_password");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_HOST"), "http_proxy_host");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_PORT"), "http_proxy_port");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_USERNAME"), "http_proxy_username");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_USER"), "http_proxy_user");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PROXY_PASSWORD"), "http_proxy_password");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TIMEOUT"), "timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CONNECT_TIMEOUT"), "connect_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_READ_TIMEOUT"), "read_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WRITE_TIMEOUT"), "write_timeout");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SSL_DISABLE_COMPRESSION"), "ssl_disable_compression");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CORO_NUM"), "max_coro_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_COROUTINE"), "max_coroutine");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_DEADLOCK_CHECK"), "enable_deadlock_check");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HOOK_FLAGS"), "hook_flags");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_PREEMPTIVE_SCHEDULER"), "enable_preemptive_scheduler");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_C_STACK_SIZE"), "c_stack_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_STACK_SIZE"), "stack_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_EXPIRE"), "dns_cache_expire");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_DNS_CACHE_CAPACITY"), "dns_cache_capacity");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_HEADER_TABLE_SIZE"), "http2_header_table_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_INITIAL_WINDOW_SIZE"), "http2_initial_window_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_CONCURRENT_STREAMS"), "http2_max_concurrent_streams");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_FRAME_SIZE"), "http2_max_frame_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP2_MAX_HEADER_LIST_SIZE"), "http2_max_header_list_size");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_RECONNECT"), "reconnect");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DEFER"), "defer");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_LOWERCASE_HEADER"), "lowercase_header");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_KEEP_ALIVE"), "keep_alive");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_MASK"), "websocket_mask");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_COMPRESSION"), "http_compression");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_BODY_DECOMPRESSION"), "body_decompression");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_COMPRESSION"), "websocket_compression");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_COOKIE"), "http_parse_cookie");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_POST"), "http_parse_post");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_PARSE_FILES"), "http_parse_files");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_COMPRESSION_LEVEL"), "http_compression_level");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_GZIP_LEVEL"), "http_gzip_level");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_COMPRESSION_MIN_LENGTH"), "compression_min_length");
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
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_COMPATIBILITY_MODE"), "compatibility_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_CHROOT"), "chroot");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_GROUP"), "group");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DAEMONIZE"), "daemonize");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_PID_FILE"), "pid_file");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_REACTOR_NUM"), "reactor_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SINGLE_THREAD"), "single_thread");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_WORKER_NUM"), "worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_WAIT_TIME"), "max_wait_time");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_MAX_QUEUED_BYTES"), "max_queued_bytes");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SEND_TIMEOUT"), "send_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPATCH_MODE"), "dispatch_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SEND_YIELD"), "send_yield");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DISPATCH_FUNC"), "dispatch_func");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_DISCARD_TIMEOUT_REQUEST"), "discard_timeout_request");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_UNSAFE_EVENT"), "enable_unsafe_event");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_DELAY_RECEIVE"), "enable_delay_receive");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_REUSE_PORT"), "enable_reuse_port");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_USE_OBJECT"), "task_use_object");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_OBJECT"), "task_object");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_EVENT_OBJECT"), "event_object");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TASK_ENABLE_COROUTINE"), "task_enable_coroutine");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_WORKER_NUM"), "task_worker_num");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_IPC_MODE"), "task_ipc_mode");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TASK_TMPDIR"), "task_tmpdir");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TASK_MAX_REQUEST"), "task_max_request");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TASK_MAX_REQUEST_GRACE"), "task_max_request_grace");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONNECTION"), "max_connection");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_CONN"), "max_conn");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_START_SESSION_ID"), "start_session_id");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HEARTBEAT_CHECK_INTERVAL"), "heartbeat_check_interval");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HEARTBEAT_IDLE_TIME"), "heartbeat_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST"), "max_request");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST_GRACE"), "max_request_grace");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_MAX_REQUEST_EXECUTION_TIME"), "max_request_execution_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_RELOAD_ASYNC"), "reload_async");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_CPU_AFFINITY"), "open_cpu_affinity");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_CPU_AFFINITY_IGNORE"), "cpu_affinity_ignore");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_ENABLE_STATIC_HANDLER"), "enable_static_handler");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_DOCUMENT_ROOT"), "document_root");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_AUTOINDEX"), "http_autoindex");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_HTTP_INDEX_FILES"), "http_index_files");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_STATIC_HANDLER_LOCATIONS"), "static_handler_locations");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_INPUT_BUFFER_SIZE"), "input_buffer_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_INPUT_SIZE"), "buffer_input_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OUTPUT_BUFFER_SIZE"), "output_buffer_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_BUFFER_OUTPUT_SIZE"), "buffer_output_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_MESSAGE_QUEUE_KEY"), "message_queue_key");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_BACKLOG"), "backlog");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_KERNEL_SOCKET_RECV_BUFFER_SIZE"), "kernel_socket_recv_buffer_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_KERNEL_SOCKET_SEND_BUFFER_SIZE"), "kernel_socket_send_buffer_size");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TCP_DEFER_ACCEPT"), "tcp_defer_accept");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_TCP_KEEPALIVE"), "open_tcp_keepalive");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_HTTP_PROTOCOL"), "open_http_protocol");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PROTOCOL"), "open_websocket_protocol");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_WEBSOCKET_SUBPROTOCOL"), "websocket_subprotocol");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_CLOSE_FRAME"), "open_websocket_close_frame");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PING_FRAME"), "open_websocket_ping_frame");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_WEBSOCKET_PONG_FRAME"), "open_websocket_pong_frame");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_HTTP2_PROTOCOL"), "open_http2_protocol");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_REDIS_PROTOCOL"), "open_redis_protocol");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_MAX_IDLE_TIME"), "max_idle_time");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPIDLE"), "tcp_keepidle");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPINTERVAL"), "tcp_keepinterval");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_KEEPCOUNT"), "tcp_keepcount");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_TCP_USER_TIMEOUT"), "tcp_user_timeout");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_TCP_FASTOPEN"), "tcp_fastopen");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_PACKAGE_BODY_START"), "package_body_start");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SSL_CLIENT_CERT_FILE"), "ssl_client_cert_file");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_SSL_PREFER_SERVER_CIPHERS"), "ssl_prefer_server_ciphers");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_ECDH_CURVE"), "ssl_ecdh_curve");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_DHPARAM"), "ssl_dhparam");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_SSL_SNI_CERTS"), "ssl_sni_certs");
    zend_declare_class_constant_string(openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_SSL"), "open_ssl");
    zend_declare_class_constant_string(
        openswoole_constants_ce, ZEND_STRL("OPTION_OPEN_FASTCGI_PROTOCOL"), "open_fastcgi_protocol");

    // backward compatibility

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_TCP", OSW_SOCK_TCP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_TCP6", OSW_SOCK_TCP6);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_UDP", OSW_SOCK_UDP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_UDP6", OSW_SOCK_UDP6);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_UNIX_DGRAM", OSW_SOCK_UNIX_DGRAM);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SOCK_UNIX_STREAM", OSW_SOCK_UNIX_STREAM);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TCP", OSW_SOCK_TCP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TCP6", OSW_SOCK_TCP6);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_UDP", OSW_SOCK_UDP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_UDP6", OSW_SOCK_UDP6);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_UNIX_DGRAM", OSW_SOCK_UNIX_DGRAM);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_UNIX_STREAM", OSW_SOCK_UNIX_STREAM);

    OSW_REGISTER_BOOL_CONSTANT("OPENSWOOLE_SOCK_SYNC", 0);
    OSW_REGISTER_BOOL_CONSTANT("OPENSWOOLE_SOCK_ASYNC", 1);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SYNC", OSW_FLAG_SYNC);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ASYNC", OSW_FLAG_ASYNC);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_KEEP", OSW_FLAG_KEEP);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SELECT", openswoole::Reactor::TYPE_SELECT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_EPOLL", openswoole::Reactor::TYPE_EPOLL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_KQUEUE", openswoole::Reactor::TYPE_KQUEUE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_POLL", openswoole::Reactor::TYPE_POLL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_IO_URING", openswoole::Reactor::TYPE_IO_URING);

#ifdef OSW_USE_OPENSSL
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL", OSW_SOCK_SSL);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv3_METHOD", OSW_SSLv3_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv3_SERVER_METHOD", OSW_SSLv3_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv3_CLIENT_METHOD", OSW_SSLv3_CLIENT_METHOD);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_METHOD", OSW_TLSv1_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_SERVER_METHOD", OSW_TLSv1_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_CLIENT_METHOD", OSW_TLSv1_CLIENT_METHOD);

#ifdef TLS1_1_VERSION
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_1_METHOD", OSW_TLSv1_1_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_1_SERVER_METHOD", OSW_TLSv1_1_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_1_CLIENT_METHOD", OSW_TLSv1_1_CLIENT_METHOD);
#endif

#ifdef TLS1_2_VERSION
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_2_METHOD", OSW_TLSv1_2_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_2_SERVER_METHOD", OSW_TLSv1_2_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLSv1_2_CLIENT_METHOD", OSW_TLSv1_2_CLIENT_METHOD);
#endif

#ifdef OSW_SUPPORT_DTLS
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_DTLS_SERVER_METHOD", OSW_DTLS_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_DTLS_CLIENT_METHOD", OSW_DTLS_CLIENT_METHOD);
#endif

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv23_METHOD", OSW_SSLv23_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv23_SERVER_METHOD", OSW_SSLv23_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSLv23_CLIENT_METHOD", OSW_SSLv23_CLIENT_METHOD);

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLS_METHOD", OSW_SSLv23_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLS_SERVER_METHOD", OSW_SSLv23_SERVER_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TLS_CLIENT_METHOD", OSW_SSLv23_CLIENT_METHOD);

#ifdef HAVE_SSL3
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_SSLv3", OSW_SSL_SSLv3);
#endif
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_TLSv1", OSW_SSL_TLSv1);

#ifdef TLS1_1_VERSION
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_TLSv1_1", OSW_SSL_TLSv1_1);
#endif

#ifdef TLS1_2_VERSION
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_TLSv1_2", OSW_SSL_TLSv1_2);
#endif

#ifdef TLS1_3_VERSION
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_TLSv1_3", OSW_SSL_TLSv1_3);
#endif

#ifdef OSW_SUPPORT_DTLS
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_DTLS", OSW_SSL_DTLS);
#endif
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_SSL_SSLv2", OSW_SSL_SSLv2);
#endif

    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_EVENT_READ", OSW_EVENT_READ);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_EVENT_WRITE", OSW_EVENT_WRITE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STRERROR_SYSTEM", OSW_STRERROR_SYSTEM);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STRERROR_GAI", OSW_STRERROR_GAI);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STRERROR_DNS", OSW_STRERROR_DNS);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STRERROR_SWOOLE", OSW_STRERROR_SWOOLE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_MALLOC_FAIL", OSW_ERROR_MALLOC_FAIL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SYSTEM_CALL_FAIL", OSW_ERROR_SYSTEM_CALL_FAIL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_PHP_FATAL_ERROR", OSW_ERROR_PHP_FATAL_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_NAME_TOO_LONG", OSW_ERROR_NAME_TOO_LONG);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_INVALID_PARAMS", OSW_ERROR_INVALID_PARAMS);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_QUEUE_FULL", OSW_ERROR_QUEUE_FULL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_OPERATION_NOT_SUPPORT", OSW_ERROR_OPERATION_NOT_SUPPORT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_PROTOCOL_ERROR", OSW_ERROR_PROTOCOL_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WRONG_OPERATION", OSW_ERROR_WRONG_OPERATION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_FILE_NOT_EXIST", OSW_ERROR_FILE_NOT_EXIST);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_FILE_TOO_LARGE", OSW_ERROR_FILE_TOO_LARGE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_FILE_EMPTY", OSW_ERROR_FILE_EMPTY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DNSLOOKUP_DUPLICATE_REQUEST", OSW_ERROR_DNSLOOKUP_DUPLICATE_REQUEST);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED", OSW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT", OSW_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DNSLOOKUP_UNSUPPORTED", OSW_ERROR_DNSLOOKUP_UNSUPPORTED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DNSLOOKUP_NO_SERVER", OSW_ERROR_DNSLOOKUP_NO_SERVER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_BAD_IPV6_ADDRESS", OSW_ERROR_BAD_IPV6_ADDRESS);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_UNREGISTERED_SIGNAL", OSW_ERROR_UNREGISTERED_SIGNAL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_EVENT_SOCKET_REMOVED", OSW_ERROR_EVENT_SOCKET_REMOVED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_CLOSED_BY_SERVER", OSW_ERROR_SESSION_CLOSED_BY_SERVER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_CLOSED_BY_CLIENT", OSW_ERROR_SESSION_CLOSED_BY_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_CLOSING", OSW_ERROR_SESSION_CLOSING);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_CLOSED", OSW_ERROR_SESSION_CLOSED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_NOT_EXIST", OSW_ERROR_SESSION_NOT_EXIST);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_INVALID_ID", OSW_ERROR_SESSION_INVALID_ID);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_DISCARD_TIMEOUT_DATA", OSW_ERROR_SESSION_DISCARD_TIMEOUT_DATA);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SESSION_DISCARD_DATA", OSW_ERROR_SESSION_DISCARD_DATA);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_OUTPUT_BUFFER_OVERFLOW", OSW_ERROR_OUTPUT_BUFFER_OVERFLOW);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_OUTPUT_SEND_YIELD", OSW_ERROR_OUTPUT_SEND_YIELD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_NOT_READY", OSW_ERROR_SSL_NOT_READY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_CANNOT_USE_SENFILE", OSW_ERROR_SSL_CANNOT_USE_SENFILE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_EMPTY_PEER_CERTIFICATE", OSW_ERROR_SSL_EMPTY_PEER_CERTIFICATE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_VERIFY_FAILED", OSW_ERROR_SSL_VERIFY_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_BAD_CLIENT", OSW_ERROR_SSL_BAD_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_BAD_PROTOCOL", OSW_ERROR_SSL_BAD_PROTOCOL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_RESET", OSW_ERROR_SSL_RESET);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SSL_HANDSHAKE_FAILED", OSW_ERROR_SSL_HANDSHAKE_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_PACKAGE_LENGTH_TOO_LARGE", OSW_ERROR_PACKAGE_LENGTH_TOO_LARGE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_PACKAGE_LENGTH_NOT_FOUND", OSW_ERROR_PACKAGE_LENGTH_NOT_FOUND);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_DATA_LENGTH_TOO_LARGE", OSW_ERROR_DATA_LENGTH_TOO_LARGE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_TASK_PACKAGE_TOO_BIG", OSW_ERROR_TASK_PACKAGE_TOO_BIG);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_TASK_DISPATCH_FAIL", OSW_ERROR_TASK_DISPATCH_FAIL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_TASK_TIMEOUT", OSW_ERROR_TASK_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP2_STREAM_ID_TOO_BIG", OSW_ERROR_HTTP2_STREAM_ID_TOO_BIG);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP2_STREAM_NO_HEADER", OSW_ERROR_HTTP2_STREAM_NO_HEADER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP2_STREAM_NOT_FOUND", OSW_ERROR_HTTP2_STREAM_NOT_FOUND);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP2_STREAM_IGNORE", OSW_ERROR_HTTP2_STREAM_IGNORE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_AIO_BAD_REQUEST", OSW_ERROR_AIO_BAD_REQUEST);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_AIO_CANCELED", OSW_ERROR_AIO_CANCELED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_AIO_TIMEOUT", OSW_ERROR_AIO_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CLIENT_NO_CONNECTION", OSW_ERROR_CLIENT_NO_CONNECTION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKET_CLOSED", OSW_ERROR_SOCKET_CLOSED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKET_POLL_TIMEOUT", OSW_ERROR_SOCKET_POLL_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKS5_UNSUPPORT_VERSION", OSW_ERROR_SOCKS5_UNSUPPORT_VERSION);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKS5_UNSUPPORT_METHOD", OSW_ERROR_SOCKS5_UNSUPPORT_METHOD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKS5_AUTH_FAILED", OSW_ERROR_SOCKS5_AUTH_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKS5_SERVER_ERROR", OSW_ERROR_SOCKS5_SERVER_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SOCKS5_HANDSHAKE_FAILED", OSW_ERROR_SOCKS5_HANDSHAKE_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP_PROXY_HANDSHAKE_ERROR", OSW_ERROR_HTTP_PROXY_HANDSHAKE_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP_INVALID_PROTOCOL", OSW_ERROR_HTTP_INVALID_PROTOCOL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP_PROXY_HANDSHAKE_FAILED", OSW_ERROR_HTTP_PROXY_HANDSHAKE_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_HTTP_PROXY_BAD_RESPONSE", OSW_ERROR_HTTP_PROXY_BAD_RESPONSE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WEBSOCKET_BAD_CLIENT", OSW_ERROR_WEBSOCKET_BAD_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WEBSOCKET_BAD_OPCODE", OSW_ERROR_WEBSOCKET_BAD_OPCODE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WEBSOCKET_UNCONNECTED", OSW_ERROR_WEBSOCKET_UNCONNECTED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WEBSOCKET_HANDSHAKE_FAILED", OSW_ERROR_WEBSOCKET_HANDSHAKE_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_WEBSOCKET_PACK_FAILED", OSW_ERROR_WEBSOCKET_PACK_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT",
                              OSW_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_TOO_MANY_SOCKET", OSW_ERROR_SERVER_TOO_MANY_SOCKET);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_WORKER_TERMINATED", OSW_ERROR_SERVER_WORKER_TERMINATED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_INVALID_LISTEN_PORT", OSW_ERROR_SERVER_INVALID_LISTEN_PORT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_TOO_MANY_LISTEN_PORT", OSW_ERROR_SERVER_TOO_MANY_LISTEN_PORT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_PIPE_BUFFER_FULL", OSW_ERROR_SERVER_PIPE_BUFFER_FULL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_NO_IDLE_WORKER", OSW_ERROR_SERVER_NO_IDLE_WORKER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_ONLY_START_ONE", OSW_ERROR_SERVER_ONLY_START_ONE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_SEND_IN_MASTER", OSW_ERROR_SERVER_SEND_IN_MASTER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_INVALID_REQUEST", OSW_ERROR_SERVER_INVALID_REQUEST);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_CONNECT_FAIL", OSW_ERROR_SERVER_CONNECT_FAIL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_WORKER_EXIT_TIMEOUT", OSW_ERROR_SERVER_WORKER_EXIT_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA",
                              OSW_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_SERVER_WORKER_UNPROCESSED_DATA", OSW_ERROR_SERVER_WORKER_UNPROCESSED_DATA);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_OUT_OF_COROUTINE", OSW_ERROR_CO_OUT_OF_COROUTINE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_HAS_BEEN_BOUND", OSW_ERROR_CO_HAS_BEEN_BOUND);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_HAS_BEEN_DISCARDED", OSW_ERROR_CO_HAS_BEEN_DISCARDED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_MUTEX_DOUBLE_UNLOCK", OSW_ERROR_CO_MUTEX_DOUBLE_UNLOCK);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_BLOCK_OBJECT_LOCKED", OSW_ERROR_CO_BLOCK_OBJECT_LOCKED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_BLOCK_OBJECT_WAITING", OSW_ERROR_CO_BLOCK_OBJECT_WAITING);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_YIELD_FAILED", OSW_ERROR_CO_YIELD_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_GETCONTEXT_FAILED", OSW_ERROR_CO_GETCONTEXT_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_SWAPCONTEXT_FAILED", OSW_ERROR_CO_SWAPCONTEXT_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_MAKECONTEXT_FAILED", OSW_ERROR_CO_MAKECONTEXT_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_IOCPINIT_FAILED", OSW_ERROR_CO_IOCPINIT_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_PROTECT_STACK_FAILED", OSW_ERROR_CO_PROTECT_STACK_FAILED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_STD_THREAD_LINK_ERROR", OSW_ERROR_CO_STD_THREAD_LINK_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_DISABLED_MULTI_THREAD", OSW_ERROR_CO_DISABLED_MULTI_THREAD);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_CANNOT_CANCEL", OSW_ERROR_CO_CANNOT_CANCEL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_NOT_EXISTS", OSW_ERROR_CO_NOT_EXISTS);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_CANCELED", OSW_ERROR_CO_CANCELED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_ERROR_CO_TIMEDOUT", OSW_ERROR_CO_TIMEDOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_SERVER", OSW_TRACE_SERVER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CLIENT", OSW_TRACE_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_BUFFER", OSW_TRACE_BUFFER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CONN", OSW_TRACE_CONN);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_EVENT", OSW_TRACE_EVENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_WORKER", OSW_TRACE_WORKER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_MEMORY", OSW_TRACE_MEMORY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_REACTOR", OSW_TRACE_REACTOR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_PHP", OSW_TRACE_PHP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_HTTP", OSW_TRACE_HTTP);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_HTTP2", OSW_TRACE_HTTP2);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_EOF_PROTOCOL", OSW_TRACE_EOF_PROTOCOL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_LENGTH_PROTOCOL", OSW_TRACE_LENGTH_PROTOCOL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CLOSE", OSW_TRACE_CLOSE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_WEBSOCKET", OSW_TRACE_WEBSOCKET);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_REDIS_CLIENT", OSW_TRACE_REDIS_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_MYSQL_CLIENT", OSW_TRACE_MYSQL_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_HTTP_CLIENT", OSW_TRACE_HTTP_CLIENT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_AIO", OSW_TRACE_AIO);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_SSL", OSW_TRACE_SSL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_NORMAL", OSW_TRACE_NORMAL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CHANNEL", OSW_TRACE_CHANNEL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_TIMER", OSW_TRACE_TIMER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_SOCKET", OSW_TRACE_SOCKET);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_COROUTINE", OSW_TRACE_COROUTINE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CONTEXT", OSW_TRACE_CONTEXT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CO_HTTP_SERVER", OSW_TRACE_CO_HTTP_SERVER);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_TABLE", OSW_TRACE_TABLE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CO_CURL", OSW_TRACE_CO_CURL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_CARES", OSW_TRACE_CARES);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TRACE_ALL", OSW_TRACE_ALL);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_DEBUG", OSW_LOG_DEBUG);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_TRACE", OSW_LOG_TRACE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_INFO", OSW_LOG_INFO);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_NOTICE", OSW_LOG_NOTICE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_WARNING", OSW_LOG_WARNING);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ERROR", OSW_LOG_ERROR);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_NONE", OSW_LOG_NONE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ROTATION_SINGLE", OSW_LOG_ROTATION_SINGLE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ROTATION_MONTHLY", OSW_LOG_ROTATION_MONTHLY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ROTATION_DAILY", OSW_LOG_ROTATION_DAILY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ROTATION_HOURLY", OSW_LOG_ROTATION_HOURLY);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_LOG_ROTATION_EVERY_MINUTE", OSW_LOG_ROTATION_EVERY_MINUTE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_IPC_NONE", OSW_IPC_NONE);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_IPC_UNIXSOCK", OSW_IPC_UNIXSOCK);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_IPC_SOCKET", OSW_IPC_SOCKET);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_DEFAULT", 0);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_JSON", 1);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_STATS_OPENMETRICS", 2);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_IOV_MAX", IOV_MAX);

    openswoole_init();

    // init bug report message
    bug_report_message_init();
    if (strcmp("cli", sapi_module.name) == 0 || strcmp("phpdbg", sapi_module.name) == 0) {
        OPENSWOOLE_G(cli) = 1;
    }

    OSW_INIT_CLASS_ENTRY_EX2(openswoole_exception,
                            "OpenSwoole\\Exception",
                            "openswoole_exception",
                            nullptr,
                            nullptr,
                            zend_ce_exception,
                            zend_get_std_object_handlers());

    OSW_INIT_CLASS_ENTRY_EX2(
        openswoole_error, "OpenSwoole\\Error", "openswoole_error", nullptr, nullptr, zend_ce_error, zend_get_std_object_handlers());

    /** <Sort by dependency> **/
    php_openswoole_event_minit(module_number);
    // base
    php_openswoole_util_minit(module_number);
    php_openswoole_atomic_minit(module_number);
    php_openswoole_lock_minit(module_number);
    php_openswoole_process_minit(module_number);
    php_openswoole_process_pool_minit(module_number);
    php_openswoole_table_minit(module_number);
    php_openswoole_timer_minit(module_number);
    // coroutine
    php_openswoole_coroutine_minit(module_number);
    php_openswoole_coroutine_system_minit(module_number);
    php_openswoole_coroutine_scheduler_minit(module_number);
    php_openswoole_channel_coro_minit(module_number);
    php_openswoole_runtime_minit(module_number);
    // client
    php_openswoole_socket_coro_minit(module_number);
    php_openswoole_client_minit(module_number);
    php_openswoole_client_coro_minit(module_number);
    php_openswoole_http_client_coro_minit(module_number);
#ifdef OSW_USE_HTTP2
    php_openswoole_http2_client_coro_minit(module_number);
#endif
    // server
    php_openswoole_server_minit(module_number);
    php_openswoole_server_port_minit(module_number);
    php_openswoole_http_request_minit(module_number);
    php_openswoole_http_response_minit(module_number);
    php_openswoole_http_server_minit(module_number);
    php_openswoole_websocket_server_minit(module_number);
#ifdef OSW_USE_POSTGRES
    php_openswoole_postgresql_coro_minit(module_number);
#endif

    OpenSwooleG.fatal_error = fatal_error;
    Socket::default_buffer_size = OPENSWOOLE_G(socket_buffer_size);
    OpenSwooleG.dns_cache_refresh_time = 60;

    // enable pcre.jit and use swoole extension on MacOS will lead to coredump, disable it temporarily
#if defined(PHP_PCRE_VERSION) && defined(HAVE_PCRE_JIT_SUPPORT) && __MACH__ &&              \
    !defined(OSW_DEBUG)
    PCRE_G(jit) = 0;
#endif

    zend::known_strings_init();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(openswoole) {
    openswoole_clean();
    zend::known_strings_dtor();
    php_openswoole_runtime_mshutdown();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(openswoole) {
    char buf[64];
    php_info_print_table_start();
    php_info_print_table_header(2, "OpenSwoole", "enabled");
    php_info_print_table_row(2, "Author", "OpenSwoole Group <hello@openswoole.com>");
    php_info_print_table_row(2, "Version", OPENSWOOLE_VERSION);
    snprintf(buf, sizeof(buf), "%s %s", __DATE__, __TIME__);
    php_info_print_table_row(2, "Built", buf);
#if defined(OSW_USE_THREAD_CONTEXT)
    php_info_print_table_row(2, "coroutine", "enabled with thread context");
#else
    if (openswoole::Coroutine::use_fiber_context) {
        php_info_print_table_row(2, "coroutine", "enabled with fiber context");
    }
#if defined(OSW_USE_ASM_CONTEXT)
    else {
        php_info_print_table_row(2, "coroutine", "enabled with boost asm context");
    }
#else
    else {
        php_info_print_table_row(2, "coroutine", "enabled with ucontext");
    }
#endif
#endif
#ifdef OSW_DEBUG
    php_info_print_table_row(2, "debug", "enabled");
#endif
#ifdef OSW_LOG_TRACE_OPEN
    php_info_print_table_row(2, "trace_log", "enabled");
#endif
#ifdef HAVE_IO_URING
    php_info_print_table_row(2, "io_uring", "enabled");
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
#ifdef OSW_USE_ACCEPT4
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
#ifdef OSW_SOCKETS
    php_info_print_table_row(2, "sockets", "enabled");
#endif
#ifdef OSW_USE_OPENSSL
#ifdef OPENSSL_VERSION_TEXT
    php_info_print_table_row(2, "openssl", OPENSSL_VERSION_TEXT);
#else
    php_info_print_table_row(2, "openssl", "enabled");
#endif
#ifdef OSW_SUPPORT_DTLS
    php_info_print_table_row(2, "dtls", "enabled");
#endif
#endif
#ifdef OSW_USE_HTTP2
    php_info_print_table_row(2, "http2", "enabled");
#endif
#ifdef OSW_USE_CURL
    php_info_print_table_row(2, "hook-curl", "enabled");
#endif
#ifdef HAVE_PCRE
    php_info_print_table_row(2, "pcre", "enabled");
#endif
#ifdef OSW_USE_CARES
    php_info_print_table_row(2, "c-ares", ares_version(nullptr));
#endif
#ifdef OSW_HAVE_ZLIB
#ifdef ZLIB_VERSION
    php_info_print_table_row(2, "zlib", ZLIB_VERSION);
#else
    php_info_print_table_row(2, "zlib", "enabled");
#endif
#endif
#ifdef OSW_HAVE_BROTLI
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
#ifdef OSW_USE_MYSQLND
    php_info_print_table_row(2, "mysqlnd", "enabled");
#endif
#ifdef OSW_USE_JEMALLOC
    php_info_print_table_row(2, "jemalloc", "enabled");
#endif
#ifdef OSW_USE_TCMALLOC
    php_info_print_table_row(2, "tcmalloc", "enabled");
#endif
#ifdef OSW_USE_POSTGRES
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
        osw_memset_zero(mem, size);
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

static openswoole::Allocator php_allocator{
    _sw_emalloc,
    _sw_ecalloc,
    _sw_erealloc,
    _sw_efree,
};

static openswoole::Allocator zend_string_allocator{
    _sw_zend_string_malloc,
    _sw_zend_string_calloc,
    _sw_zend_string_realloc,
    _sw_zend_string_free,
};

const openswoole::Allocator *osw_php_allocator() {
    return &php_allocator;
}

const openswoole::Allocator *osw_zend_string_allocator() {
    return &zend_string_allocator;
}

PHP_RINIT_FUNCTION(openswoole) {
    if (!OPENSWOOLE_G(cli)) {
        return SUCCESS;
    }

    OPENSWOOLE_G(req_status) = PHP_OPENSWOOLE_RINIT_BEGIN;
    OpenSwooleG.running = 1;

    php_openswoole_register_shutdown_function("openswoole_internal_call_user_shutdown_begin");

#ifdef ZEND_SIGNALS
    /* Disable warning even in ZEND_DEBUG because we may register our own signal handlers  */
    SIGG(check) = 0;
#endif

    php_openswoole_coroutine_rinit();
    php_openswoole_runtime_rinit();

    OPENSWOOLE_G(req_status) = PHP_OPENSWOOLE_RINIT_END;

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(openswoole) {
    if (!OPENSWOOLE_G(cli)) {
        return SUCCESS;
    }

    OPENSWOOLE_G(req_status) = PHP_OPENSWOOLE_RSHUTDOWN_BEGIN;

    rshutdown_callbacks.execute();

    openswoole_event_free();

    php_openswoole_server_rshutdown();
    php_openswoole_async_coro_rshutdown();
    php_openswoole_coroutine_rshutdown();
    php_openswoole_runtime_rshutdown();

    php_openswoole_process_clean();

    OpenSwooleG.running = 0;
    OPENSWOOLE_G(req_status) = PHP_OPENSWOOLE_RSHUTDOWN_END;

#ifdef PHP_STREAM_FLAG_NO_CLOSE
    auto php_openswoole_set_stdio_no_close = [](const char *name, size_t name_len) {
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
    php_openswoole_set_stdio_no_close(ZEND_STRL("STDOUT"));
    php_openswoole_set_stdio_no_close(ZEND_STRL("STDERR"));
#endif

    return SUCCESS;
}

static PHP_FUNCTION(openswoole_internal_call_user_shutdown_begin) {
    if (OPENSWOOLE_G(req_status) == PHP_OPENSWOOLE_RINIT_END) {
        OPENSWOOLE_G(req_status) = PHP_OPENSWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN;
        RETURN_TRUE;
    } else {
        php_error_docref(nullptr, E_WARNING, "can not call this function in user level");
        RETURN_FALSE;
    }
}