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

#ifndef PHP_OPENSWOOLE_PRIVATE_H
#define PHP_OPENSWOOLE_PRIVATE_H

// C++ build format macros must defined earlier
#ifdef __cplusplus
#define __STDC_FORMAT_MACROS
#endif

#include "php_openswoole.h"

#define OSW_HAVE_COUNTABLE 1

#include "openswoole_api.h"
#include "openswoole_async.h"

#ifdef OSW_HAVE_ZLIB
#include <zlib.h>
#endif

BEGIN_EXTERN_C()
#include <ext/date/php_date.h>
#include <ext/standard/url.h>
#include <ext/standard/info.h>
#include <ext/standard/php_array.h>
#include <ext/standard/php_var.h>
#include <ext/standard/basic_functions.h>
#include <ext/standard/php_http.h>

#define PHP_OPENSWOOLE_VERSION OPENSWOOLE_VERSION
#define OPENSWOOLE_VERSION_STRING "OpenSwoole-" OPENSWOOLE_VERSION
#define PHP_OPENSWOOLE_CLIENT_USE_POLL

extern PHPAPI int php_array_merge(zend_array *dest, zend_array *src);

#ifdef PHP_WIN32
#define PHP_OPENSWOOLE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_OPENSWOOLE_API __attribute__((visibility("default")))
#else
#define PHP_OPENSWOOLE_API
#endif

#define OSW_CHECK_RETURN(s)                                                                                             \
    if (s < 0) {                                                                                                       \
        RETURN_FALSE;                                                                                                  \
    } else {                                                                                                           \
        RETURN_TRUE;                                                                                                   \
    }
#define OSW_LOCK_CHECK_RETURN(s)                                                                                        \
    zend_long ___tmp_return_value = s;                                                                                 \
    if (___tmp_return_value == 0) {                                                                                    \
        RETURN_TRUE;                                                                                                   \
    } else {                                                                                                           \
        zend_update_property_long(NULL, OSW_Z8_OBJ_P(ZEND_THIS), OSW_STRL("errCode"), ___tmp_return_value);              \
        RETURN_FALSE;                                                                                                  \
    }

