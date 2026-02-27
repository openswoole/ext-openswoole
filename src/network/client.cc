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
#include "openswoole_reactor.h"
#include "openswoole_timer.h"
#include "openswoole_protocol.h"
#include "openswoole_client.h"
#include "openswoole_proxy.h"
#include "openswoole_async.h"

#include <assert.h>

namespace openswoole {
namespace network {

static int Client_inet_addr(Client *cli, const char *host, int port);
static int Client_tcp_connect_sync(Client *cli, const char *host, int port, double _timeout, int udp_connect);
static int Client_tcp_connect_async(Client *cli, const char *host, int port, double timeout, int nonblock);
static int Client_udp_connect(Client *cli, const char *host, int port, double _timeout, int udp_connect);

static ssize_t Client_tcp_send_sync(Client *cli, const char *data, size_t length, int flags);
static ssize_t Client_tcp_send_async(Client *cli, const char *data, size_t length, int flags);
static ssize_t Client_udp_send(Client *cli, const char *data, size_t length, int flags);

static int Client_tcp_sendfile_sync(Client *cli, const char *filename, off_t offset, size_t length);
static int Client_tcp_sendfile_async(Client *cli, const char *filename, off_t offset, size_t length);

static ssize_t Client_tcp_recv_no_buffer(Client *cli, char *data, size_t len, int flags);
static ssize_t Client_udp_recv(Client *cli, char *data, size_t len, int waitall);

static int Client_onDgramRead(Reactor *reactor, Event *event);
static int Client_onStreamRead(Reactor *reactor, Event *event);
static int Client_onWrite(Reactor *reactor, Event *event);
static int Client_onError(Reactor *reactor, Event *event);
static void Client_onTimeout(Timer *timer, TimerNode *tnode);
static void Client_onResolveCompleted(AsyncEvent *event);
static int Client_onPackage(Protocol *proto, Socket *conn, const char *data, uint32_t length);

static osw_inline void execute_onConnect(Client *cli) {
    if (cli->timer) {
        openswoole_timer_del(cli->timer);
        cli->timer = nullptr;
    }
    cli->onConnect(cli);
}

void Client::init_reactor(Reactor *reactor) {
    reactor->set_handler(OSW_FD_STREAM_CLIENT | OSW_EVENT_READ, Client_onStreamRead);
    reactor->set_handler(OSW_FD_DGRAM_CLIENT | OSW_EVENT_READ, Client_onDgramRead);
    reactor->set_handler(OSW_FD_STREAM_CLIENT | OSW_EVENT_WRITE, Client_onWrite);
    reactor->set_handler(OSW_FD_STREAM_CLIENT | OSW_EVENT_ERROR, Client_onError);
}

Client::Client(SocketType _type, bool _async) : async(_async) {
    fd_type = Socket::is_stream(_type) ? OSW_FD_STREAM_CLIENT : OSW_FD_DGRAM_CLIENT;
    socket = openswoole::make_socket(_type, fd_type, (async ? OSW_SOCK_NONBLOCK : 0) | OSW_SOCK_CLOEXEC);
    if (socket == nullptr) {
        openswoole_sys_warning("socket() failed");
        return;
    }

    socket->object = this;
    input_buffer_size = OSW_CLIENT_BUFFER_SIZE;
    socket->chunk_size = OSW_SEND_BUFFER_SIZE;

    if (socket->is_stream()) {
        recv = Client_tcp_recv_no_buffer;
        if (async) {
            connect = Client_tcp_connect_async;
            send = Client_tcp_send_async;
            sendfile = Client_tcp_sendfile_async;
            socket->dontwait = OpenSwooleG.socket_dontwait;
        } else {
            connect = Client_tcp_connect_sync;
            send = Client_tcp_send_sync;
            sendfile = Client_tcp_sendfile_sync;
        }
    } else {
        connect = Client_udp_connect;
        recv = Client_udp_recv;
        send = Client_udp_send;
    }

    Socket::get_domain_and_type(_type, &_sock_domain, &_sock_type);

    protocol.package_length_type = 'N';
    protocol.package_length_size = 4;
    protocol.package_body_offset = 0;
    protocol.package_max_length = OSW_INPUT_BUFFER_SIZE;
    protocol.onPackage = Client_onPackage;
}

int Client::sleep() {
    int ret;
    if (socket->events & OSW_EVENT_WRITE) {
        ret = openswoole_event_set(socket, OSW_EVENT_WRITE);
    } else {
        ret = openswoole_event_del(socket);
    }
    if (ret == OSW_OK) {
        sleep_ = true;
    }
    return ret;
}

int Client::wakeup() {
    int ret;
    if (socket->events & OSW_EVENT_WRITE) {
        ret = openswoole_event_set(socket, OSW_EVENT_READ | OSW_EVENT_WRITE);
    } else {
        ret = openswoole_event_add(socket, OSW_EVENT_READ);
    }
    if (ret == OSW_OK) {
        sleep_ = false;
    }
    return ret;
}

int Client::shutdown(int __how) {
    if (!socket || closed) {
        return OSW_ERR;
    }
    if (__how == SHUT_RD) {
        if (shutdown_read || shutdow_rw || ::shutdown(socket->fd, SHUT_RD)) {
            return OSW_ERR;
        } else {
            shutdown_read = 1;
            return OSW_OK;
        }
    } else if (__how == SHUT_WR) {
        if (shutdown_write || shutdow_rw || ::shutdown(socket->fd, SHUT_WR) < 0) {
            return OSW_ERR;
        } else {
            shutdown_write = 1;
            return OSW_OK;
        }
    } else if (__how == SHUT_RDWR) {
        if (shutdow_rw || ::shutdown(socket->fd, SHUT_RDWR) < 0) {
            return OSW_ERR;
        } else {
            shutdown_read = 1;
            return OSW_OK;
        }
    } else {
        return OSW_ERR;
    }
}

int Client::socks5_handshake(const char *recv_data, size_t length) {
    Socks5Proxy *ctx = socks5_proxy;
    char *buf = ctx->buf;
    uchar version, status, result, method;

    if (ctx->state == OSW_SOCKS5_STATE_HANDSHAKE) {
        version = recv_data[0];
        method = recv_data[1];
        if (version != OSW_SOCKS5_VERSION_CODE) {
            openswoole_error_log(OSW_LOG_NOTICE, OSW_ERROR_SOCKS5_UNSUPPORT_VERSION, "SOCKS version is not supported");
            return OSW_ERR;
        }
        if (method != ctx->method) {
            openswoole_error_log(
                OSW_LOG_NOTICE, OSW_ERROR_SOCKS5_UNSUPPORT_METHOD, "SOCKS authentication method is not supported");
            return OSW_ERR;
        }
        // authenticate request
        if (method == OSW_SOCKS5_METHOD_AUTH) {
            buf[0] = 0x01;
            buf[1] = ctx->username.length();

            buf += 2;
            memcpy(buf, ctx->username.c_str(), ctx->username.length());
            buf += ctx->username.length();
            buf[0] = ctx->password.length();
            memcpy(buf + 1, ctx->password.c_str(), ctx->password.length());

            ctx->state = OSW_SOCKS5_STATE_AUTH;

            return send(this, ctx->buf, ctx->username.length() + ctx->password.length() + 3, 0);
        }
        // send connect request
        else {
        _send_connect_request:
            buf[0] = OSW_SOCKS5_VERSION_CODE;
            buf[1] = 0x01;
            buf[2] = 0x00;

            ctx->state = OSW_SOCKS5_STATE_CONNECT;

            if (ctx->dns_tunnel) {
                buf[3] = 0x03;
                buf[4] = ctx->target_host.length();
                buf += 5;
                memcpy(buf, ctx->target_host.c_str(), ctx->target_host.length());
                buf += ctx->target_host.length();
                *(uint16_t *) buf = htons(ctx->target_port);
                return send(this, ctx->buf, ctx->target_host.length() + 7, 0);
            } else {
                buf[3] = 0x01;
                buf += 4;
                *(uint32_t *) buf = htons(ctx->target_host.length());
                buf += 4;
                *(uint16_t *) buf = htons(ctx->target_port);
                return send(this, ctx->buf, ctx->target_host.length() + 7, 0);
            }
        }
    } else if (ctx->state == OSW_SOCKS5_STATE_AUTH) {
        version = recv_data[0];
        status = recv_data[1];
        if (version != 0x01) {
            openswoole_error_log(OSW_LOG_NOTICE, OSW_ERROR_SOCKS5_UNSUPPORT_VERSION, "SOCKS version is not supported");
            return OSW_ERR;
        }
        if (status != 0) {
            openswoole_error_log(
                OSW_LOG_NOTICE, OSW_ERROR_SOCKS5_AUTH_FAILED, "SOCKS username/password authentication failed");
            return OSW_ERR;
        }
        goto _send_connect_request;
    } else if (ctx->state == OSW_SOCKS5_STATE_CONNECT) {
        version = recv_data[0];
        if (version != OSW_SOCKS5_VERSION_CODE) {
            openswoole_error_log(OSW_LOG_NOTICE, OSW_ERROR_SOCKS5_UNSUPPORT_VERSION, "SOCKS version is not supported");
            return OSW_ERR;
        }
        result = recv_data[1];
#if 0
        uchar reg = recv_data[2];
        uchar type = recv_data[3];
        uint32_t ip = *(uint32_t *) (recv_data + 4);
        uint16_t port = *(uint16_t *) (recv_data + 8);
#endif
        if (result == 0) {
            ctx->state = OSW_SOCKS5_STATE_READY;
        } else {
            openswoole_error_log(OSW_LOG_NOTICE,
                             OSW_ERROR_SOCKS5_SERVER_ERROR,
                             "Socks5 server error, reason :%s",
                             Socks5Proxy::strerror(result));
        }
        return result;
    }
    return OSW_OK;
}

#ifdef OSW_USE_OPENSSL
#ifdef OSW_SUPPORT_DTLS
void Client::enable_dtls() {
    ssl_context->protocols = OSW_SSL_DTLS;
    socket->dtls = 1;
    socket->chunk_size = OSW_SSL_BUFFER_SIZE;
    send = Client_tcp_send_sync;
    recv = Client_tcp_recv_no_buffer;
}
#endif

int Client::enable_ssl_encrypt() {
    if (ssl_context) {
        return OSW_ERR;
    }
    ssl_context.reset(new openswoole::SSLContext());
    open_ssl = true;
#ifdef OSW_SUPPORT_DTLS
    if (socket->is_dgram()) {
        enable_dtls();
    }
#else
    {
        openswoole_warning("DTLS support require openssl-1.1 or later");
        return OSW_ERR;
    }
#endif
    return OSW_OK;
}

int Client::ssl_handshake() {
    if (socket->ssl_state == OSW_SSL_STATE_READY) {
        return OSW_ERR;
    }
    if (!ssl_context->ready()) {
        ssl_context->http_v2 = http2;
        if (!ssl_context->create()) {
            return OSW_ERR;
        }
    }
    if (!socket->ssl) {
        socket->ssl_send_ = 1;
        if (socket->ssl_create(ssl_context.get(), OSW_SSL_CLIENT) < 0) {
            return OSW_ERR;
        }
#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
        if (!ssl_context->tls_host_name.empty()) {
            SSL_set_tlsext_host_name(socket->ssl, ssl_context->tls_host_name.c_str());
        }
#endif
    }
    if (socket->ssl_connect() < 0) {
        return OSW_ERR;
    }
    if (socket->ssl_state == OSW_SSL_STATE_READY && ssl_context->verify_peer) {
        if (ssl_verify(ssl_context->allow_self_signed) < 0) {
            return OSW_ERR;
        }
    }
    return OSW_OK;
}

int Client::ssl_verify(int allow_self_signed) {
    if (!socket->ssl_verify(allow_self_signed)) {
        return OSW_ERR;
    }
#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
    if (!ssl_context->tls_host_name.empty() && !socket->ssl_check_host(ssl_context->tls_host_name.c_str())) {
        return OSW_ERR;
    }
#endif
    return OSW_OK;
}

#endif

static int Client_inet_addr(Client *cli, const char *host, int port) {
    // enable socks5 proxy
    if (cli->socks5_proxy) {
        cli->socks5_proxy->target_host = host;
        cli->socks5_proxy->target_port = port;

        host = cli->socks5_proxy->host.c_str();
        port = cli->socks5_proxy->port;
    }

    // enable http proxy
    if (cli->http_proxy) {
        cli->http_proxy->target_host = host;
        cli->http_proxy->target_port = port;

        host = cli->http_proxy->proxy_host.c_str();
        port = cli->http_proxy->proxy_port;
    }

    cli->server_host = host;
    cli->server_port = port;

    void *addr = nullptr;
    if (cli->socket->is_inet4()) {
        cli->server_addr.addr.inet_v4.sin_family = AF_INET;
        cli->server_addr.addr.inet_v4.sin_port = htons(port);
        cli->server_addr.len = sizeof(cli->server_addr.addr.inet_v4);
        addr = &cli->server_addr.addr.inet_v4.sin_addr.s_addr;

        if (inet_pton(AF_INET, host, addr)) {
            return OSW_OK;
        }
    } else if (cli->socket->is_inet6()) {
        cli->server_addr.addr.inet_v6.sin6_family = AF_INET6;
        cli->server_addr.addr.inet_v6.sin6_port = htons(port);
        cli->server_addr.len = sizeof(cli->server_addr.addr.inet_v6);
        addr = cli->server_addr.addr.inet_v6.sin6_addr.s6_addr;

        if (inet_pton(AF_INET6, host, addr)) {
            return OSW_OK;
        }
    } else if (cli->socket->is_local()) {
        cli->server_addr.addr.un.sun_family = AF_UNIX;
        openswoole_strlcpy(cli->server_addr.addr.un.sun_path, host, sizeof(cli->server_addr.addr.un.sun_path));
        cli->server_addr.addr.un.sun_path[sizeof(cli->server_addr.addr.un.sun_path) - 1] = 0;
        cli->server_addr.len = sizeof(cli->server_addr.addr.un.sun_path);
        return OSW_OK;
    } else {
        return OSW_ERR;
    }
    if (!cli->async) {
        if (openswoole::network::gethostbyname(cli->_sock_domain, host, (char *) addr) < 0) {
            openswoole_set_last_error(OSW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
            return OSW_ERR;
        }
    } else {
        cli->wait_dns = 1;
    }
    return OSW_OK;
}

void Client::destroy() {
    if (destroyed) {
        return;
    }
    destroyed = true;
    openswoole_event_defer(
        [](void *data) {
            Client *object = (Client *) data;
            delete object;
        },
        this);
}

Client::~Client() {
    if (!socket) {
        return;
    }
    assert(socket->fd != 0);
    // remove from reactor
    if (!closed) {
        close();
    }
    // clear buffer
    if (buffer) {
        delete buffer;
        buffer = nullptr;
    }
    if (server_str) {
        ::osw_free((void *) server_str);
    }
    if (socks5_proxy) {
        delete socks5_proxy;
    }
    if (http_proxy) {
        delete http_proxy;
    }
    if (async) {
        socket->free();
    } else {
        delete socket;
    }
}

int Client::close() {
    if (socket == nullptr || closed) {
        return OSW_ERR;
    }
    closed = 1;

    int fd = socket->fd;
    assert(fd != 0);

#ifdef OSW_USE_OPENSSL
    if (open_ssl && ssl_context) {
        if (socket->ssl) {
            socket->ssl_close();
        }
    }
#endif

    if (socket->socket_type == OSW_SOCK_UNIX_DGRAM) {
        unlink(socket->info.addr.un.sun_path);
    }
    if (async) {
        // remove from reactor
        if (!socket->removed) {
            openswoole_event_del(socket);
        }
        if (timer) {
            openswoole_timer_del(timer);
            timer = nullptr;
        }
        // onClose callback
        if (active) {
            active = 0;
            onClose(this);
        }
    } else {
        active = 0;
    }

    /**
     * fd marked -1, prevent double close
     */
    socket->fd = -1;

    return ::close(fd);
}

static int Client_tcp_connect_sync(Client *cli, const char *host, int port, double timeout, int nonblock) {
    int ret;

    cli->timeout = timeout;

    if (Client_inet_addr(cli, host, port) < 0) {
        return OSW_ERR;
    }

    if (nonblock) {
        cli->socket->set_nonblock();
    } else {
        if (cli->timeout > 0) {
            cli->socket->set_timeout(timeout);
        }
#ifndef HAVE_KQUEUE
        cli->socket->set_block();
#endif
    }
    while (1) {
#ifdef HAVE_KQUEUE
        if (nonblock == 2) {
            // special case on MacOS
            ret = cli->socket->connect(cli->server_addr);
        } else {
            cli->socket->set_nonblock();
            ret = cli->socket->connect(cli->server_addr);
            if (ret < 0) {
                if (errno != EINPROGRESS) {
                    return OSW_ERR;
                }
                if (cli->socket->wait_event(timeout > 0 ? (int) (timeout * 1000) : timeout, OSW_EVENT_WRITE) < 0) {
                    openswoole_set_last_error(ETIMEDOUT);
                    return OSW_ERR;
                }
                int err;
                socklen_t len = sizeof(len);
                ret = cli->socket->get_option(SOL_SOCKET, SO_ERROR, &err, &len);
                if (ret < 0) {
                    openswoole_set_last_error(errno);
                    return OSW_ERR;
                }
                if (err != 0) {
                    openswoole_set_last_error(err);
                    return OSW_ERR;
                }
                cli->socket->set_block();
                ret = 0;
            }
        }
#else
        ret = cli->socket->connect(cli->server_addr);
#endif
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EINPROGRESS) {
                if (nonblock) {
                    cli->async_connect = true;
                } else {
                    errno = ETIMEDOUT;
                }
            }
            openswoole_set_last_error(errno);
        }
        break;
    }

