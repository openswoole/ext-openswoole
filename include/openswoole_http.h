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
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
 +----------------------------------------------------------------------+
 */
#pragma once

#include "openswoole.h"

enum swHttpVersion {
    OSW_HTTP_VERSION_10 = 1,
    OSW_HTTP_VERSION_11,
    OSW_HTTP_VERSION_2,
    OSW_HTTP_VERSION_3,
};

enum swHttpMethod {
    OSW_HTTP_DELETE = 1,
    OSW_HTTP_GET,
    OSW_HTTP_HEAD,
    OSW_HTTP_POST,
    OSW_HTTP_PUT,
    OSW_HTTP_PATCH,
    /* pathological */
    OSW_HTTP_CONNECT,
    OSW_HTTP_OPTIONS,
    OSW_HTTP_TRACE,
    /* webdav */
    OSW_HTTP_COPY,
    OSW_HTTP_LOCK,
    OSW_HTTP_MKCOL,
    OSW_HTTP_MOVE,
    OSW_HTTP_PROPFIND,
    OSW_HTTP_PROPPATCH,
    OSW_HTTP_UNLOCK,
    /* subversion */
    OSW_HTTP_REPORT,
    OSW_HTTP_MKACTIVITY,
    OSW_HTTP_CHECKOUT,
    OSW_HTTP_MERGE,
    /* upnp */
    OSW_HTTP_MSEARCH,
    OSW_HTTP_NOTIFY,
    OSW_HTTP_SUBSCRIBE,
    OSW_HTTP_UNSUBSCRIBE,
    /* proxy */
    OSW_HTTP_PURGE,
    /* Http2 */
    OSW_HTTP_PRI,
};

enum swHttpStatusCode {
    OSW_HTTP_CONTINUE = 100,
    OSW_HTTP_SWITCHING_PROTOCOLS = 101,
    OSW_HTTP_PROCESSING = 102,

    OSW_HTTP_OK = 200,
    OSW_HTTP_CREATED = 201,
    OSW_HTTP_ACCEPTED = 202,
    OSW_HTTP_NO_CONTENT = 204,
    OSW_HTTP_PARTIAL_CONTENT = 206,

    OSW_HTTP_SPECIAL_RESPONSE = 300,
    OSW_HTTP_MOVED_PERMANENTLY = 301,
    OSW_HTTP_MOVED_TEMPORARILY = 302,
    OSW_HTTP_SEE_OTHER = 303,
    OSW_HTTP_NOT_MODIFIED = 304,
    OSW_HTTP_TEMPORARY_REDIRECT = 307,
    OSW_HTTP_PERMANENT_REDIRECT = 308,

    OSW_HTTP_BAD_REQUEST = 400,
    OSW_HTTP_UNAUTHORIZED = 401,
    OSW_HTTP_FORBIDDEN = 403,
    OSW_HTTP_NOT_FOUND = 404,
    OSW_HTTP_NOT_ALLOWED = 405,
    OSW_HTTP_REQUEST_TIME_OUT = 408,
    OSW_HTTP_CONFLICT = 409,
    OSW_HTTP_LENGTH_REQUIRED = 411,
    OSW_HTTP_PRECONDITION_FAILED = 412,
    OSW_HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
    OSW_HTTP_REQUEST_URI_TOO_LARGE = 414,
    OSW_HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
    OSW_HTTP_RANGE_NOT_SATISFIABLE = 416,
    OSW_HTTP_MISDIRECTED_REQUEST = 421,
    OSW_HTTP_TOO_MANY_REQUESTS = 429,

    OSW_HTTP_INTERNAL_SERVER_ERROR = 500,
    OSW_HTTP_NOT_IMPLEMENTED = 501,
    OSW_HTTP_BAD_GATEWAY = 502,
    OSW_HTTP_SERVICE_UNAVAILABLE = 503,
    OSW_HTTP_GATEWAY_TIME_OUT = 504,
    OSW_HTTP_VERSION_NOT_SUPPORTED = 505,
    OSW_HTTP_INSUFFICIENT_STORAGE = 507
};

namespace openswoole {
namespace http_server {
//-----------------------------------------------------------------
struct Request {
  public:
    uint8_t method;
    uint8_t version;
    uchar excepted : 1;

    uchar header_parsed : 1;
    uchar tried_to_dispatch : 1;

    uchar known_length : 1;
    uchar keep_alive : 1;
    uchar chunked : 1;
    uchar nobody_chunked : 1;

    uint32_t url_offset_;
    uint32_t url_length_;

    uint32_t request_line_length_; /* without \r\n  */
    uint32_t header_length_;       /* include request_line_length + \r\n */
    uint32_t content_length_;

    String *buffer_;

  public:
    Request() {
        clean();
        buffer_ = nullptr;
    }
    inline void clean() {
        memset(this, 0, offsetof(Request, buffer_));
    }
    int get_protocol();
    int get_header_length();
    int get_chunked_body_length();
    void parse_header_info();
    std::string get_date_if_modified_since();
#ifdef OSW_HTTP_100_CONTINUE
    bool has_expect_header();
#endif
};

int get_method(const char *method_str, size_t method_len);
const char *get_method_string(int method);
const char *get_status_message(int code);
size_t url_decode(char *str, size_t len);
char *url_encode(char const *str, size_t len);

#ifdef OSW_USE_HTTP2
ssize_t get_package_length(Protocol *protocol, network::Socket *conn, const char *data, uint32_t length);
uint8_t get_package_length_size(network::Socket *conn);
int dispatch_frame(Protocol *protocol, network::Socket *conn, const char *data, uint32_t length);
#endif

//-----------------------------------------------------------------
}  // namespace http_server
}  // namespace openswoole