#define php_openswoole_fatal_error(level, fmt_str, ...)                                                                    \
    php_error_docref(NULL, level, (const char *) (fmt_str), ##__VA_ARGS__)

#define php_openswoole_error(level, fmt_str, ...)                                                                          \
    if (OPENSWOOLE_G(display_errors) || level == E_ERROR) php_openswoole_fatal_error(level, fmt_str, ##__VA_ARGS__)

#define php_openswoole_sys_error(level, fmt_str, ...)                                                                      \
    php_openswoole_error(level, fmt_str ", Error: %s[%d]", ##__VA_ARGS__, strerror(errno), errno)

#ifdef OSW_USE_OPENSSL
#ifndef HAVE_OPENSSL
#error "Enable openssl support, require openssl library"
#endif
#endif

#ifdef OSW_USE_CARES
#ifndef HAVE_CARES
#error "Enable c-ares support, require c-ares library"
#endif
#endif

#ifdef OSW_SOCKETS
#include "ext/sockets/php_sockets.h"
#define OPENSWOOLE_SOCKETS_SUPPORT
#endif

#if PHP_VERSION_ID < 80200
#error "require PHP version 8.2 or later"
#endif

#if defined(ZTS) && defined(OSW_USE_THREAD_CONTEXT)
#error "thread context cannot be used with ZTS"
#endif

//--------------------------------------------------------
#define OSW_MAX_FIND_COUNT 100  // for openswoole_server::connection_list
#define OSW_PHP_CLIENT_BUFFER_SIZE 65535
//--------------------------------------------------------
enum php_openswoole_client_callback_type {
    OSW_CLIENT_CB_onConnect = 1,
    OSW_CLIENT_CB_onReceive,
    OSW_CLIENT_CB_onClose,
    OSW_CLIENT_CB_onError,
    OSW_CLIENT_CB_onBufferFull,
    OSW_CLIENT_CB_onBufferEmpty,
#ifdef OSW_USE_OPENSSL
    OSW_CLIENT_CB_onSSLReady,
#endif
};
//---------------------------------------------------------
#define OSW_FLAG_KEEP (1u << 12)
#define OSW_FLAG_ASYNC (1u << 10)
#define OSW_FLAG_SYNC (1u << 11)
//---------------------------------------------------------
enum php_openswoole_fd_type {
    PHP_OPENSWOOLE_FD_STREAM_CLIENT = OSW_FD_STREAM_CLIENT,
    PHP_OPENSWOOLE_FD_DGRAM_CLIENT,
    PHP_OPENSWOOLE_FD_MYSQL,
    PHP_OPENSWOOLE_FD_REDIS,
    PHP_OPENSWOOLE_FD_HTTPCLIENT,
    PHP_OPENSWOOLE_FD_PROCESS_STREAM,
    PHP_OPENSWOOLE_FD_MYSQL_CORO,
    PHP_OPENSWOOLE_FD_REDIS_CORO,
    PHP_OPENSWOOLE_FD_POSTGRESQL,
    PHP_OPENSWOOLE_FD_SOCKET,
    PHP_OPENSWOOLE_FD_CO_CURL,
};
//---------------------------------------------------------
enum php_openswoole_req_status {
    PHP_OPENSWOOLE_RINIT_BEGIN,
    PHP_OPENSWOOLE_RINIT_END,
    PHP_OPENSWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN,
    PHP_OPENSWOOLE_RSHUTDOWN_BEGIN,
    PHP_OPENSWOOLE_RSHUTDOWN_END,
};
//---------------------------------------------------------

static osw_inline enum swSocketType php_openswoole_socktype(long type) {
    return (enum swSocketType)(type & (~OSW_FLAG_SYNC) & (~OSW_FLAG_ASYNC) & (~OSW_FLAG_KEEP) & (~OSW_SOCK_SSL));
}

extern zend_class_entry *openswoole_event_ce;
extern zend_class_entry *openswoole_timer_ce;
extern zend_class_entry *openswoole_socket_coro_ce;
extern zend_class_entry *openswoole_client_ce;
extern zend_class_entry *openswoole_server_ce;
extern zend_object_handlers openswoole_server_handlers;
extern zend_class_entry *openswoole_connection_iterator_ce;
extern zend_class_entry *openswoole_process_ce;
extern zend_class_entry *openswoole_http_server_ce;
extern zend_object_handlers openswoole_http_server_handlers;
extern zend_class_entry *openswoole_websocket_server_ce;
extern zend_class_entry *openswoole_websocket_frame_ce;
extern zend_class_entry *openswoole_server_port_ce;
extern zend_class_entry *openswoole_exception_ce;
extern zend_object_handlers openswoole_exception_handlers;
extern zend_class_entry *openswoole_error_ce;
extern zend_class_entry *openswoole_constants_ce;

//---------------------------------------------------------
//                  Coroutine API
//---------------------------------------------------------
PHP_FUNCTION(openswoole_coroutine_create);
PHP_FUNCTION(openswoole_coroutine_exec);
PHP_FUNCTION(openswoole_coroutine_gethostbyname);
PHP_FUNCTION(openswoole_coroutine_defer);
PHP_FUNCTION(openswoole_coroutine_socketpair);
PHP_FUNCTION(openswoole_test_kernel_coroutine);  // for tests
//---------------------------------------------------------
//                  error
//---------------------------------------------------------
#define OSW_STRERROR_SYSTEM 0
#define OSW_STRERROR_GAI 1
#define OSW_STRERROR_DNS 2
#define OSW_STRERROR_SWOOLE 9

/**
 * MINIT <Sort by dependency>
 * ==============================================================
 */
void php_openswoole_event_minit(int module_number);
// base
void php_openswoole_util_minit(int module_number);
void php_openswoole_atomic_minit(int module_number);
void php_openswoole_lock_minit(int module_number);
void php_openswoole_process_minit(int module_number);
void php_openswoole_process_pool_minit(int module_number);
void php_openswoole_table_minit(int module_number);
void php_openswoole_timer_minit(int module_number);
// coroutine
void php_openswoole_coroutine_minit(int module_number);
void php_openswoole_coroutine_system_minit(int module_number);
void php_openswoole_coroutine_scheduler_minit(int module_number);
void php_openswoole_channel_coro_minit(int module_number);
void php_openswoole_runtime_minit(int module_number);
// client
void php_openswoole_socket_coro_minit(int module_number);
void php_openswoole_client_minit(int module_number);
void php_openswoole_client_coro_minit(int module_number);
void php_openswoole_http_client_coro_minit(int module_number);
#ifdef OSW_USE_HTTP2
void php_openswoole_http2_client_coro_minit(int module_number);
#endif
// server
void php_openswoole_server_minit(int module_number);
void php_openswoole_server_port_minit(int module_number);
void php_openswoole_http_request_minit(int module_number);
void php_openswoole_http_response_minit(int module_number);
void php_openswoole_http_server_minit(int module_number);
void php_openswoole_websocket_server_minit(int module_number);
// other
#ifdef OSW_USE_POSTGRES
void php_openswoole_postgresql_coro_minit(int module_number);
#endif

/**
 * RINIT
 * ==============================================================
 */
void php_openswoole_coroutine_rinit();
void php_openswoole_runtime_rinit();

/**
 * RSHUTDOWN
 * ==============================================================
 */
void php_openswoole_async_coro_rshutdown();
void php_openswoole_coroutine_rshutdown();
void php_openswoole_runtime_rshutdown();
void php_openswoole_server_rshutdown();

int php_openswoole_reactor_init();
void php_openswoole_set_global_option(zend_array *vht);
void php_openswoole_set_coroutine_option(zend_array *vht);
void php_openswoole_set_aio_option(zend_array *vht);

// shutdown
void php_openswoole_register_shutdown_function(const char *function);
void php_openswoole_register_shutdown_function_prepend(const char *function);

// event
void php_openswoole_event_init();
void php_openswoole_event_wait();
void php_openswoole_event_exit();

/**
 * MSHUTDOWN
 * ==============================================================
 */
void php_openswoole_runtime_mshutdown();

static osw_inline zend_bool php_openswoole_websocket_frame_is_object(zval *zdata) {
    return Z_TYPE_P(zdata) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zdata), openswoole_websocket_frame_ce);
}

static osw_inline size_t php_openswoole_get_send_data(zval *zdata, char **str) {
    convert_to_string(zdata);
    *str = Z_STRVAL_P(zdata);
    return Z_STRLEN_P(zdata);
}

int php_openswoole_convert_to_fd(zval *zsocket);
int php_openswoole_convert_to_fd_ex(zval *zsocket, int *async);

#ifdef OPENSWOOLE_SOCKETS_SUPPORT
php_socket *php_openswoole_convert_to_socket(int sock);
#endif

zend_bool php_openswoole_signal_isset_handler(int signo);

/* PHP 7 compatibility patches */
#define osw_zend_bailout() zend_bailout()

// Fixed in php-7.2.3RC1 (https://github.com/php/php-src/commit/e88e83d3e5c33fcd76f08b23e1a2e4e8dc98ce41)
#if PHP_MAJOR_VERSION == 7 && ((PHP_MINOR_VERSION == 2 && PHP_RELEASE_VERSION < 3))
// See https://github.com/php/php-src/commit/0495bf5650995cd8f18d6a9909eb4c5dcefde669
// Then https://github.com/php/php-src/commit/2dcfd8d16f5fa69582015cbd882aff833075a34c
// See https://github.com/php/php-src/commit/52db03b3e52bfc886896925d050af79bc4dc1ba3
#if PHP_MINOR_VERSION == 2
#define OSW_ZEND_WRONG_PARAMETERS_COUNT_ERROR                                                                           \
    zend_wrong_parameters_count_error(_flags &ZEND_PARSE_PARAMS_THROW, _num_args, _min_num_args, _max_num_args)
#else
#define OSW_ZEND_WRONG_PARAMETERS_COUNT_ERROR zend_wrong_parameters_count_error(_num_args, _min_num_args, _max_num_args)
#endif

#undef ZEND_PARSE_PARAMETERS_START_EX

#define ZEND_PARSE_PARAMETERS_START_EX(flags, min_num_args, max_num_args)                                              \
    do {                                                                                                               \
        const int _flags = (flags);                                                                                    \
        int _min_num_args = (min_num_args);                                                                            \
        int _max_num_args = (max_num_args);                                                                            \
        int _num_args = EX_NUM_ARGS();                                                                                 \
        int _i;                                                                                                        \
        zval *_real_arg, *_arg = NULL;                                                                                 \
        zend_expected_type _expected_type = Z_EXPECTED_LONG;                                                           \
        char *_error = NULL;                                                                                           \
        zend_bool _dummy;                                                                                              \
        zend_bool _optional = 0;                                                                                       \
        int error_code = ZPP_ERROR_OK;                                                                                 \
        ((void) _i);                                                                                                   \
        ((void) _real_arg);                                                                                            \
        ((void) _arg);                                                                                                 \
        ((void) _expected_type);                                                                                       \
        ((void) _error);                                                                                               \
        ((void) _dummy);                                                                                               \
        ((void) _optional);                                                                                            \
                                                                                                                       \
        do {                                                                                                           \
            if (UNEXPECTED(_num_args < _min_num_args) ||                                                               \
                (UNEXPECTED(_num_args > _max_num_args) && EXPECTED(_max_num_args >= 0))) {                             \
                if (!(_flags & ZEND_PARSE_PARAMS_QUIET)) {                                                             \
                    OSW_ZEND_WRONG_PARAMETERS_COUNT_ERROR;                                                              \
                }                                                                                                      \
                error_code = ZPP_ERROR_FAILURE;                                                                        \
                break;                                                                                                 \
            }                                                                                                          \
            _i = 0;                                                                                                    \
            _real_arg = ZEND_CALL_ARG(execute_data, 0);
#endif

/* PHP 7.3 compatibility macro {{{*/

#ifndef GC_ADDREF
#define GC_ADDREF(ref) ++GC_REFCOUNT(ref)
#define GC_DELREF(ref) --GC_REFCOUNT(ref)
#endif

#ifndef ZEND_CLOSURE_OBJECT
#define ZEND_CLOSURE_OBJECT(func) (zend_object *) func->op_array.prototype
#endif

/* PHP 7.4 compatibility macro {{{*/
#ifndef ZEND_COMPILE_EXTENDED_STMT
#define ZEND_COMPILE_EXTENDED_STMT ZEND_COMPILE_EXTENDED_INFO
#endif

#ifndef ZVAL_EMPTY_ARRAY
#define ZVAL_EMPTY_ARRAY(zval) (array_init((zval)))
#endif
#ifndef RETVAL_EMPTY_ARRAY
#define RETVAL_EMPTY_ARRAY() ZVAL_EMPTY_ARRAY(return_value)
#endif
#ifndef RETURN_EMPTY_ARRAY
#define RETURN_EMPTY_ARRAY()                                                                                           \
    do {                                                                                                               \
        RETVAL_EMPTY_ARRAY();                                                                                          \
        return;                                                                                                        \
    } while (0)
#endif

#ifndef ZEND_THIS
#define ZEND_THIS (&EX(This))
#endif

#ifndef ZEND_THIS_OBJECT
#define ZEND_THIS_OBJECT Z_OBJ_P(ZEND_THIS)
#endif

#ifndef E_FATAL_ERRORS
#define E_FATAL_ERRORS (E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR | E_USER_ERROR | E_RECOVERABLE_ERROR | E_PARSE)
#endif
/*}}}*/

#define OSW_Z8_OBJ_P(zobj) Z_OBJ_P(zobj)

typedef ssize_t php_stream_size_t;

/* PHP 7 wrapper functions / macros */

//----------------------------------Zval API------------------------------------

// Deprecated: do not use it anymore
// do not use osw_copy_to_stack(return_value, foo);
#define osw_copy_to_stack(ptr, val)                                                                                     \
    do {                                                                                                               \
        (val) = *(zval *) (ptr);                                                                                       \
        (ptr) = &(val);                                                                                                \
    } while (0)

#define OSW_ZVAL_SOCKET(return_value, result) ZVAL_OBJ(return_value, &result->std)
#define OSW_Z_SOCKET_P(zsocket) Z_SOCKET_P(zsocket)

#ifndef ZVAL_IS_BOOL
static osw_inline zend_bool ZVAL_IS_BOOL(zval *v) {
    return Z_TYPE_P(v) == IS_TRUE || Z_TYPE_P(v) == IS_FALSE;
}
#endif

#ifndef ZVAL_IS_TRUE
static osw_inline zend_bool ZVAL_IS_TRUE(zval *v) {
    return Z_TYPE_P(v) == IS_TRUE;
}
#endif

#ifndef ZVAL_IS_FALSE
static osw_inline zend_bool ZVAL_IS_FALSE(zval *v) {
    return Z_TYPE_P(v) == IS_FALSE;
}
#endif

#ifndef ZVAL_IS_LONG
static osw_inline zend_bool ZVAL_IS_LONG(zval *v) {
    return Z_TYPE_P(v) == IS_LONG;
}
#endif

#ifndef ZVAL_IS_STRING
static osw_inline zend_bool ZVAL_IS_STRING(zval *v) {
    return Z_TYPE_P(v) == IS_STRING;
}
#endif

#ifndef Z_BVAL_P
static osw_inline zend_bool Z_BVAL_P(zval *v) {
    return Z_TYPE_P(v) == IS_TRUE;
}
#endif

#ifndef ZVAL_IS_ARRAY
static osw_inline zend_bool ZVAL_IS_ARRAY(zval *v) {
    return Z_TYPE_P(v) == IS_ARRAY;
}
#endif

#ifndef ZVAL_IS_OBJECT
static osw_inline zend_bool ZVAL_IS_OBJECT(zval *v) {
    return Z_TYPE_P(v) == IS_OBJECT;
}
#endif

static osw_inline zval *osw_malloc_zval() {
    return (zval *) emalloc(sizeof(zval));
}

static osw_inline zval *osw_zval_dup(zval *val) {
    zval *dup = osw_malloc_zval();
    memcpy(dup, val, sizeof(zval));
    return dup;
}

static osw_inline void osw_zval_free(zval *val) {
    zval_ptr_dtor(val);
    efree(val);
}

//----------------------------------Constant API------------------------------------

/* Helper macro: if name starts with "OPENSWOOLE_", also register a "SWOOLE_" alias.
 * Uses zend_register_*_constant directly so module_number is passed explicitly. */
#define _OSW_REGISTER_SWOOLE_ALIAS_LONG(name, value) do { \
    const char *_osw_prefix = "OPENSWOOLE_"; \
    if (strncmp(name, _osw_prefix, 11) == 0) { \
        const char *_osw_rest = (name) + 11; \
        size_t _osw_rest_len = strlen(_osw_rest); \
        size_t _osw_alias_len = 7 + _osw_rest_len; \
        char *_osw_alias = (char *) malloc(_osw_alias_len + 1); \
        memcpy(_osw_alias, "SWOOLE_", 7); \
        memcpy(_osw_alias + 7, _osw_rest, _osw_rest_len + 1); \
        zend_register_long_constant(_osw_alias, _osw_alias_len, (value), CONST_CS | CONST_PERSISTENT, module_number); \
        free(_osw_alias); \
    } \
} while (0)

#define _OSW_REGISTER_SWOOLE_ALIAS_DOUBLE(name, value) do { \
    const char *_osw_prefix = "OPENSWOOLE_"; \
    if (strncmp(name, _osw_prefix, 11) == 0) { \
        const char *_osw_rest = (name) + 11; \
        size_t _osw_rest_len = strlen(_osw_rest); \
        size_t _osw_alias_len = 7 + _osw_rest_len; \
        char *_osw_alias = (char *) malloc(_osw_alias_len + 1); \
        memcpy(_osw_alias, "SWOOLE_", 7); \
        memcpy(_osw_alias + 7, _osw_rest, _osw_rest_len + 1); \
        zend_register_double_constant(_osw_alias, _osw_alias_len, (value), CONST_CS | CONST_PERSISTENT, module_number); \
        free(_osw_alias); \
    } \
} while (0)

#define _OSW_REGISTER_SWOOLE_ALIAS_STRING(name, value) do { \
    const char *_osw_prefix = "OPENSWOOLE_"; \
    if (strncmp(name, _osw_prefix, 11) == 0) { \
        const char *_osw_rest = (name) + 11; \
        size_t _osw_rest_len = strlen(_osw_rest); \
        size_t _osw_alias_len = 7 + _osw_rest_len; \
        char *_osw_alias = (char *) malloc(_osw_alias_len + 1); \
        memcpy(_osw_alias, "SWOOLE_", 7); \
        memcpy(_osw_alias + 7, _osw_rest, _osw_rest_len + 1); \
        zend_register_string_constant(_osw_alias, _osw_alias_len, (char *)(value), CONST_CS | CONST_PERSISTENT, module_number); \
        free(_osw_alias); \
    } \
} while (0)