    if (ret >= 0) {
        cli->active = 1;

        // socks5 proxy
        if (cli->socks5_proxy) {
            char buf[1024];
            Socks5Proxy::pack(buf, cli->socks5_proxy->username.empty() ? 0x00 : 0x02);
            if (cli->send(cli, buf, 3, 0) < 0) {
                return OSW_ERR;
            }
            cli->socks5_proxy->state = OSW_SOCKS5_STATE_HANDSHAKE;
            while (1) {
                ssize_t n = cli->recv(cli, buf, sizeof(buf), 0);
                if (n > 0) {
                    if (cli->socks5_handshake(buf, n) < 0) {
                        return OSW_ERR;
                    }
                    if (cli->socks5_proxy->state == OSW_SOCKS5_STATE_READY) {
                        break;
                    } else {
                        continue;
                    }
                }
                return OSW_ERR;
            }
        }

#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl && cli->ssl_handshake() < 0) {
            return OSW_ERR;
        }
#endif
    }

    return ret;
}

static int Client_tcp_connect_async(Client *cli, const char *host, int port, double timeout, int nonblock) {
    int ret;

    cli->timeout = timeout;

    if (!cli->buffer) {
        cli->buffer = new String(cli->input_buffer_size);
    }

    if (!(cli->onConnect && cli->onError && cli->onClose && cli->onReceive)) {
        openswoole_warning("onConnect/onError/onReceive/onClose callback have not set");
        return OSW_ERR;
    }

    if (cli->onBufferFull && cli->buffer_high_watermark == 0) {
        cli->buffer_high_watermark = cli->socket->buffer_size * 0.8;
    }

    if (Client_inet_addr(cli, host, port) < 0) {
        return OSW_ERR;
    }

    if (cli->wait_dns) {
        AsyncEvent ev{};

        size_t len = strlen(cli->server_host);
        if (len < OSW_IP_MAX_LENGTH) {
            ev.nbytes = OSW_IP_MAX_LENGTH;
        } else {
            ev.nbytes = len + 1;
        }

        ev.buf = osw_malloc(ev.nbytes);
        if (!ev.buf) {
            openswoole_warning("malloc failed");
            return OSW_ERR;
        }

        memcpy(ev.buf, cli->server_host, len);
        ((char *) ev.buf)[len] = 0;
        ev.flags = cli->_sock_domain;
        ev.object = cli;
        ev.fd = cli->socket->fd;
        ev.handler = async::handler_gethostbyname;
        ev.callback = Client_onResolveCompleted;

        if (openswoole::async::dispatch(&ev) == nullptr) {
            osw_free(ev.buf);
            return OSW_ERR;
        } else {
            return OSW_OK;
        }
    }

    while (1) {
        ret = cli->socket->connect(cli->server_addr);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            openswoole_set_last_error(errno);
        }
        break;
    }

    if ((ret < 0 && errno == EINPROGRESS) || ret == 0) {
        if (openswoole_event_add(cli->socket, OSW_EVENT_WRITE) < 0) {
            return OSW_ERR;
        }
        if (timeout > 0) {
            cli->timer = openswoole_timer_add((long) (timeout * 1000), false, Client_onTimeout, cli);
        }
        return OSW_OK;
    } else {
        cli->active = 0;
        cli->socket->removed = 1;
        cli->close();
        if (cli->onError) {
            cli->onError(cli);
        }
    }

    return ret;
}

