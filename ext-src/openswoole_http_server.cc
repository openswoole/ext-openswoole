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

#include "php_openswoole_http_server.h"

using namespace openswoole;
using openswoole::coroutine::Socket;

using HttpRequest = openswoole::http::Request;
using HttpResponse = openswoole::http::Response;
using HttpContext = openswoole::http::Context;

namespace WebSocket = openswoole::websocket;

String *openswoole_http_buffer;
#ifdef OSW_HAVE_COMPRESSION
/* not only be used by zlib but also be used by br */
String *openswoole_zlib_buffer;
#endif
String *openswoole_http_form_data_buffer;

zend_class_entry *openswoole_http_server_ce;
zend_object_handlers openswoole_http_server_handlers;

static bool http_context_send_data(HttpContext *ctx, const char *data, size_t length);
static bool http_context_sendfile(HttpContext *ctx, const char *file, uint32_t l_file, off_t offset, size_t length);
static bool http_context_disconnect(HttpContext *ctx);

void php_openswoole_http_request_onTimeout(Timer *timer, TimerNode *tnode) {
    HttpContext *ctx = (HttpContext *) tnode->data;
    if (!ctx || (ctx->end_ || ctx->detached) || !ctx->fd) {
        return;
    }
    ctx->send(ctx, OSW_STRL(OSW_HTTP_REQUEST_TIMEOUT_PACKET));
    ctx->close(ctx);
}

int php_openswoole_http_server_onReceive(Server *serv, RecvData *req) {
    SessionId session_id = req->info.fd;
    int server_fd = req->info.server_fd;

    Connection *conn = serv->get_connection_verify_no_ssl(session_id);
    if (!conn) {
        openswoole_error_log(OSW_LOG_NOTICE, OSW_ERROR_SESSION_NOT_EXIST, "session[%ld] is closed", session_id);
        return OSW_ERR;
    }

    ListenPort *port = serv->get_port_by_server_fd(server_fd);
    // other server port
    if (!(port->open_http_protocol && php_openswoole_server_isset_callback(serv, port, OSW_SERVER_CB_onRequest)) &&
        !(port->open_websocket_protocol && php_openswoole_server_isset_callback(serv, port, OSW_SERVER_CB_onMessage))) {
        return php_openswoole_server_onReceive(serv, req);
    }
    // websocket client
    if (conn->websocket_status == WebSocket::STATUS_ACTIVE) {
        return openswoole_websocket_onMessage(serv, req);
    }
#ifdef OSW_USE_HTTP2
    if (conn->http2_stream) {
        return openswoole_http2_server_onFrame(serv, conn, req);
    }
#endif

    HttpContext *ctx = openswoole_http_context_new(session_id);
    ctx->init(serv);

    zval *zdata = &ctx->request.zdata;
    php_openswoole_get_recv_data(serv, zdata, req);

    openswoole_trace_log(OSW_TRACE_SERVER,
                     "http request from %ld with %d bytes: <<EOF\n%.*s\nEOF",
                     session_id,
                     (int) Z_STRLEN_P(zdata),
                     (int) Z_STRLEN_P(zdata),
                     Z_STRVAL_P(zdata));

    zval args[2], *zrequest_object = &args[0], *zresponse_object = &args[1];
    args[0] = *ctx->request.zobject;
    args[1] = *ctx->response.zobject;

    openswoole_http_parser *parser = &ctx->parser;
    parser->data = ctx;
    openswoole_http_parser_init(parser, PHP_HTTP_REQUEST);

    size_t parsed_n = ctx->parse(Z_STRVAL_P(zdata), Z_STRLEN_P(zdata));
    if (ctx->parser.state == s_dead) {
#ifdef OSW_HTTP_BAD_REQUEST_PACKET
        ctx->send(ctx, OSW_STRL(OSW_HTTP_BAD_REQUEST_PACKET));
#endif
        ctx->close(ctx);
        openswoole_notice("request is illegal and it has been discarded, %ld bytes unprocessed",
                      Z_STRLEN_P(zdata) - parsed_n);
        goto _dtor_and_return;
    }

    do {
        zval *zserver = ctx->request.zserver;
        Connection *serv_sock = serv->get_connection(conn->server_fd);
        if (serv_sock) {
            add_assoc_long(zserver, "server_port", serv_sock->info.get_port());
        }
        add_assoc_long(zserver, "remote_port", conn->info.get_port());
        add_assoc_string(zserver, "remote_addr", (char *) conn->info.get_ip());
        add_assoc_long(zserver, "master_time", (int) conn->last_recv_time);
    } while (0);

    // begin to check and call registerd callback
    do {
        zend_fcall_info_cache *fci_cache = nullptr;

        if (conn->websocket_status == WebSocket::STATUS_CONNECTION) {
            fci_cache = php_openswoole_server_get_fci_cache(serv, server_fd, OSW_SERVER_CB_onHandShake);
            if (fci_cache == nullptr) {
                openswoole_websocket_onHandshake(serv, port, ctx);
                goto _dtor_and_return;
            } else {
                conn->websocket_status = WebSocket::STATUS_HANDSHAKE;
                ctx->upgrade = 1;
            }
        } else {
            fci_cache = php_openswoole_server_get_fci_cache(serv, server_fd, OSW_SERVER_CB_onRequest);
            if (fci_cache == nullptr) {
                openswoole_websocket_onRequest(ctx);
                goto _dtor_and_return;
            }
        }

        if (serv->max_request_execution_time > 0) {
            openswoole_timer_add((long) (serv->max_request_execution_time * 1000),
                             false,
                             php_openswoole_http_request_onTimeout,
                             (HttpContext *) ctx);
        }

        if (UNEXPECTED(!zend::function::call(fci_cache, 2, args, nullptr, serv->is_enable_coroutine()))) {
            php_openswoole_error(E_WARNING, "%s->onRequest handler error", ZSTR_VAL(openswoole_http_server_ce->name));
#ifdef OSW_HTTP_SERVICE_UNAVAILABLE_PACKET
            ctx->send(ctx, OSW_STRL(OSW_HTTP_SERVICE_UNAVAILABLE_PACKET));
#endif
            ctx->close(ctx);
        }
    } while (0);

_dtor_and_return:
    zval_ptr_dtor(zrequest_object);
    zval_ptr_dtor(zresponse_object);

    return OSW_OK;
}