#define OSW_REGISTER_NULL_CONSTANT(name) REGISTER_NULL_CONSTANT(name, CONST_CS | CONST_PERSISTENT)
#define OSW_REGISTER_BOOL_CONSTANT(name, value) \
    REGISTER_BOOL_CONSTANT(name, value, CONST_CS | CONST_PERSISTENT); \
    _OSW_REGISTER_SWOOLE_ALIAS_LONG(name, (zend_long)(value))
#define OSW_REGISTER_LONG_CONSTANT(name, value) \
    REGISTER_LONG_CONSTANT(name, value, CONST_CS | CONST_PERSISTENT); \
    _OSW_REGISTER_SWOOLE_ALIAS_LONG(name, (zend_long)(value))
#define OSW_REGISTER_DOUBLE_CONSTANT(name, value) \
    REGISTER_DOUBLE_CONSTANT(name, value, CONST_CS | CONST_PERSISTENT); \
    _OSW_REGISTER_SWOOLE_ALIAS_DOUBLE(name, (double)(value))
#define OSW_REGISTER_STRING_CONSTANT(name, value) \
    REGISTER_STRING_CONSTANT(name, (char *) value, CONST_CS | CONST_PERSISTENT); \
    _OSW_REGISTER_SWOOLE_ALIAS_STRING(name, value)
