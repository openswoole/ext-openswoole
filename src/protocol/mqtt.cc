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
 | Author: Xinhua Guo  <guoxinhua@swoole.com>                           |
 +----------------------------------------------------------------------+
 */

#include "openswoole_mqtt.h"
#include "openswoole_protocol.h"

using openswoole::network::Socket;

namespace openswoole {
namespace mqtt {

void print_package(Packet *pkg) {
    printf("type=%d, length=%d\n", pkg->type, pkg->length);
}

void set_protocol(Protocol *protocol) {
    protocol->package_length_size = OSW_MQTT_MAX_LENGTH_SIZE;
    protocol->package_length_offset = 1;
    protocol->package_body_offset = 0;
    protocol->get_package_length = get_package_length;
}

// recv variable_header packet twice may cause that the '*data' contain the payload data,
// but there's no chance to read the next mqtt request ,because MQTT client will recv ACK blocking
#define OSW_MQTT_RECV_LEN_AGAIN 0

ssize_t get_package_length(Protocol *protocol, Socket *conn, const char *data, uint32_t size) {
    //-1 cause the arg 'size' contain length_offset(1 byte len)
    uint32_t recv_variable_header_size = (size - 1);
    if (recv_variable_header_size < OSW_MQTT_MIN_LENGTH_SIZE) {  // recv continue
        return OSW_MQTT_RECV_LEN_AGAIN;
    }

    uint8_t byte;
    int mul = 1;
    ssize_t length = 0;
    ssize_t variable_header_byte_count = 0;
    while (1) {
        variable_header_byte_count++;
        byte = data[variable_header_byte_count];
        length += (byte & 127) * mul;
        mul *= 128;
        if ((byte & 128) == 0) {  // done! there is no surplus length byte
            break;
        }
        if (variable_header_byte_count >= OSW_MQTT_MAX_LENGTH_SIZE) {
            openswoole_error_log(OSW_LOG_WARNING,
                             OSW_ERROR_PACKAGE_LENGTH_TOO_LARGE,
                             "bad request, the variable header size is larger than %d",
                             OSW_MQTT_MAX_LENGTH_SIZE);
            return OSW_ERR;
        }
        if (variable_header_byte_count >= recv_variable_header_size) {  // length not enough
            return OSW_MQTT_RECV_LEN_AGAIN;
        }
    }
    // payload_length + variable_header_byte_count + length_offset(1)
    return length + variable_header_byte_count + 1;
}

}  // namespace mqtt
}  // namespace openswoole