void php_openswoole_http_server_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY_EX(
        openswoole_http_server, "OpenSwoole\\Http\\Server", "openswoole_http_server", nullptr, nullptr, openswoole_server);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_http_server);
    OSW_SET_CLASS_CLONEABLE(openswoole_http_server, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_http_server, osw_zend_class_unset_property_deny);
}

HttpContext *openswoole_http_context_new(SessionId fd) {
    HttpContext *ctx = new HttpContext();

    zval *zrequest_object = &ctx->request._zobject;
    ctx->request.zobject = zrequest_object;
    object_init_ex(zrequest_object, openswoole_http_request_ce);
    php_openswoole_http_request_set_context(zrequest_object, ctx);

    zval *zresponse_object = &ctx->response._zobject;
    ctx->response.zobject = zresponse_object;
    object_init_ex(zresponse_object, openswoole_http_response_ce);
    php_openswoole_http_response_set_context(zresponse_object, ctx);

    zend_update_property_long(openswoole_http_request_ce, OSW_Z8_OBJ_P(zrequest_object), ZEND_STRL("fd"), fd);
    zend_update_property_long(openswoole_http_response_ce, OSW_Z8_OBJ_P(zresponse_object), ZEND_STRL("fd"), fd);

    openswoole_http_init_and_read_property(
        openswoole_http_request_ce, zrequest_object, &ctx->request.zserver, ZEND_STRL("server"));
    openswoole_http_init_and_read_property(
        openswoole_http_request_ce, zrequest_object, &ctx->request.zheader, ZEND_STRL("header"));
    ctx->fd = fd;

    return ctx;
}

void HttpContext::init(Server *serv) {
    parse_cookie = serv->http_parse_cookie;
    parse_body = serv->http_parse_post;
    parse_files = serv->http_parse_files;
#ifdef OSW_HAVE_COMPRESSION
    enable_compression = serv->http_compression;
    compression_level = serv->http_compression_level;
    compression_min_length = serv->compression_min_length;
#endif
    upload_tmp_dir = serv->upload_tmp_dir;
    bind(serv);
}

void HttpContext::bind(Server *serv) {
    private_data = serv;
    send = http_context_send_data;
    sendfile = http_context_sendfile;
    close = http_context_disconnect;
}