#define OSW_REGISTER_STRINGL_CONSTANT(name, value)                                                                      \
    REGISTER_STRINGL_CONSTANT(name, (char *) value, CONST_CS | CONST_PERSISTENT)

//----------------------------------Number API-----------------------------------

#define osw_php_math_round(value, places, mode) _php_math_round(value, places, mode)

//----------------------------------String API-----------------------------------

#define OSW_PHP_OB_START(zoutput)                                                                                       \
    zval zoutput;                                                                                                      \
    do {                                                                                                               \
        php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
#define OSW_PHP_OB_END()                                                                                                \
    php_output_get_contents(&zoutput);                                                                                 \
    php_output_discard();                                                                                              \
    }                                                                                                                  \
    while (0)

static osw_inline zend_string *osw_zend_string_recycle(zend_string *s, size_t alloc_len, size_t real_len) {
    OSW_ASSERT(!ZSTR_IS_INTERNED(s));
    if (UNEXPECTED(alloc_len != real_len)) {
        if (alloc_len > OpenSwooleG.pagesize && alloc_len > real_len * 2) {
            s = zend_string_realloc(s, real_len, 0);
        } else {
            ZSTR_LEN(s) = real_len;
        }
    }
    ZSTR_VAL(s)[real_len] = '\0';
    return s;
}

//----------------------------------Array API------------------------------------

#define php_openswoole_array_length(zarray) zend_hash_num_elements(Z_ARRVAL_P(zarray))
#define php_openswoole_array_get_value(ht, str, v) ((v = zend_hash_str_find(ht, str, sizeof(str) - 1)) && !ZVAL_IS_NULL(v))
#define php_openswoole_array_get_value_ex(ht, str, v) ((v = zend_hash_str_find(ht, str, strlen(str))) && !ZVAL_IS_NULL(v))

static osw_inline int php_openswoole_array_length_safe(zval *zarray) {
    if (zarray && ZVAL_IS_ARRAY(zarray)) {
        return php_openswoole_array_length(zarray);
    } else {
        return 0;
    }
}

void php_openswoole_sha1(const char *str, int _len, uchar *digest);
void php_openswoole_sha256(const char *str, int _len, uchar *digest);