static ssize_t Client_tcp_send_async(Client *cli, const char *data, size_t length, int flags) {
    ssize_t n = length;
    if (openswoole_event_write(cli->socket, data, length) < 0) {
        if (openswoole_get_last_error() == OSW_ERROR_OUTPUT_BUFFER_OVERFLOW) {
            n = -1;
            cli->high_watermark = 1;
        } else {
            return OSW_ERR;
        }
    }
    if (cli->onBufferFull && cli->socket->out_buffer && cli->high_watermark == 0 &&
        cli->socket->out_buffer->length() >= cli->buffer_high_watermark) {
        cli->high_watermark = 1;
        cli->onBufferFull(cli);
    }
    return n;
}

static ssize_t Client_tcp_send_sync(Client *cli, const char *data, size_t length, int flags) {
    size_t written = 0;
    ssize_t n;

    assert(length > 0);
    assert(data != nullptr);

    while (written < length) {
        n = cli->socket->send(data, length - written, flags);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                cli->socket->wait_event(1000, OSW_EVENT_WRITE);
                continue;
            } else {
                openswoole_set_last_error(errno);
                return OSW_ERR;
            }
        }
        written += n;
        data += n;
    }

    return written;
}

static int Client_tcp_sendfile_sync(Client *cli, const char *filename, off_t offset, size_t length) {
    if (cli->socket->sendfile_blocking(filename, offset, length, cli->timeout) < 0) {
        openswoole_set_last_error(errno);
        return OSW_ERR;
    }
    return OSW_OK;
}

