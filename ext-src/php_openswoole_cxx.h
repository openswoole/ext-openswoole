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

#include "php_openswoole_private.h"
#include "php_openswoole_coroutine.h"
#include "openswoole_util.h"

#include <string>

// clang-format off
//----------------------------------Swoole known string------------------------------------

#define OSW_ZEND_KNOWN_STRINGS(_) \
    _(OSW_ZEND_STR_TYPE,                     "type") \
    _(OSW_ZEND_STR_HOST,                     "host") \
    _(OSW_ZEND_STR_PORT,                     "port") \
    _(OSW_ZEND_STR_SETTING,                  "setting") \
    _(OSW_ZEND_STR_ID,                       "id") \
    _(OSW_ZEND_STR_FD,                       "fd") \
    _(OSW_ZEND_STR_SOCK,                     "sock") \
    _(OSW_ZEND_STR_PIPE,                     "pipe") \
    _(OSW_ZEND_STR_HEADERS,                  "headers") \
    _(OSW_ZEND_STR_SET_COOKIE_HEADERS,       "set_cookie_headers") \
    _(OSW_ZEND_STR_REQUEST_METHOD,           "requestMethod") \
    _(OSW_ZEND_STR_REQUEST_HEADERS,          "requestHeaders") \
    _(OSW_ZEND_STR_REQUEST_BODY,             "requestBody") \
    _(OSW_ZEND_STR_UPLOAD_FILES,             "uploadFiles") \
    _(OSW_ZEND_STR_COOKIES,                  "cookies") \
    _(OSW_ZEND_STR_DOWNLOAD_FILE,            "downloadFile") \
    _(OSW_ZEND_STR_DOWNLOAD_OFFSET,          "downloadOffset") \
    _(OSW_ZEND_STR_TMPFILES,                 "tmpfiles") \
    _(OSW_ZEND_STR_HEADER,                   "header") \
    _(OSW_ZEND_STR_COOKIE,                   "cookie") \
    _(OSW_ZEND_STR_METHOD,                   "method") \
    _(OSW_ZEND_STR_PATH,                     "path") \
    _(OSW_ZEND_STR_DATA,                     "data") \
    _(OSW_ZEND_STR_PIPELINE,                 "pipeline") \
    _(OSW_ZEND_STR_USE_PIPELINE_READ,        "usePipelineRead") \
    _(OSW_ZEND_STR_TRAILER,                  "trailer") \
    _(OSW_ZEND_STR_MASTER_PID,               "master_pid") \
    _(OSW_ZEND_STR_CALLBACK,                 "callback") \
    _(OSW_ZEND_STR_VALUE,                    "value") \
    _(OSW_ZEND_STR_KEY,                      "key") \
    _(OSW_ZEND_STR_OPCODE,                   "opcode") \
    _(OSW_ZEND_STR_CODE,                     "code") \
    _(OSW_ZEND_STR_REASON,                   "reason") \
    _(OSW_ZEND_STR_FLAGS,                    "flags") \
    _(OSW_ZEND_STR_FINISH,                   "finish") \
    _(OSW_ZEND_STR_CURL_IS_CO,               "is_co") \
    _(OSW_ZEND_STR_CURL_OPTION,              "option") \

typedef enum osw_zend_known_string_id {
#define _SW_ZEND_STR_ID(id, str) id,
OSW_ZEND_KNOWN_STRINGS(_SW_ZEND_STR_ID)
#undef _SW_ZEND_STR_ID
    OSW_ZEND_STR_LAST_KNOWN
} osw_zend_known_string_id;

// clang-format on

#define OSW_ZSTR_KNOWN(idx) osw_zend_known_strings[idx]
extern zend_string **osw_zend_known_strings;

//----------------------------------Swoole known string------------------------------------

#define OSW_SET_CLASS_CREATE_WITH_ITS_OWN_HANDLERS(module)                                                              \
    module##_ce->create_object = [](zend_class_entry *ce) { return osw_zend_create_object(ce, &module##_handlers); }

OSW_API bool php_openswoole_is_enable_coroutine();
OSW_API zend_object *php_openswoole_create_socket_from_fd(int fd, enum swSocketType type);
OSW_API bool php_openswoole_export_socket(zval *zobject, openswoole::coroutine::Socket *_socket);
OSW_API zend_object *php_openswoole_dup_socket(int fd, enum swSocketType type);
OSW_API void php_openswoole_init_socket_object(zval *zobject, openswoole::coroutine::Socket *socket);
OSW_API openswoole::coroutine::Socket *php_openswoole_get_socket(zval *zobject);
#ifdef OSW_USE_OPENSSL
OSW_API bool php_openswoole_socket_set_ssl(openswoole::coroutine::Socket *sock, zval *zset);
#endif
OSW_API bool php_openswoole_socket_set_protocol(openswoole::coroutine::Socket *sock, zval *zset);
OSW_API bool php_openswoole_client_set(openswoole::coroutine::Socket *cli, zval *zset);
OSW_API php_stream *php_openswoole_create_stream_from_socket(php_socket_t _fd,
                                                        int domain,
                                                        int type,
                                                        int protocol STREAMS_DC);
