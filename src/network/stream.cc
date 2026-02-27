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
#include "openswoole_api.h"
#include "openswoole_string.h"
#include "openswoole_socket.h"
#include "openswoole_protocol.h"
#include "openswoole_client.h"

namespace openswoole {
namespace network {

static void Stream_onConnect(Client *cli) {
    Stream *stream = (Stream *) cli->object;
    if (stream->cancel) {
        cli->close();
    }
    *((uint32_t *) stream->buffer->str) = ntohl(stream->buffer->length - 4);
    if (cli->send(cli, stream->buffer->str, stream->buffer->length, 0) < 0) {
        cli->close();
    } else {
        delete stream->buffer;
        stream->buffer = nullptr;
    }
}

static void Stream_onError(Client *cli) {
    Stream *stream = (Stream *) cli->object;
    stream->errCode = openswoole_get_last_error();

    openswoole_error_log(OSW_LOG_WARNING,
                     OSW_ERROR_SERVER_CONNECT_FAIL,
                     " connect() failed (%d: %s) while connecting to worker process",
                     stream->errCode,
                     openswoole_strerror(stream->errCode));

    if (!stream->response) {
        return;
    }

    stream->response(stream, nullptr, 0);
    delete stream;
}

static void Stream_onReceive(Client *cli, const char *data, uint32_t length) {
    Stream *stream = (Stream *) cli->object;
    if (length == 4) {
        cli->socket->close_wait = 1;
    } else {
        stream->response(stream, data + 4, length - 4);
    }
}

static void Stream_onClose(Client *cli) {
    openswoole_event_defer(
        [](void *data) {
            Client *cli = (Client *) data;
            delete (Stream *) cli->object;
        },
        cli);
}

Stream::Stream(const char *dst_host, int dst_port, SocketType type) : client(type, true) {
    if (client.socket == nullptr) {
        return;
    }

    client.onConnect = Stream_onConnect;
    client.onReceive = Stream_onReceive;
    client.onError = Stream_onError;
    client.onClose = Stream_onClose;
    client.object = this;

    client.open_length_check = 1;
    set_protocol(&client.protocol);

    if (client.connect(&client, dst_host, dst_port, -1, 0) < 0) {
        openswoole_sys_warning("failed to connect to [%s:%d]", dst_host, dst_port);
        return;
    }
    connected = true;
}

Stream::~Stream() {
    if (buffer) {
        delete buffer;
    }
}

/**
 * Stream Protocol: Length(32bit/Network Byte Order) + Body
 */
void Stream::set_protocol(Protocol *protocol) {
    protocol->get_package_length = Protocol::default_length_func;
    protocol->package_length_size = 4;
    protocol->package_length_type = 'N';
    protocol->package_body_offset = 4;
    protocol->package_length_offset = 0;
}

void Stream::set_max_length(uint32_t max_length) {
    client.protocol.package_max_length = max_length;
}

int Stream::send(const char *data, size_t length) {
    if (buffer == nullptr) {
        buffer = new String(openswoole_size_align(length + 4, OpenSwooleG.pagesize));
        buffer->length = 4;
    }
    if (buffer->append(data, length) < 0) {
        return OSW_ERR;
    }
    return OSW_OK;
}

ssize_t Stream::recv_blocking(Socket *sock, void *__buf, size_t __len) {
    int tmp = 0;
    ssize_t ret = sock->recv_blocking(&tmp, sizeof(tmp), MSG_WAITALL);
    if (ret <= 0) {
        return OSW_ERR;
    }
    int length = (int) ntohl(tmp);
    if (length <= 0 || length > (int) __len) {
        return OSW_ERR;
    }
    return sock->recv_blocking(__buf, length, MSG_WAITALL);
}

}  // namespace network
}  // namespace openswoole