static int Client_tcp_sendfile_async(Client *cli, const char *filename, off_t offset, size_t length) {
    if (cli->socket->sendfile(filename, offset, length) < 0) {
        openswoole_set_last_error(errno);
        return OSW_ERR;
    }
    if (openswoole_event_add_or_update(cli->socket, OSW_EVENT_WRITE) == OSW_ERR) {
        return OSW_ERR;
    }
    return OSW_OK;
}

/**
 * Only for synchronous client
 */
static ssize_t Client_tcp_recv_no_buffer(Client *cli, char *data, size_t len, int flag) {
    ssize_t ret;

    while (1) {
#ifdef HAVE_KQUEUE
        int timeout_ms = (int) (cli->timeout * 1000);
#ifdef OSW_USE_OPENSSL
        if (cli->socket->ssl) {
            timeout_ms = 0;
        }
#endif
        if (timeout_ms > 0 && cli->socket->wait_event(timeout_ms, OSW_EVENT_READ) < 0) {
            return -1;
        }
#endif
        ret = cli->socket->recv(data, len, flag);
        if (ret >= 0) {
            break;
        }
        if (errno == EINTR) {
            if (cli->interrupt_time <= 0) {
                cli->interrupt_time = microtime();
                continue;
            } else if (microtime() > cli->interrupt_time + cli->timeout) {
                break;
            } else {
                continue;
            }
        }
#ifdef OSW_USE_OPENSSL
        if (cli->socket->catch_error(errno) == OSW_WAIT && cli->socket->ssl) {
            int timeout_ms = (int) (cli->timeout * 1000);
            if (cli->socket->ssl_want_read && cli->socket->wait_event(timeout_ms, OSW_EVENT_READ) == OSW_OK) {
                continue;
            } else if (cli->socket->ssl_want_write && cli->socket->wait_event(timeout_ms, OSW_EVENT_WRITE) == OSW_OK) {
                continue;
            }
        }
#endif
        break;
    }

    return ret;
}