OSW_API php_stream_ops *php_openswoole_get_ori_php_stream_stdio_ops();
OSW_API void php_openswoole_register_rshutdown_callback(openswoole::Callback cb, void *private_data);

// timer
OSW_API bool php_openswoole_timer_clear(openswoole::TimerNode *tnode);
OSW_API bool php_openswoole_timer_clear_all();

static inline bool php_openswoole_is_fatal_error() {
    if (PG(last_error_message)) {
        switch (PG(last_error_type)) {
        case E_ERROR:
        case E_CORE_ERROR:
        case E_USER_ERROR:
        case E_COMPILE_ERROR:
            return true;
        default:
            break;
        }
    }
    return false;
}

ssize_t php_openswoole_length_func(openswoole::Protocol *protocol,
                               openswoole::network::Socket *_socket,
                               const char *data,
                               uint32_t length);

#ifdef OSW_HAVE_ZLIB
#define php_openswoole_websocket_frame_pack php_openswoole_websocket_frame_pack_ex
#define php_openswoole_websocket_frame_object_pack php_openswoole_websocket_frame_object_pack_ex
#else
#define php_openswoole_websocket_frame_pack(buffer, zdata, opcode, flags, mask, allow_compress)                            \
    php_openswoole_websocket_frame_pack_ex(buffer, zdata, opcode, flags, mask, 0)
#define php_openswoole_websocket_frame_object_pack(buffer, zdata, mask, allow_compress)                                    \
    php_openswoole_websocket_frame_object_pack_ex(buffer, zdata, mask, 0)
#endif
int php_openswoole_websocket_frame_pack_ex(
    openswoole::String *buffer, zval *zdata, zend_long opcode, uint8_t flags, zend_bool mask, zend_bool allow_compress);
int php_openswoole_websocket_frame_object_pack_ex(openswoole::String *buffer,
                                              zval *zdata,
                                              zend_bool mask,
                                              zend_bool allow_compress);
void php_openswoole_websocket_frame_unpack(openswoole::String *data, zval *zframe);
void php_openswoole_websocket_frame_unpack_ex(openswoole::String *data, zval *zframe, uchar allow_uncompress);

openswoole::TaskId php_openswoole_task_pack(openswoole::EventData *task, zval *data);
zval *php_openswoole_task_unpack(openswoole::EventData *task_result);

#ifdef OSW_HAVE_ZLIB
int php_openswoole_zlib_decompress(z_stream *stream, openswoole::String *buffer, char *body, int length);
#endif

const openswoole::Allocator *osw_php_allocator();
const openswoole::Allocator *osw_zend_string_allocator();

namespace zend {
//-----------------------------------namespace begin--------------------------------------------
class String {
  public:
    String() {
        str = nullptr;
    }

    String(const char *_str, size_t len) {
        str = zend_string_init(_str, len, 0);
    }

    String(const std::string &_str) {
        str = zend_string_init(_str.c_str(), _str.length(), 0);
    }

    String(zval *v) {
        str = zval_get_string(v);
    }

    String(zend_string *v, bool copy) {
        if (copy) {
            str = zend_string_copy(v);
        } else {
            str = v;
        }
    }

    String(const String &o) {
        str = zend_string_copy(o.str);
    }

    String(String &&o) {
        str = o.str;
        o.str = nullptr;
    }

    void operator=(zval *v) {
        if (str) {
            zend_string_release(str);
        }
        str = zval_get_string(v);
    }

    String &operator=(String &&o) {
        str = o.str;
        o.str = nullptr;
        return *this;
    }

    String &operator=(const String &o) {
        str = zend_string_copy(o.str);
        return *this;
    }

    inline char *val() {
        return ZSTR_VAL(str);
    }

    inline size_t len() {
        return ZSTR_LEN(str);
    }

    inline zend_string *get() {
        return str;
    }

    void rtrim() {
        ZSTR_LEN(str) = openswoole::rtrim(val(), len());
    }

    inline const std::string to_std_string() {
        return std::string(val(), len());
    }

    inline char *dup() {
        return osw_likely(len() > 0) ? osw_strndup(val(), len()) : nullptr;
    }

    inline char *edup() {
        return osw_likely(len() > 0) ? estrndup(val(), len()) : nullptr;
    }

    inline void release() {
        if (str) {
            zend_string_release(str);
            str = nullptr;
        }
    }

    ~String() {
        release();
    }

  private:
    zend_string *str;
};

class KeyValue {
  public:
    zend_ulong index;
    zend_string *key;
    zval zvalue;

    KeyValue(zend_ulong _index, zend_string *_key, zval *_zvalue) {
        index = _index;
        key = _key ? zend_string_copy(_key) : nullptr;
        ZVAL_DEREF(_zvalue);
        zvalue = *_zvalue;
        Z_TRY_ADDREF(zvalue);
    }

