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

#pragma once

#include "php_openswoole_server.h"
#include "php_openswoole_http.h"

#include "openswoole_http.h"
#include "openswoole_websocket.h"
#include "openswoole_mime_type.h"

#ifdef OSW_USE_HTTP2
#include "openswoole_http2.h"
#endif

int openswoole_websocket_onMessage(openswoole::Server *serv, openswoole::RecvData *req);
int openswoole_websocket_onHandshake(openswoole::Server *serv, openswoole::ListenPort *port, openswoole::http::Context *ctx);
void openswoole_websocket_onOpen(openswoole::http::Context *ctx);
void openswoole_websocket_onRequest(openswoole::http::Context *ctx);
bool openswoole_websocket_handshake(openswoole::http::Context *ctx);

#ifdef OSW_USE_HTTP2

int openswoole_http2_server_onFrame(openswoole::Server *serv, openswoole::Connection *conn, openswoole::RecvData *req);
int openswoole_http2_server_parse(openswoole::http2::Session *client, const char *buf);
void openswoole_http2_server_session_free(openswoole::Connection *conn);
int openswoole_http2_server_ping(openswoole::http::Context *ctx);
int openswoole_http2_server_goaway(openswoole::http::Context *ctx,
                               zend_long error_code,
                               const char *debug_data,
                               size_t debug_data_len);

#endif