static int Client_udp_connect(Client *cli, const char *host, int port, double timeout, int udp_connect) {
    if (Client_inet_addr(cli, host, port) < 0) {
        return OSW_ERR;
    }

    if (cli->async && !cli->onReceive) {
        openswoole_warning("onReceive callback have not set");
        return OSW_ERR;
    }

    cli->active = 1;
    cli->timeout = timeout;
    int bufsize = Socket::default_buffer_size;

    if (timeout > 0) {
        cli->socket->set_timeout(timeout);
    }

    if (cli->socket->socket_type == OSW_SOCK_UNIX_DGRAM) {
        struct sockaddr_un *client_addr = &cli->socket->info.addr.un;
        sprintf(client_addr->sun_path, "/tmp/openswoole-client.%d.%d.sock", getpid(), cli->socket->fd);
        client_addr->sun_family = AF_UNIX;
        unlink(client_addr->sun_path);

        if (bind(cli->socket->fd, (struct sockaddr *) client_addr, sizeof(cli->socket->info.addr.un)) < 0) {
            openswoole_sys_warning("bind(%s) failed", client_addr->sun_path);
            return OSW_ERR;
        }
    }

#ifdef OSW_USE_OPENSSL
    if (cli->open_ssl)
#ifdef OSW_SUPPORT_DTLS
    {
        udp_connect = 1;
        cli->enable_dtls();
    }
#else
    {
        openswoole_warning("DTLS support require openssl-1.1 or later");
        return OSW_ERR;
    }
#endif
#endif

    if (udp_connect != 1) {
        goto _connect_ok;
    }

    if (cli->socket->connect(cli->server_addr) == 0) {
        cli->socket->clean();
    _connect_ok:

        cli->socket->set_option(SOL_SOCKET, SO_SNDBUF, bufsize);
        cli->socket->set_option(SOL_SOCKET, SO_RCVBUF, bufsize);

        if (cli->async && cli->onConnect) {
            if (openswoole_event_add(cli->socket, OSW_EVENT_READ) < 0) {
                return OSW_ERR;
            }
            execute_onConnect(cli);
        }
#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl && cli->ssl_handshake() < 0) {
            return OSW_ERR;
        }
#endif
        return OSW_OK;
    } else {
        cli->active = 0;
        cli->socket->removed = 1;
        cli->close();
        if (cli->async && cli->onError) {
            cli->onError(cli);
        }
        return OSW_ERR;
    }
}

