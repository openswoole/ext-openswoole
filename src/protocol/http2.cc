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
#include "openswoole_socket.h"
#include "openswoole_http2.h"

using openswoole::Protocol;
using openswoole::network::Socket;

namespace openswoole {
namespace http2 {

// advertises the settings the server applies to the connection
int send_setting_frame(Protocol *protocol, Socket *_socket, const Settings *settings, bool send_max_header_list_size) {
    char setting_frame[OSW_HTTP2_FRAME_HEADER_SIZE + OSW_HTTP2_SETTING_OPTION_SIZE * 5];
    char *p = setting_frame + OSW_HTTP2_FRAME_HEADER_SIZE;

    auto add = [&p](uint16_t id, uint32_t value) {
        id = htons(id);
        value = htonl(value);
        memcpy(p, &id, sizeof(id));
        memcpy(p + 2, &value, sizeof(value));
        p += OSW_HTTP2_SETTING_OPTION_SIZE;
    };
    add(OSW_HTTP2_SETTING_HEADER_TABLE_SIZE, settings->header_table_size);
    add(OSW_HTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, settings->max_concurrent_streams);
    add(OSW_HTTP2_SETTINGS_INIT_WINDOW_SIZE, settings->window_size);
    add(OSW_HTTP2_SETTINGS_MAX_FRAME_SIZE, settings->max_frame_size);
    // the default is no limit, it is only sent when it is configured
    if (send_max_header_list_size) {
        add(OSW_HTTP2_SETTINGS_MAX_HEADER_LIST_SIZE, settings->max_header_list_size);
    }

    size_t length = p - setting_frame - OSW_HTTP2_FRAME_HEADER_SIZE;
    set_frame_header(setting_frame, OSW_HTTP2_TYPE_SETTINGS, length, 0, 0);
    return _socket->send(setting_frame, OSW_HTTP2_FRAME_HEADER_SIZE + length, 0);
}

/**
 +-----------------------------------------------+
 |                 Length (24)                   |
 +---------------+---------------+---------------+
 |   Type (8)    |   Flags (8)   |
 +-+-------------+---------------+-------------------------------+
 |R|                 Stream Identifier (31)                      |
 +=+=============================================================+
 |                   Frame Payload (0...)                      ...
 +---------------------------------------------------------------+
 */
ssize_t get_frame_length(Protocol *protocol, Socket *conn, const char *buf, uint32_t length) {
    if (length < OSW_HTTP2_FRAME_HEADER_SIZE) {
        return 0;
    }
    return get_length(buf) + OSW_HTTP2_FRAME_HEADER_SIZE;
}

const char *get_type(int type) {
    switch (type) {
    case OSW_HTTP2_TYPE_DATA:
        return "DATA";
    case OSW_HTTP2_TYPE_HEADERS:
        return "HEADERS";
    case OSW_HTTP2_TYPE_PRIORITY:
        return "PRIORITY";
    case OSW_HTTP2_TYPE_RST_STREAM:
        return "RST_STREAM";
    case OSW_HTTP2_TYPE_SETTINGS:
        return "SETTINGS";
    case OSW_HTTP2_TYPE_PUSH_PROMISE:
        return "PUSH_PROMISE";
    case OSW_HTTP2_TYPE_PING:
        return "PING";
    case OSW_HTTP2_TYPE_GOAWAY:
        return "GOAWAY";
    case OSW_HTTP2_TYPE_WINDOW_UPDATE:
        return "WINDOW_UPDATE";
    case OSW_HTTP2_TYPE_CONTINUATION:
        return "CONTINUATION";
    default:
        return "UNKOWN";
    }
}

int get_type_color(int type) {
    switch (type) {
    case OSW_HTTP2_TYPE_DATA:
    case OSW_HTTP2_TYPE_WINDOW_UPDATE:
        return OSW_COLOR_MAGENTA;
    case OSW_HTTP2_TYPE_HEADERS:
    case OSW_HTTP2_TYPE_SETTINGS:
    case OSW_HTTP2_TYPE_PUSH_PROMISE:
    case OSW_HTTP2_TYPE_CONTINUATION:
        return OSW_COLOR_GREEN;
    case OSW_HTTP2_TYPE_PING:
    case OSW_HTTP2_TYPE_PRIORITY:
        return OSW_COLOR_WHITE;
    case OSW_HTTP2_TYPE_RST_STREAM:
    case OSW_HTTP2_TYPE_GOAWAY:
    default:
        return OSW_COLOR_RED;
    }
}

}  // namespace http2
}  // namespace openswoole