#define OSW_HASHTABLE_FOREACH_START(ht, _val)                                                                           \
    ZEND_HASH_FOREACH_VAL(ht, _val);                                                                                   \
    {
#define OSW_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, _val)                                                          \
    zend_string *_foreach_key;                                                                                         \
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, _foreach_key, _val);                                                             \
    if (!_foreach_key) {                                                                                               \
        k = NULL;                                                                                                      \
        klen = 0;                                                                                                      \
        ktype = 0;                                                                                                     \
    } else {                                                                                                           \
        k = ZSTR_VAL(_foreach_key), klen = ZSTR_LEN(_foreach_key);                                                     \
        ktype = 1;                                                                                                     \
    }                                                                                                                  \
    {
#define OSW_HASHTABLE_FOREACH_END()                                                                                     \
    }                                                                                                                  \
    ZEND_HASH_FOREACH_END();

static osw_inline void add_assoc_ulong_safe_ex(zval *arg, const char *key, size_t key_len, zend_ulong value) {
    if (osw_likely(value <= ZEND_LONG_MAX)) {
        add_assoc_long_ex(arg, key, key_len, value);
    } else {
        char buf[MAX_LENGTH_OF_LONG + 1];
        size_t len = osw_snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, value);
        add_assoc_stringl_ex(arg, key, key_len, buf, len);
    }
}

static osw_inline void add_assoc_ulong_safe(zval *arg, const char *key, zend_ulong value) {
    add_assoc_ulong_safe_ex(arg, key, strlen(key), value);
}

//----------------------------------Class API------------------------------------

#define OSW_Z_OBJCE_NAME_VAL_P(zobject) ZSTR_VAL(Z_OBJCE_P(zobject)->name)

/* Forward declaration */
static osw_inline int osw_zend_register_class_alias(const char *name, size_t name_len, zend_class_entry *ce);

/* Register a backward-compat "Swoole\*" alias for any "OpenSwoole\*" class.
 * If namespace_name starts with "OpenSwoole\\", register an alias with "Swoole\\" prefix.
 * Also "OpenSwoole\\" prefix for the old runtime prefix hack style. */
static osw_inline void osw_register_swoole_compat_alias(const char *namespace_name, zend_class_entry *ce) {
    const char *prefix = "OpenSwoole\\";
    size_t prefix_len = 11; /* strlen("OpenSwoole\\") */
    if (strncmp(namespace_name, prefix, prefix_len) == 0) {
        const char *rest = namespace_name + prefix_len;
        size_t rest_len = strlen(rest);
        /* Register "Swoole\\" + rest alias */
        size_t alias_len = 7 + rest_len; /* strlen("Swoole\\") = 7 */
        char *alias = (char *) emalloc(alias_len + 1);
        memcpy(alias, "Swoole\\", 7);
        memcpy(alias + 7, rest, rest_len);
        alias[alias_len] = '\0';
        osw_zend_register_class_alias(alias, alias_len, ce);
        efree(alias);
        /* Register "OpenSwoole\\" + rest alias (for old runtime prefix style) */
        size_t alias2_len = 11 + rest_len; /* strlen("OpenSwoole\\") = 11 */
        char *alias2 = (char *) emalloc(alias2_len + 1);
        memcpy(alias2, "OpenSwoole\\", 11);
        memcpy(alias2 + 11, rest, rest_len);
        alias2[alias2_len] = '\0';
        osw_zend_register_class_alias(alias2, alias2_len, ce);
        efree(alias2);
    }
}

/* Register a backward-compat "swoole_*" alias for any "openswoole_*" snake name. */
static osw_inline void osw_register_swoole_snake_compat_alias(const char *snake_name, zend_class_entry *ce) {
    if (!snake_name) return;
    const char *prefix = "openswoole_";
    size_t prefix_len = 11; /* strlen("openswoole_") */
    if (strncmp(snake_name, prefix, prefix_len) == 0) {
        const char *rest = snake_name + prefix_len;
        size_t rest_len = strlen(rest);
        size_t alias_len = 7 + rest_len; /* strlen("swoole_") = 7 */
        char *alias = (char *) emalloc(alias_len + 1);
        memcpy(alias, "swoole_", 7);
        memcpy(alias + 7, rest, rest_len);
        alias[alias_len] = '\0';
        osw_zend_register_class_alias(alias, alias_len, ce);
        efree(alias);
    }
}

/* PHP 7 class declaration macros */