static ssize_t Client_udp_send(Client *cli, const char *data, size_t len, int flags) {
    ssize_t n = sendto(cli->socket->fd, data, len, 0, (struct sockaddr *) &cli->server_addr.addr, cli->server_addr.len);
    if (n < 0 || n < (ssize_t) len) {
        return OSW_ERR;
    } else {
        return n;
    }
}

static ssize_t Client_udp_recv(Client *cli, char *data, size_t length, int flags) {
#ifdef HAVE_KQUEUE
    if (!cli->async) {
        int timeout_ms = (int) (cli->timeout * 1000);
        if (cli->socket->wait_event(timeout_ms, OSW_EVENT_READ) < 0) {
            return -1;
        }
    }
#endif
    ssize_t ret = cli->socket->recvfrom(data, length, flags, &cli->remote_addr);
    if (ret < 0) {
        if (errno == EINTR) {
            ret = cli->socket->recvfrom(data, length, flags, &cli->remote_addr);
        } else {
            return OSW_ERR;
        }
    }
    return ret;
}

#ifdef OSW_USE_OPENSSL
static int Client_https_proxy_handshake(Client *cli) {
    char *buf = cli->buffer->str;
    size_t len = cli->buffer->length;
    int state = 0;
    char *p = buf;
    char *pe = buf + len;
    for (; p < pe; p++) {
        if (state == 0) {
            if (OSW_STRCASECT(p, pe - p, "HTTP/1.1") || OSW_STRCASECT(p, pe - p, "HTTP/1.0")) {
                state = 1;
                p += sizeof("HTTP/1.x") - 1;
            } else {
                break;
            }
        } else if (state == 1) {
            if (isspace(*p)) {
                continue;
            } else {
                if (OSW_STRCASECT(p, pe - p, "200")) {
                    state = 2;
                    p += sizeof("200") - 1;
                } else {
                    break;
                }
            }
        } else if (state == 2) {
            if (isspace(*p)) {
                continue;
            } else {
                if (OSW_STRCASECT(p, pe - p, "Connection established")) {
                    return OSW_OK;
                } else {
                    break;
                }
            }
        }
    }
    return OSW_ERR;
}
#endif

