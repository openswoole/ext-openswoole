/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2017-now OpenSwoole Group                             |
 | Copyright (c) 2012-2017 The Swoole Group                             |
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
namespace openswoole {
/**
 * return the package total length
 */
ssize_t Protocol::default_length_func(Protocol *protocol, network::Socket *socket, const char *data, uint32_t size) {
    uint16_t length_offset = protocol->package_length_offset;
    uint8_t package_length_size =
        protocol->get_package_length_size ? protocol->get_package_length_size(socket) : protocol->package_length_size;
    int32_t body_length;

    if (package_length_size == 0) {
        // protocol error
        return OSW_ERR;
    }
    /**
     * no have length field, wait more data
     */
    if (size < length_offset + package_length_size) {
        protocol->real_header_length = length_offset + package_length_size;
        return 0;
    }
    body_length = openswoole_unpack(protocol->package_length_type, data + length_offset);
    // Length error
    // Protocol length is not legitimate, out of bounds or exceed the allocated length
    if (body_length < 0) {
        openswoole_warning("invalid package (size=%d) from socket#%u<%s:%d>",
                       size,
                       socket->fd,
                       socket->info.get_ip(),
                       socket->info.get_port());
        return OSW_ERR;
    }
    openswoole_debug("length=%d", protocol->package_body_offset + body_length);

    // total package length
    return protocol->package_body_offset + body_length;
}

int Protocol::recv_split_by_eof(network::Socket *socket, String *buffer) {
    if (buffer->length < package_eof_len) {
        return OSW_CONTINUE;
    }

    int retval;

    ssize_t n = buffer->split(package_eof, package_eof_len, [&](const char *data, size_t length) -> int {
        if (onPackage(this, socket, data, length) < 0) {
            retval = OSW_CLOSE;
            return false;
        }
        if (socket->removed) {
            return false;
        }
        return true;
    });

    if (socket->removed) {
        return OSW_CLOSE;
    }

    if (n < 0) {
        return retval;
    } else if (n == 0) {
        return OSW_CONTINUE;
    } else if (n < (ssize_t) buffer->length) {
        off_t offset;
        buffer->reduce(n);
        offset = buffer->length - package_eof_len;
        buffer->offset = offset > 0 ? offset : 0;
    } else {
        buffer->clear();
    }

#ifdef OSW_USE_OPENSSL
    if (socket->ssl) {
        return OSW_CONTINUE;
    }
#endif

    return OSW_OK;
}

/**
 * @return OSW_ERR: close the connection
 * @return OSW_OK: continue
 */
int Protocol::recv_with_length_protocol(network::Socket *socket, String *buffer) {
    ssize_t package_length;
    uint8_t _package_length_size = get_package_length_size ? get_package_length_size(socket) : package_length_size;
    uint32_t recv_size;
    ssize_t recv_n = 0;

    // protocol error
    if (get_package_length_size && _package_length_size == 0) {
        return OSW_ERR;
    }

    if (socket->skip_recv) {
        socket->skip_recv = 0;
        goto _do_get_length;
    }

_do_recv:
    if (socket->removed) {
        return OSW_OK;
    }
    if (buffer->offset > 0) {
        recv_size = buffer->offset - buffer->length;
    } else {
        recv_size = package_length_offset + _package_length_size;
    }

    recv_n = socket->recv(buffer->str + buffer->length, recv_size, 0);
    if (recv_n < 0) {
        switch (socket->catch_error(errno)) {
        case OSW_ERROR:
            openswoole_sys_warning("recv(%d, %d) failed", socket->fd, recv_size);
            return OSW_OK;
        case OSW_CLOSE:
            return OSW_ERR;
        default:
            return OSW_OK;
        }
    } else if (recv_n == 0) {
        return OSW_ERR;
    } else {
        buffer->length += recv_n;

        if (socket->recv_wait) {
            if (buffer->length >= (size_t) buffer->offset) {
            _do_dispatch:
                if (onPackage(this, socket, buffer->str, buffer->offset) < 0) {
                    return OSW_ERR;
                }
                if (socket->removed) {
                    return OSW_OK;
                }
                socket->recv_wait = 0;

                if (buffer->length > (size_t) buffer->offset) {
                    buffer->reduce(buffer->offset);
                    goto _do_get_length;
                } else {
                    buffer->clear();
                }
            }
#ifdef OSW_USE_OPENSSL
            if (socket->ssl) {
                goto _do_recv;
            }
#endif
            return OSW_OK;
        } else {
        _do_get_length:
            package_length = get_package_length(this, socket, buffer->str, buffer->length);
            // invalid package, close connection.
            if (package_length < 0) {
                return OSW_ERR;
            }
            // no length
            else if (package_length == 0) {
                if (buffer->length == package_length_offset + package_length_size) {
                    openswoole_error_log(OSW_LOG_WARNING,
                                     OSW_ERROR_PACKAGE_LENGTH_NOT_FOUND,
                                     "bad request, no length found in %ld bytes",
                                     buffer->length);
                    return OSW_ERR;
                } else {
                    return OSW_OK;
                }
            } else if (package_length > package_max_length) {
                openswoole_error_log(OSW_LOG_WARNING,
                                 OSW_ERROR_PACKAGE_LENGTH_TOO_LARGE,
                                 "package is too big, remote_addr=%s:%d, length=%zu",
                                 socket->info.get_ip(),
                                 socket->info.get_port(),
                                 package_length);
                return OSW_ERR;
            }
            // get length success
            else {
                if (buffer->size < (size_t) package_length) {
                    if (!buffer->extend(package_length)) {
                        return OSW_ERR;
                    }
                }
                socket->recv_wait = 1;
                buffer->offset = package_length;

                if (buffer->length >= (size_t) package_length) {
                    goto _do_dispatch;
                } else {
                    goto _do_recv;
                }
            }
        }
    }
    return OSW_OK;
}

/**
 * @return OSW_ERR: close the connection
 * @return OSW_OK: continue
 */
int Protocol::recv_with_eof_protocol(network::Socket *socket, String *buffer) {
    bool recv_again = false;
    int buf_size;

_recv_data:
    buf_size = buffer->size - buffer->length;
    char *buf_ptr = buffer->str + buffer->length;

    if (buf_size > OSW_BUFFER_SIZE_STD) {
        buf_size = OSW_BUFFER_SIZE_STD;
    }

    int n = socket->recv(buf_ptr, buf_size, 0);
    if (n < 0) {
        switch (socket->catch_error(errno)) {
        case OSW_ERROR:
            openswoole_sys_warning("recv from socket#%d failed", socket->fd);
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

        if (buffer->length < package_eof_len) {
            return OSW_OK;
        }

        if (split_by_eof) {
            int retval = recv_split_by_eof(socket, buffer);
            if (retval == OSW_CONTINUE) {
                recv_again = true;
            } else if (retval == OSW_CLOSE) {
                return OSW_ERR;
            } else {
                return OSW_OK;
            }
        } else if (memcmp(buffer->str + buffer->length - package_eof_len, package_eof, package_eof_len) == 0) {
            buffer->offset = buffer->length;
            if (onPackage(this, socket, buffer->str, buffer->length) < 0) {
                return OSW_ERR;
            }
            if (socket->removed) {
                return OSW_OK;
            }
            buffer->clear();
#ifdef OSW_USE_OPENSSL
            if (socket->ssl && SSL_pending(socket->ssl) > 0) {
                goto _recv_data;
            }
#endif
            return OSW_OK;
        }

        // over max length, will discard
        if (buffer->length == package_max_length) {
            openswoole_warning("Package is too big. package_length=%d", (int) buffer->length);
            return OSW_ERR;
        }

        // buffer is full, may have not read data
        if (buffer->length == buffer->size) {
            recv_again = true;
            if (buffer->size < package_max_length) {
                uint32_t extend_size = openswoole_size_align(buffer->size * 2, OpenSwooleG.pagesize);
                if (extend_size > package_max_length) {
                    extend_size = package_max_length;
                }
                if (!buffer->extend(extend_size)) {
                    return OSW_ERR;
                }
            }
        }
        // no eof
        if (recv_again) {
            goto _recv_data;
        }
    }
    return OSW_OK;
}

}  // namespace openswoole