    inline void add_to(zval *zarray) {
        HashTable *ht = Z_ARRVAL_P(zarray);
        zval *dest_elem = !key ? zend_hash_index_update(ht, index, &zvalue) : zend_hash_update(ht, key, &zvalue);
        Z_TRY_ADDREF_P(dest_elem);
    }

    ~KeyValue() {
        if (key) {
            zend_string_release(key);
        }
        zval_ptr_dtor(&zvalue);
    }
};

class ArrayIterator {
  public:
    ArrayIterator(Bucket *p) {
        _ptr = p;
        _key = _ptr->key;
        _val = &_ptr->val;
        _index = _ptr->h;
        pe = p;
    }
    ArrayIterator(Bucket *p, Bucket *_pe) {
        _ptr = p;
        _key = _ptr->key;
        _val = &_ptr->val;
        _index = _ptr->h;
        pe = _pe;
        skipUndefBucket();
    }
    void operator++(int i) {
        ++_ptr;
        skipUndefBucket();
    }
    bool operator!=(ArrayIterator b) {
        return b.ptr() != _ptr;
    }
    std::string key() {
        return std::string(_key->val, _key->len);
    }
    zend_ulong index() {
        return _index;
    }
    zval *value() {
        return _val;
    }
    Bucket *ptr() {
        return _ptr;
    }

  private:
    void skipUndefBucket() {
        while (_ptr != pe) {
            _val = &_ptr->val;
            if (_val && Z_TYPE_P(_val) == IS_INDIRECT) {
                _val = Z_INDIRECT_P(_val);
            }
            if (UNEXPECTED(Z_TYPE_P(_val) == IS_UNDEF)) {
                ++_ptr;
                continue;
            }
            if (_ptr->key) {
                _key = _ptr->key;
                _index = 0;
            } else {
                _index = _ptr->h;
                _key = nullptr;
            }
            break;
        }
    }

    zval *_val;
    zend_string *_key;
    Bucket *_ptr;
    Bucket *pe;
    zend_ulong _index;
};

class Array {
  public:
    zval *arr;

    Array(zval *_arr) {
        assert(Z_TYPE_P(_arr) == IS_ARRAY);
        arr = _arr;
    }

    inline size_t count() {
        return zend_hash_num_elements(Z_ARRVAL_P(arr));
    }

    inline bool set(zend_ulong index, zval *value) {
        return add_index_zval(arr, index, value) == SUCCESS;
    }

    inline bool append(zval *value) {
        return add_next_index_zval(arr, value) == SUCCESS;
    }

    inline bool set(zend_ulong index, zend_resource *res) {
        zval tmp;
        ZVAL_RES(&tmp, res);
        return set(index, &tmp);
    }

    ArrayIterator begin() {
        return ArrayIterator(Z_ARRVAL_P(arr)->arData, Z_ARRVAL_P(arr)->arData + Z_ARRVAL_P(arr)->nNumUsed);
    }

    ArrayIterator end() {
        return ArrayIterator(Z_ARRVAL_P(arr)->arData + Z_ARRVAL_P(arr)->nNumUsed);
    }
};

enum PipeType {
    PIPE_TYPE_NONE = 0,
    PIPE_TYPE_STREAM = 1,
    PIPE_TYPE_DGRAM = 2,
};

class Process {
  public:
    zend_object *zsocket = nullptr;
    enum PipeType pipe_type;
    bool enable_coroutine;

    Process(enum PipeType pipe_type, bool enable_coroutine)
        : pipe_type(pipe_type), enable_coroutine(enable_coroutine) {}

    ~Process() {
        if (zsocket) {
            OBJ_RELEASE(zsocket);
        }
    }
};

namespace function {
/* must use this API to call event callbacks to ensure that exceptions are handled correctly */
bool call(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv, zval *retval, const bool enable_coroutine);

class ReturnValue {
  public:
    zval value;
    ReturnValue() {
        value = {};
    }
    ~ReturnValue() {
        zval_dtor(&value);
    }
};

ReturnValue call(const std::string &func_name, int argc, zval *argv);
}  // namespace function

struct Function {
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    inline bool call(zval *retval, const bool enable_coroutine) {
        return function::call(&fci_cache, fci.param_count, fci.params, retval, enable_coroutine);
    }
};

bool eval(const std::string &code, const std::string &filename = "");
void known_strings_init(void);
void known_strings_dtor(void);

static inline zend_string *fetch_zend_string_by_val(void *val) {
    return (zend_string *) ((char *) val - XtOffsetOf(zend_string, val));
}

static inline void assign_zend_string_by_val(zval *zdata, char *addr, size_t length) {
    zend_string *zstr = fetch_zend_string_by_val(addr);
    addr[length] = 0;
    zstr->len = length;
    ZVAL_STR(zdata, zstr);
}

#define ZEND_STR_CONST const

#ifndef ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX
#define ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, allow_null) \
    ZEND_BEGIN_ARG_INFO_EX(name, 0, return_reference, required_num_args)
#endif

//-----------------------------------namespace end--------------------------------------------
}  // namespace zend
