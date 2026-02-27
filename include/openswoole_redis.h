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

#include "openswoole_server.h"

#include <vector>
#include <string>

#define OSW_REDIS_RETURN_NIL "$-1\r\n"

#define OSW_REDIS_MAX_COMMAND_SIZE 64
#define OSW_REDIS_MAX_LINES 128
#define OSW_REDIS_MAX_STRING_SIZE 536870912  // 512M

namespace openswoole {
namespace redis {

enum State {
    STATE_RECEIVE_TOTAL_LINE,
    STATE_RECEIVE_LENGTH,
    STATE_RECEIVE_STRING,
};

enum ReplyType {
    REPLY_ERROR,
    REPLY_NIL,
    REPLY_STATUS,
    REPLY_INT,
    REPLY_STRING,
    REPLY_SET,
    REPLY_MAP,
};

static osw_inline const char *get_number(const char *p, int *_ret) {
    char *endptr;
    p++;
    int ret = strtol(p, &endptr, 10);
    if (strncmp(OSW_CRLF, endptr, OSW_CRLF_LEN) == 0) {
        p += (endptr - p) + OSW_CRLF_LEN;
        *_ret = ret;
        return p;
    } else {
        return nullptr;
    }
}

int recv_packet(Protocol *protocol, Connection *conn, String *buffer);
std::vector<std::string> parse(const char *data, size_t len);
bool format(String *buf);
bool format(String *buf, enum ReplyType type, const std::string &value);
bool format(String *buf, enum ReplyType type, long value);

}  // namespace redis
}  // namespace openswoole