static int Client_onPackage(Protocol *proto, Socket *conn, const char *data, uint32_t length) {
    Client *cli = (Client *) conn->object;
    cli->onReceive(cli, data, length);
    return conn->close_wait ? OSW_ERR : OSW_OK;
}

static int Client_onStreamRead(Reactor *reactor, Event *event) {
    ssize_t n = -1;
    Client *cli = (Client *) event->socket->object;
    char *buf = cli->buffer->str + cli->buffer->length;
    ssize_t buf_size = cli->buffer->size - cli->buffer->length;

    if (cli->http_proxy && cli->http_proxy->state != OSW_HTTP_PROXY_STATE_READY) {
#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl) {
            n = event->socket->recv(buf, buf_size, 0);
            if (n <= 0) {
                goto __close;
            }
            cli->buffer->length += n;
            if (cli->buffer->length < sizeof(OSW_HTTPS_PROXY_HANDSHAKE_RESPONSE) - 1) {
                return OSW_OK;
            }
            if (Client_https_proxy_handshake(cli) < 0) {
                openswoole_error_log(
                    OSW_LOG_NOTICE, OSW_ERROR_HTTP_PROXY_HANDSHAKE_ERROR, "failed to handshake with http proxy");
                goto _connect_fail;
            } else {
                cli->http_proxy->state = OSW_HTTP_PROXY_STATE_READY;
                cli->buffer->clear();
            }
            if (cli->ssl_handshake() < 0) {
                goto _connect_fail;
            } else {
                if (cli->socket->ssl_state == OSW_SSL_STATE_READY) {
                    execute_onConnect(cli);
                } else if (cli->socket->ssl_state == OSW_SSL_STATE_WAIT_STREAM && cli->socket->ssl_want_write) {
                    openswoole_event_set(event->socket, OSW_EVENT_WRITE);
                }
            }
            return OSW_OK;
        }
#endif
    }
    if (cli->socks5_proxy && cli->socks5_proxy->state != OSW_SOCKS5_STATE_READY) {
        n = event->socket->recv(buf, buf_size, 0);
        if (n <= 0) {
            goto __close;
        }
        if (cli->socks5_handshake(buf, buf_size) < 0) {
            goto __close;
        }
        if (cli->socks5_proxy->state != OSW_SOCKS5_STATE_READY) {
            return OSW_OK;
        }
#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl) {
            if (cli->ssl_handshake() < 0) {
            _connect_fail:
                cli->active = 0;
                cli->close();
                if (cli->onError) {
                    cli->onError(cli);
                }
            } else {
                cli->socket->ssl_state = OSW_SSL_STATE_WAIT_STREAM;
                return openswoole_event_set(event->socket, OSW_EVENT_WRITE);
            }
        } else
#endif
        {
            execute_onConnect(cli);
        }
        return OSW_OK;
    }

#ifdef OSW_USE_OPENSSL
    if (cli->open_ssl && cli->socket->ssl_state == OSW_SSL_STATE_WAIT_STREAM) {
        if (cli->ssl_handshake() < 0) {
            goto _connect_fail;
        }
        if (cli->socket->ssl_state != OSW_SSL_STATE_READY) {
            return OSW_OK;
        } else {
            execute_onConnect(cli);
            return OSW_OK;
        }
    }
#endif

    if (cli->open_eof_check || cli->open_length_check) {
        Socket *conn = cli->socket;
        Protocol *protocol = &cli->protocol;

        if (cli->open_eof_check) {
            n = protocol->recv_with_eof_protocol(conn, cli->buffer);
        } else {
            n = protocol->recv_with_length_protocol(conn, cli->buffer);
        }

        if (n < 0) {
            if (!cli->closed) {
                cli->close();
            }
            return OSW_OK;
        } else {
            if (conn->removed == 0 && cli->remove_delay) {
                cli->sleep();
                cli->remove_delay = 0;
            }
            return OSW_OK;
        }
    }

#ifdef OSW_CLIENT_RECV_AGAIN
_recv_again:
#endif
    n = event->socket->recv(buf, buf_size, 0);
    if (n < 0) {
        switch (event->socket->catch_error(errno)) {
        case OSW_ERROR:
            openswoole_sys_warning("Read from socket[%d] failed", event->fd);
            return OSW_OK;
        case OSW_CLOSE:
            goto __close;
        case OSW_WAIT:
            return OSW_OK;
        default:
            return OSW_OK;
        }
    } else if (n == 0) {
    __close:
        return cli->close();
    } else {
        cli->onReceive(cli, buf, n);
#ifdef OSW_CLIENT_RECV_AGAIN
        if (n == buf_size) {
            goto _recv_again;
        }
#endif
        return OSW_OK;
    }
    return OSW_OK;
}

static int Client_onDgramRead(Reactor *reactor, Event *event) {
    Client *cli = (Client *) event->socket->object;
    char buffer[OSW_BUFFER_SIZE_UDP];

    int n = Client_udp_recv(cli, buffer, sizeof(buffer), 0);
    if (n < 0) {
        return OSW_ERR;
    } else {
        cli->onReceive(cli, buffer, n);
    }
    return OSW_OK;
}