void HttpContext::init(Socket *sock) {
    parse_cookie = 1;
    parse_body = 1;
    parse_files = 1;
#ifdef OSW_HAVE_COMPRESSION
    enable_compression = 1;
    compression_level = OSW_Z_BEST_SPEED;
#endif
#ifdef OSW_HAVE_ZLIB
    websocket_compression = 0;
#endif
    upload_tmp_dir = "/tmp";
    bind(sock);
}

void HttpContext::bind(Socket *sock) {
    private_data = sock;
    co_socket = 1;
    send = http_context_send_data;
    sendfile = http_context_sendfile;
    close = http_context_disconnect;
}

void HttpContext::copy(HttpContext *ctx) {
    parse_cookie = ctx->parse_cookie;
    parse_body = ctx->parse_body;
    parse_files = ctx->parse_files;
#ifdef OSW_HAVE_COMPRESSION
    enable_compression = ctx->enable_compression;
    compression_level = ctx->compression_level;
#endif
    co_socket = ctx->co_socket;
    private_data = ctx->private_data;
    upload_tmp_dir = ctx->upload_tmp_dir;
    send = ctx->send;
    sendfile = ctx->sendfile;
    close = ctx->close;
}

void HttpContext::free() {
    /* http context can only be free'd after request and response were free'd */
    if (request.zobject || response.zobject) {
        return;
    }
#ifdef OSW_USE_HTTP2
    if (stream) {
        return;
    }
#endif

    HttpRequest *req = &request;
    HttpResponse *res = &response;
    if (req->path) {
        efree(req->path);
    }
    if (Z_TYPE(req->zdata) == IS_STRING) {
        zend_string_release(Z_STR(req->zdata));
    }
    if (req->chunked_body) {
        delete req->chunked_body;
    }
#ifdef OSW_USE_HTTP2
    if (req->h2_data_buffer) {
        delete req->h2_data_buffer;
    }
#endif
    if (res->reason) {
        efree(res->reason);
    }
    delete this;
}

void php_openswoole_http_server_init_global_variant() {
    openswoole_http_buffer = new String(OSW_HTTP_RESPONSE_INIT_SIZE);
    openswoole_http_form_data_buffer = new String(OSW_HTTP_RESPONSE_INIT_SIZE);
    // for is_uploaded_file and move_uploaded_file
    if (!SG(rfc1867_uploaded_files)) {
        ALLOC_HASHTABLE(SG(rfc1867_uploaded_files));
        zend_hash_init(SG(rfc1867_uploaded_files), 8, nullptr, nullptr, 0);
    }
}

HttpContext *php_openswoole_http_request_get_and_check_context(zval *zobject) {
    HttpContext *ctx = php_openswoole_http_request_get_context(zobject);
    if (!ctx) {
        php_openswoole_fatal_error(E_WARNING, "http request is unavailable (maybe it has been ended)");
    }
    return ctx;
}

HttpContext *php_openswoole_http_response_get_and_check_context(zval *zobject) {
    HttpContext *ctx = php_openswoole_http_response_get_context(zobject);
    if (!ctx || (ctx->end_ || ctx->detached)) {
        php_openswoole_fatal_error(E_WARNING, "http response is unavailable (maybe it has been ended or detached)");
        // TODO: break out and throw exception
        return nullptr;
    }
    return ctx;
}

bool http_context_send_data(HttpContext *ctx, const char *data, size_t length) {
    Server *serv = (Server *) ctx->private_data;
    bool retval = serv->send(ctx->fd, (void *) data, length);
    if (!retval && openswoole_get_last_error() == OSW_ERROR_OUTPUT_SEND_YIELD) {
        zval yield_data, return_value;
        ZVAL_STRINGL(&yield_data, data, length);
        php_openswoole_server_send_yield(serv, ctx->fd, &yield_data, &return_value);
        return Z_BVAL_P(&return_value);
    }
    return retval;
}

static bool http_context_sendfile(HttpContext *ctx, const char *file, uint32_t l_file, off_t offset, size_t length) {
    Server *serv = (Server *) ctx->private_data;
    return serv->sendfile(ctx->fd, file, l_file, offset, length);
}

static bool http_context_disconnect(HttpContext *ctx) {
    Server *serv = (Server *) ctx->private_data;
    return serv->close(ctx->fd, 0);
}