#define OSW_INIT_CLASS_ENTRY_BASE(module, namespace_name, snake_name, short_name, methods, parent_ce)                   \
    do {                                                                                                               \
        zend_class_entry _##module##_ce = {};                                                                          \
        INIT_CLASS_ENTRY(_##module##_ce, namespace_name, methods);                                                     \
        module##_ce = zend_register_internal_class_ex(&_##module##_ce, parent_ce);                                     \
        osw_register_swoole_compat_alias(namespace_name, module##_ce);                                                 \
        if (snake_name) {                                                                                              \
            OSW_CLASS_ALIAS(snake_name, module);                                                                        \
            osw_register_swoole_snake_compat_alias(snake_name, module##_ce);                                           \
        }                                                                                                              \
        if (short_name) OSW_CLASS_ALIAS(short_name, module);                                                            \
    } while (0)

#define OSW_INIT_CLASS_ENTRY(module, namespace_name, snake_name, short_name, methods)                                   \
    OSW_INIT_CLASS_ENTRY_BASE(module, namespace_name, snake_name, short_name, methods, NULL);                           \
    memcpy(&module##_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers))

#define OSW_INIT_CLASS_ENTRY_EX(module, namespace_name, snake_name, short_name, methods, parent_module)                 \
    OSW_INIT_CLASS_ENTRY_BASE(module, namespace_name, snake_name, short_name, methods, parent_module##_ce);             \
    memcpy(&module##_handlers, &parent_module##_handlers, sizeof(zend_object_handlers))

#define OSW_INIT_CLASS_ENTRY_EX2(                                                                                       \
    module, namespace_name, snake_name, short_name, methods, parent_module_ce, parent_module_handlers)                 \
    OSW_INIT_CLASS_ENTRY_BASE(module, namespace_name, snake_name, short_name, methods, parent_module_ce);               \
    memcpy(&module##_handlers, parent_module_handlers, sizeof(zend_object_handlers))

// Data Object: no methods, no parent
#define OSW_INIT_CLASS_ENTRY_DATA_OBJECT(module, namespace_name)                                                        \
    OSW_INIT_CLASS_ENTRY_BASE(module, namespace_name, NULL, NULL, NULL, NULL);                                          \
    memcpy(&module##_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers))

#define OSW_CLASS_ALIAS(name, module)                                                                                   \
    do {                                                                                                               \
        if (name) {                                                                                                    \
            osw_zend_register_class_alias(ZEND_STRL(name), module##_ce);                                                \
        }                                                                                                              \
    } while (0)

#define OSW_SET_CLASS_NOT_SERIALIZABLE(module) module##_ce->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;

#define osw_zend_class_clone_deny NULL
#define OSW_SET_CLASS_CLONEABLE(module, _clone_obj) module##_handlers.clone_obj = _clone_obj

#define OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(module, _unset_property) module##_handlers.unset_property = _unset_property

#define OSW_SET_CLASS_CREATE(module, _create_object) module##_ce->create_object = _create_object

#define OSW_SET_CLASS_DTOR(module, _dtor_obj) module##_handlers.dtor_obj = _dtor_obj

#define OSW_SET_CLASS_FREE(module, _free_obj) module##_handlers.free_obj = _free_obj

#define OSW_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj)                                                \
    OSW_SET_CLASS_CREATE(module, _create_object);                                                                       \
    OSW_SET_CLASS_FREE(module, _free_obj)

#define OSW_SET_CLASS_CUSTOM_OBJECT(module, _create_object, _free_obj, _struct, _std)                                   \
    OSW_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj);                                                   \
    module##_handlers.offset = XtOffsetOf(_struct, _std)

#define OSW_PREVENT_USER_DESTRUCT()                                                                                     \
    do {                                                                                                               \
        if (osw_unlikely(!(GC_FLAGS(Z_OBJ_P(ZEND_THIS)) & IS_OBJ_DESTRUCTOR_CALLED))) {                                 \
            RETURN_NULL();                                                                                             \
        }                                                                                                              \
    } while (0)

#define OSW_FUNCTION_ALIAS(origin_function_table, origin, alias_function_table, alias)                                  \
    osw_zend_register_function_alias(origin_function_table, ZEND_STRL(origin), alias_function_table, ZEND_STRL(alias))

static osw_inline int osw_zend_register_function_alias(zend_array *origin_function_table,
                                                     const char *origin,
                                                     size_t origin_length,
                                                     zend_array *alias_function_table,
                                                     const char *alias,
                                                     size_t alias_length) {
    zend_string *lowercase_origin = zend_string_alloc(origin_length, 0);
    zend_str_tolower_copy(ZSTR_VAL(lowercase_origin), origin, origin_length);
    zend_function *origin_function = (zend_function *) zend_hash_find_ptr(origin_function_table, lowercase_origin);
    zend_string_release(lowercase_origin);
    if (UNEXPECTED(!origin_function)) {
        return FAILURE;
    }
    OSW_ASSERT(origin_function->common.type == ZEND_INTERNAL_FUNCTION);
    char *_alias = (char *) emalloc(alias_length + 1);
    ((char *) memcpy(_alias, alias, alias_length))[alias_length] = '\0';
    zend_function_entry zfe[] = {{_alias,
                                  origin_function->internal_function.handler,
                                  ((zend_internal_arg_info *) origin_function->common.arg_info) - 1,
                                  origin_function->common.num_args,
                                  0},
                                 PHP_FE_END};
    int ret =
        zend_register_functions(origin_function->common.scope, zfe, alias_function_table, origin_function->common.type);
    efree(_alias);
    return ret;
}

static osw_inline int osw_zend_register_class_alias(const char *name, size_t name_len, zend_class_entry *ce) {
    zend_string *_name;
    if (name[0] == '\\') {
        _name = zend_string_init(name, name_len, 1);
        zend_str_tolower_copy(ZSTR_VAL(_name), name + 1, name_len - 1);
    } else {
        _name = zend_string_init(name, name_len, 1);
        zend_str_tolower_copy(ZSTR_VAL(_name), name, name_len);
    }

    zend_string *_interned_name = zend_new_interned_string(_name);

    return zend_register_class_alias_ex(ZSTR_VAL(_interned_name), ZSTR_LEN(_interned_name), ce, 1);
}

static osw_inline zend_object *osw_zend_create_object(zend_class_entry *ce, zend_object_handlers *handlers) {
    zend_object *object = (zend_object *) zend_object_alloc(sizeof(zend_object), ce);
    zend_object_std_init(object, ce);
    object_properties_init(object, ce);
    object->handlers = handlers;
    return object;
}

static osw_inline zend_object *osw_zend_create_object_deny(zend_class_entry *ce) {
    zend_object *object;
    object = zend_objects_new(ce);
    /* Initialize default properties */
    if (EXPECTED(ce->default_properties_count != 0)) {
        zval *p = object->properties_table;
        zval *end = p + ce->default_properties_count;
        do {
            ZVAL_UNDEF(p);
            p++;
        } while (p != end);
    }
    zend_throw_error(NULL, "The object of %s can not be created for security reasons", ZSTR_VAL(ce->name));
    return object;
}

static osw_inline void osw_zend_class_unset_property_deny(zend_object *object, zend_string *member, void **cache_slot) {
    zend_class_entry *ce = object->ce;
    while (ce->parent) {
        ce = ce->parent;
    }
    OSW_ASSERT(ce->type == ZEND_INTERNAL_CLASS);
    if (EXPECTED(zend_hash_find(&ce->properties_info, member))) {
        zend_throw_error(NULL, "Property %s of class %s cannot be unset", ZSTR_VAL(member), ZSTR_VAL(object->ce->name));
        return;
    }
    std_object_handlers.unset_property(object, member, cache_slot);
}

static osw_inline zval *osw_zend_read_property(zend_class_entry *ce, zval *obj, const char *s, int len, int silent) {
    zval rv, *property = zend_read_property(ce, OSW_Z8_OBJ_P(obj), s, len, silent, &rv);
    if (UNEXPECTED(property == &EG(uninitialized_zval))) {
        zend_update_property_null(ce, OSW_Z8_OBJ_P(obj), s, len);
        return zend_read_property(ce, OSW_Z8_OBJ_P(obj), s, len, silent, &rv);
    }
    return property;
}

static osw_inline void osw_zend_update_property_null_ex(zend_class_entry *scope, zval *object, zend_string *s) {
    zval tmp;

    ZVAL_NULL(&tmp);
    zend_update_property_ex(scope, OSW_Z8_OBJ_P(object), s, &tmp);
}

static osw_inline zval *osw_zend_read_property_ex(zend_class_entry *ce, zval *obj, zend_string *s, int silent) {
    zval rv, *property = zend_read_property_ex(ce, OSW_Z8_OBJ_P(obj), s, silent, &rv);
    if (UNEXPECTED(property == &EG(uninitialized_zval))) {
        osw_zend_update_property_null_ex(ce, obj, s);
        return zend_read_property_ex(ce, OSW_Z8_OBJ_P(obj), s, silent, &rv);
    }
    return property;
}

static osw_inline zval *osw_zend_read_property_not_null(
    zend_class_entry *ce, zval *obj, const char *s, int len, int silent) {
    zval rv, *property = zend_read_property(ce, OSW_Z8_OBJ_P(obj), s, len, silent, &rv);
    zend_uchar type = Z_TYPE_P(property);
    return (type == IS_NULL || UNEXPECTED(type == IS_UNDEF)) ? NULL : property;
}

static osw_inline zval *osw_zend_read_property_not_null_ex(zend_class_entry *ce, zval *obj, zend_string *s, int silent) {
    zval rv, *property = zend_read_property_ex(ce, OSW_Z8_OBJ_P(obj), s, silent, &rv);
    zend_uchar type = Z_TYPE_P(property);
    return (type == IS_NULL || UNEXPECTED(type == IS_UNDEF)) ? NULL : property;
}

static osw_inline zval *osw_zend_update_and_read_property_array(zend_class_entry *ce, zval *obj, const char *s, int len) {
    zval ztmp;
    array_init(&ztmp);
    zend_update_property(ce, OSW_Z8_OBJ_P(obj), s, len, &ztmp);
    zval_ptr_dtor(&ztmp);
    return zend_read_property(ce, OSW_Z8_OBJ_P(obj), s, len, 1, &ztmp);
}

static osw_inline zval *osw_zend_read_and_convert_property_array(
    zend_class_entry *ce, zval *obj, const char *s, int len, int silent) {
    zval rv, *property = zend_read_property(ce, OSW_Z8_OBJ_P(obj), s, len, silent, &rv);
    if (Z_TYPE_P(property) != IS_ARRAY) {
        // NOTICE: if user unset the property, zend_read_property will return uninitialized_zval instead of NULL pointer
        if (UNEXPECTED(property == &EG(uninitialized_zval))) {
            property = osw_zend_update_and_read_property_array(ce, obj, s, len);
        } else {
            zval_ptr_dtor(property);
            array_init(property);
        }
    }

    return property;
}

#define OSW_RETURN_PROPERTY(name)                                                                                       \
    do {                                                                                                               \
        RETURN_ZVAL(osw_zend_read_property(Z_OBJCE_P(ZEND_THIS), ZEND_THIS, ZEND_STRL(name), 0), 1, 0);                 \
    } while (0)

#define RETURN_SW_STRING(buf)                                                                                          \
    do {                                                                                                               \
        RETURN_STRINGL(buf->str, buf->length);                                                                         \
    } while (0)

//----------------------------------Function API------------------------------------

/**
 * Notice (osw_zend_call_method_with_%u_params): If you don't want to check the return value, please set retval to NULL
 */
#define osw_zend_call_method_with_0_params(zobj, obj_ce, fn_ptr_ptr, fn_name, retval)                                   \
    zend_call_method_with_0_params(OSW_Z8_OBJ_P(zobj), obj_ce, fn_ptr_ptr, fn_name, retval)

#define osw_zend_call_method_with_1_params(zobj, obj_ce, fn_ptr_ptr, fn_name, retval, v1)                               \
    zend_call_method_with_1_params(OSW_Z8_OBJ_P(zobj), obj_ce, fn_ptr_ptr, fn_name, retval, v1)

#define osw_zend_call_method_with_2_params(zobj, obj_ce, fn_ptr_ptr, fn_name, retval, v1, v2)                           \
    zend_call_method_with_2_params(OSW_Z8_OBJ_P(zobj), obj_ce, fn_ptr_ptr, fn_name, retval, v1, v2)

static osw_inline int osw_zend_function_max_num_args(zend_function *function) {
    // https://github.com/php/php-src/commit/2646f7bcb98dcdd322ea21701c8bb101104ea619
    // zend_function.common.num_args don't include the variadic argument anymore.
    return (function->common.fn_flags & ZEND_ACC_VARIADIC) ? UINT32_MAX : function->common.num_args;
}

// TODO: remove it after remove async modules
static osw_inline zend_bool osw_zend_is_callable(zval *callable, int check_flags, char **callable_name) {
    zend_string *name;
    zend_bool ret = zend_is_callable(callable, check_flags, &name);
    *callable_name = estrndup(ZSTR_VAL(name), ZSTR_LEN(name));
    zend_string_release(name);
    return ret;
}

static osw_inline zend_bool osw_zend_is_callable_at_frame(zval *zcallable,
                                                        zval *zobject,
                                                        zend_execute_data *frame,
                                                        uint check_flags,
                                                        char **callable_name,
                                                        size_t *callable_name_len,
                                                        zend_fcall_info_cache *fci_cache,
                                                        char **error) {
    zend_string *name;
    zend_bool ret;
    ret = zend_is_callable_at_frame(zcallable, zobject ? Z_OBJ_P(zobject) : NULL, frame, check_flags, fci_cache, error);
    name = zend_get_callable_name_ex(zcallable, zobject ? Z_OBJ_P(zobject) : NULL);
    if (callable_name) {
        *callable_name = estrndup(ZSTR_VAL(name), ZSTR_LEN(name));
    }
    if (callable_name_len) {
        *callable_name_len = ZSTR_LEN(name);
    }
    zend_string_release(name);
    return ret;
}

static osw_inline zend_bool osw_zend_is_callable_ex(zval *zcallable,
                                                  zval *zobject,
                                                  uint check_flags,
                                                  char **callable_name,
                                                  size_t *callable_name_len,
                                                  zend_fcall_info_cache *fci_cache,
                                                  char **error) {
    return osw_zend_is_callable_at_frame(
        zcallable, zobject, NULL, check_flags, callable_name, callable_name_len, fci_cache, error);
}

/* this API can work well when retval is NULL */
static osw_inline int osw_zend_call_function_ex(
    zval *function_name, zend_fcall_info_cache *fci_cache, uint32_t param_count, zval *params, zval *retval) {
    zend_fcall_info fci;
    zval _retval;
    int ret;

    fci.size = sizeof(fci);
    fci.object = NULL;
    if (!fci_cache || !fci_cache->function_handler) {
        if (!function_name) {
            php_openswoole_fatal_error(E_WARNING, "Bad function");
            return FAILURE;
        }
        ZVAL_COPY_VALUE(&fci.function_name, function_name);
    } else {
        ZVAL_UNDEF(&fci.function_name);
    }
    fci.retval = retval ? retval : &_retval;
    fci.param_count = param_count;
    fci.params = params;
    fci.named_params = NULL;

    ret = zend_call_function(&fci, fci_cache);

    if (!retval) {
        zval_ptr_dtor(&_retval);
    }
    return ret;
}

/* we must check for exception immediately if we don't have chances to go back to ZendVM (e.g event loop) */
static osw_inline int osw_zend_call_function_ex2(
    zval *function_name, zend_fcall_info_cache *fci_cache, uint32_t param_count, zval *params, zval *retval) {
    int ret = osw_zend_call_function_ex(function_name, fci_cache, param_count, params, retval);
    if (UNEXPECTED(EG(exception))) {
        zend_exception_error(EG(exception), E_ERROR);
    }
    return ret;
}

static osw_inline int osw_zend_call_function_anyway(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache) {
    zval retval;
    zend_object *exception = EG(exception);
    if (exception) {
        EG(exception) = NULL;
    }
    if (!fci->retval) {
        fci->retval = &retval;
    }
    int ret = zend_call_function(fci, fci_cache);
    if (fci->retval == &retval) {
        zval_ptr_dtor(&retval);
    }
    if (exception) {
        EG(exception) = exception;
    }
    return ret;
}

static osw_inline void osw_zend_fci_params_persist(zend_fcall_info *fci) {
    if (fci->param_count > 0) {
        uint32_t i;
        zval *params = (zval *) ecalloc(fci->param_count, sizeof(zval));
        for (i = 0; i < fci->param_count; i++) {
            ZVAL_COPY(&params[i], &fci->params[i]);
        }
        fci->params = params;
    }
}

static osw_inline void osw_zend_fci_params_discard(zend_fcall_info *fci) {
    if (fci->param_count > 0) {
        uint32_t i;
        for (i = 0; i < fci->param_count; i++) {
            zval_ptr_dtor(&fci->params[i]);
        }
        efree(fci->params);
    }
}

static osw_inline void osw_zend_fci_cache_persist(zend_fcall_info_cache *fci_cache) {
    if (fci_cache->object) {
        GC_ADDREF(fci_cache->object);
    }
    if (fci_cache->function_handler->op_array.fn_flags & ZEND_ACC_CLOSURE) {
        GC_ADDREF(ZEND_CLOSURE_OBJECT(fci_cache->function_handler));
    }
}

static osw_inline void osw_zend_fci_cache_discard(zend_fcall_info_cache *fci_cache) {
    if (fci_cache->object) {
        OBJ_RELEASE(fci_cache->object);
    }
    if (fci_cache->function_handler->op_array.fn_flags & ZEND_ACC_CLOSURE) {
        OBJ_RELEASE(ZEND_CLOSURE_OBJECT(fci_cache->function_handler));
    }
}

/* use void* to match some C callback function pointers */
static osw_inline void osw_zend_fci_cache_free(void *fci_cache) {
    osw_zend_fci_cache_discard((zend_fcall_info_cache *) fci_cache);
    efree((zend_fcall_info_cache *) fci_cache);
}

//----------------------------------Misc API------------------------------------

static osw_inline int php_openswoole_check_reactor() {
    if (OPENSWOOLE_G(req_status) == PHP_OPENSWOOLE_RSHUTDOWN_BEGIN) {
        return -1;
    }
    if (osw_unlikely(!osw_reactor())) {
        return php_openswoole_reactor_init() == OSW_OK ? 1 : -1;
    } else {
        return 0;
    }
}

static osw_inline char *php_openswoole_format_date(char *format, size_t format_len, time_t ts, int localtime) {
    zend_string *time = php_format_date(format, format_len, ts, localtime);
    char *return_str = estrndup(ZSTR_VAL(time), ZSTR_LEN(time));
    zend_string_release(time);
    return return_str;
}

static osw_inline char *php_openswoole_url_encode(const char *value, size_t value_len, int *exten) {
    zend_string *str = php_url_encode(value, value_len);
    *exten = ZSTR_LEN(str);
    char *return_str = estrndup(ZSTR_VAL(str), ZSTR_LEN(str));
    zend_string_release(str);
    return return_str;
}

static osw_inline char *php_openswoole_http_build_query(zval *zdata, size_t *length, smart_str *formstr) {
#if PHP_VERSION_ID < 80300
    if (HASH_OF(zdata)) {
        php_url_encode_hash_ex(HASH_OF(zdata), formstr, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, (int) PHP_QUERY_RFC1738);
    } else {
#else
    if (HASH_OF(zdata)) {
        php_url_encode_hash_ex(HASH_OF(zdata), formstr, NULL, 0, NULL, NULL, NULL, (int) PHP_QUERY_RFC1738);
    } else {
#endif
        if (formstr->s) {
            smart_str_free(formstr);
        }
        return NULL;
    }
    if (!formstr->s) {
        return NULL;
    }
    smart_str_0(formstr);
    *length = formstr->s->len;
    return formstr->s->val;
}

END_EXTERN_C()

#endif /* PHP_OPENSWOOLE_PRIVATE_H */