static int Client_onError(Reactor *reactor, Event *event) {
    Client *cli = (Client *) event->socket->object;
    if (cli->active) {
        return cli->close();
    } else {
        Client_onWrite(reactor, event);
    }
    return OSW_OK;
}

static void Client_onTimeout(Timer *timer, TimerNode *tnode) {
    Client *cli = (Client *) tnode->data;
    openswoole_set_last_error(ETIMEDOUT);

#ifdef OSW_USE_OPENSSL
    if (cli->open_ssl && cli->socket->ssl_state != OSW_SSL_STATE_READY) {
        cli->active = 0;
    }
#endif
    if (cli->socks5_proxy && cli->socks5_proxy->state != OSW_SOCKS5_STATE_READY) {
        cli->active = 0;
    } else if (cli->http_proxy && cli->http_proxy->state != OSW_HTTP_PROXY_STATE_READY) {
        cli->active = 0;
    }

    cli->close();
    if (cli->onError) {
        cli->onError(cli);
    }
}

static void Client_onResolveCompleted(AsyncEvent *event) {
    if (event->canceled) {
        osw_free(event->buf);
        return;
    }

    Client *cli = (Client *) event->object;
    cli->wait_dns = 0;

    if (event->error == 0) {
        Client_tcp_connect_async(cli, (char *) event->buf, cli->server_port, cli->timeout, 1);
    } else {
        openswoole_set_last_error(OSW_ERROR_DNSLOOKUP_RESOLVE_FAILED);
        cli->socket->removed = 1;
        cli->close();
        if (cli->onError) {
            cli->onError(cli);
        }
    }
    osw_free(event->buf);
}

static int Client_onWrite(Reactor *reactor, Event *event) {
    Client *cli = (Client *) event->socket->object;
    Socket *_socket = cli->socket;
    int ret;
    int err;

    if (cli->active) {
#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl && _socket->ssl_state == OSW_SSL_STATE_WAIT_STREAM) {
            if (cli->ssl_handshake() < 0) {
                goto _connect_fail;
            } else if (_socket->ssl_state == OSW_SSL_STATE_READY) {
                goto _connect_success;
            } else {
                if (_socket->ssl_want_read) {
                    openswoole_event_set(event->socket, OSW_EVENT_READ);
                }
                return OSW_OK;
            }
        }
#endif
        if (Reactor::_writable_callback(reactor, event) < 0) {
            return OSW_ERR;
        }
        if (cli->onBufferEmpty && cli->high_watermark && _socket->out_buffer->length() <= cli->buffer_low_watermark) {
            cli->high_watermark = 0;
            cli->onBufferEmpty(cli);
        }
        return OSW_OK;
    }

    ret = _socket->get_option(SOL_SOCKET, SO_ERROR, &err);
    openswoole_set_last_error(err);
    if (ret < 0) {
        openswoole_sys_warning("getsockopt(%d) failed", event->fd);
        return OSW_ERR;
    }

    // success
    if (openswoole_get_last_error() == 0) {
        // listen read event
        openswoole_event_set(event->socket, OSW_EVENT_READ);
        // connected
        cli->active = 1;
        // socks5 proxy
        if (cli->socks5_proxy && cli->socks5_proxy->state == OSW_SOCKS5_STATE_WAIT) {
            char buf[3];
            Socks5Proxy::pack(buf, cli->socks5_proxy->username.empty() ? 0x00 : 0x02);
            cli->socks5_proxy->state = OSW_SOCKS5_STATE_HANDSHAKE;
            return cli->send(cli, buf, sizeof(buf), 0);
        }
        // http proxy
        if (cli->http_proxy && cli->http_proxy->state == OSW_HTTP_PROXY_STATE_WAIT) {
#ifdef OSW_USE_OPENSSL
            if (cli->open_ssl) {
                cli->http_proxy->state = OSW_HTTP_PROXY_STATE_HANDSHAKE;
                int n = osw_snprintf(cli->http_proxy->buf,
                                    sizeof(cli->http_proxy->buf),
                                    "CONNECT %s:%d HTTP/1.1\r\n\r\n",
                                    cli->http_proxy->target_host.c_str(),
                                    cli->http_proxy->target_port);
                return cli->send(cli, cli->http_proxy->buf, n, 0);
            }
#endif
        }
#ifdef OSW_USE_OPENSSL
        if (cli->open_ssl) {
            if (cli->ssl_handshake() < 0) {
                goto _connect_fail;
            } else {
                _socket->ssl_state = OSW_SSL_STATE_WAIT_STREAM;
            }
            return OSW_OK;
        }
    _connect_success:
#endif
        execute_onConnect(cli);
    } else {
#ifdef OSW_USE_OPENSSL
    _connect_fail:
#endif
        cli->active = 0;
        cli->close();
        cli->onError(cli);
    }

    return OSW_OK;
}

}  // namespace network
}  // namespace openswoole
