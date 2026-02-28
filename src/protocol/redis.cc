/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012-2015 The Swoole Group                             |
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
#include "openswoole_string.h"
#include "openswoole_socket.h"
#include "openswoole_protocol.h"
#include "openswoole_server.h"
#include "openswoole_redis.h"

namespace openswoole {
namespace redis {

struct Request {
    uint8_t state;

    int n_lines_total;
    int n_lines_received;

    int n_bytes_total;
    int n_bytes_received;

    int offset;
};

int recv_packet(Protocol *protocol, Connection *conn, String *buffer) {
    const char *p, *pe;
    int ret;
    char *buf_ptr;
    size_t buf_size;

    Request *request;
    network::Socket *socket = conn->socket;

    if (conn->object == nullptr) {
        request = (Request *) osw_malloc(sizeof(Request));
        if (!request) {
            openswoole_warning("malloc(%ld) failed", sizeof(Request));
            return OSW_ERR;
        }
        osw_memset_zero(request, sizeof(Request));
        conn->object = request;
    } else {
        request = (Request *) conn->object;
    }

_recv_data:
    buf_ptr = buffer->str + buffer->length;
    buf_size = buffer->size - buffer->length;

    int n = socket->recv(buf_ptr, buf_size, 0);
    if (n < 0) {
        switch (socket->catch_error(errno)) {
        case OSW_ERROR:
            openswoole_sys_warning("recv from socket#%d failed", conn->fd);
            return OSW_OK;
        case OSW_CLOSE:
            return OSW_ERR;
        default:
            return OSW_OK;
        }
    } else if (n == 0) {
        return OSW_ERR;
    } else {
        buffer->length += n;

        if (strncmp(buffer->str + buffer->length - OSW_CRLF_LEN, OSW_CRLF, OSW_CRLF_LEN) != 0) {
            if (buffer->size < protocol->package_max_length) {
                uint32_t extend_size = openswoole_size_align(buffer->size * 2, OpenSwooleG.pagesize);
                if (extend_size > protocol->package_max_length) {
                    extend_size = protocol->package_max_length;
                }
                if (!buffer->extend(extend_size)) {
                    return OSW_ERR;
                }
            } else if (buffer->length == buffer->size) {
            _package_too_big:
                openswoole_warning("Package is too big. package_length=%ld", buffer->length);
                return OSW_ERR;
            }
            goto _recv_data;
        }

        p = buffer->str;
        pe = p + buffer->length;

        do {
            switch (request->state) {
            case STATE_RECEIVE_TOTAL_LINE:
                if (*p == '*' && (p = get_number(p, &ret))) {
                    request->n_lines_total = ret;
                    request->state = STATE_RECEIVE_LENGTH;
                    break;
                }
                if (p == nullptr) {
                    goto _failed;
                }
                /* no break */

            case STATE_RECEIVE_LENGTH:
                if (*p == '$' && (p = get_number(p, &ret))) {
                    if (ret < 0) {
                        break;
                    }
                    if (ret + (p - buffer->str) > protocol->package_max_length) {
                        goto _package_too_big;
                    }
                    request->n_bytes_total = ret;
                    request->state = STATE_RECEIVE_STRING;
                    break;
                }
                // integer
                else if (*p == ':' && (p = get_number(p, &ret))) {
                    break;
                }
                if (p == nullptr) {
                    goto _failed;
                }
                /* no break */

            case STATE_RECEIVE_STRING:
                if (pe - p < request->n_bytes_total - request->n_bytes_received) {
                    request->n_bytes_received += pe - p;
                    return OSW_OK;
                } else {
                    p += request->n_bytes_total + OSW_CRLF_LEN;
                    request->n_bytes_total = 0;
                    request->n_lines_received++;
                    request->state = STATE_RECEIVE_LENGTH;
                    buffer->offset = buffer->length;

                    if (request->n_lines_received == request->n_lines_total) {
                        if (protocol->onPackage(protocol, socket, buffer->str, buffer->length) < 0) {
                            return OSW_ERR;
                        }
                        if (socket->removed) {
                            return OSW_OK;
                        }
                        buffer->clear();
                        osw_memset_zero(request, sizeof(Request));
                        return OSW_OK;
                    }
                }
                break;

            default:
                goto _failed;
            }
        } while (p < pe);
    }
_failed:
    openswoole_warning("redis protocol error");
    return OSW_ERR;
}

bool format(String *buf) {
    return buf->append(OSW_STRL(OSW_REDIS_RETURN_NIL)) == OSW_OK;
}

bool format(String *buf, enum ReplyType type, const std::string &value) {
    if (type == REPLY_STATUS) {
        if (value.empty()) {
            return buf->append(OSW_STRL("+OK\r\n")) == OSW_OK;
        } else {
            return buf->format("+%.*s\r\n", value.length(), value.c_str()) > 0;
        }
    } else if (type == REPLY_ERROR) {
        if (value.empty()) {
            return buf->append(OSW_STRL("-ERR\r\n")) == OSW_OK;
        } else {
            return buf->format("-%.*s\r\n", value.length(), value.c_str()) > 0;
        }
    } else if (type == REPLY_STRING) {
        if (value.empty() or value.length() > OSW_REDIS_MAX_STRING_SIZE) {
            return false;
        } else {
            if (buf->format("$%zu\r\n", value.length()) == 0) {
                return false;
            }
            buf->append(value);
            buf->append(OSW_CRLF, OSW_CRLF_LEN);
            return true;
        }
    }
    return false;
}

bool format(String *buf, enum ReplyType type, long value) {
    return buf->format(":%" PRId64 "\r\n", value) > 0;
}

std::vector<std::string> parse(const char *data, size_t len) {
    int state = STATE_RECEIVE_TOTAL_LINE;

    const char *p = data;
    const char *pe = p + len;
    int ret;
    int length = 0;

    std::vector<std::string> result;
    do {
        switch (state) {
        case STATE_RECEIVE_TOTAL_LINE:
            if (*p == '*' && (p = get_number(p, &ret))) {
                state = STATE_RECEIVE_LENGTH;
                break;
            }
            /* no break */

        case STATE_RECEIVE_LENGTH:
            if (*p == '$' && (p = get_number(p, &ret))) {
                if (ret == -1) {
                    break;
                }
                length = ret;
                state = STATE_RECEIVE_STRING;
                break;
            }
            // integer
            else if (*p == ':' && (p = get_number(p, &ret))) {
                result.push_back(std::to_string(ret));
                break;
            }
            /* no break */

        case STATE_RECEIVE_STRING:
            result.push_back(std::string(p, length));
            p += length + OSW_CRLF_LEN;
            state = STATE_RECEIVE_LENGTH;
            break;

        default:
            break;
        }
    } while (p < pe);

    return result;
}
}  // namespace redis
}  // namespace openswoole
