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

#include "openswoole_server.h"

namespace openswoole {

bool BaseFactory::start() {
    OpenSwooleWG.run_always = true;
    return true;
}

bool BaseFactory::shutdown() {
    return true;
}

bool BaseFactory::dispatch(SendData *task) {
    PacketPtr pkg{};
    Connection *conn = nullptr;

    if (Server::is_stream_event(task->info.type)) {
        conn = server_->get_connection(task->info.fd);
        if (conn == nullptr || conn->active == 0) {
            openswoole_warning("dispatch[type=%d] failed, socket#%ld is not active", task->info.type, task->info.fd);
            return false;
        }
        // server active close, discard data.
        if (conn->closed) {
            openswoole_warning("dispatch[type=%d] failed, socket#%ld is closed by server", task->info.type, task->info.fd);
            return false;
        }
        // converted fd to session_id
        task->info.fd = conn->session_id;
        task->info.server_fd = conn->server_fd;
    }
    // with data
    if (task->info.len > 0) {
        memcpy(&pkg.info, &task->info, sizeof(pkg.info));
        pkg.info.flags = OSW_EVENT_DATA_PTR;
        pkg.data.length = task->info.len;
        pkg.data.str = (char *) task->data;

        if (conn && conn->socket->recv_buffer && task->data == conn->socket->recv_buffer->str &&
            conn->socket->recv_buffer->offset > 0 &&
            conn->socket->recv_buffer->length == (size_t) conn->socket->recv_buffer->offset) {
            pkg.info.flags |= OSW_EVENT_DATA_POP_PTR;
        }

        return server_->accept_task((EventData *) &pkg) == OSW_OK;
    }
    // no data
    else {
        return server_->accept_task((EventData *) &task->info) == OSW_OK;
    }
}

/**
 * only stream fd
 */
bool BaseFactory::notify(DataHead *info) {
    Connection *conn = server_->get_connection(info->fd);
    if (conn == nullptr || conn->active == 0) {
        openswoole_warning("dispatch[type=%d] failed, socket#%ld is not active", info->type, info->fd);
        return false;
    }
    // server active close, discard data.
    if (conn->closed) {
        openswoole_warning("dispatch[type=%d] failed, session#%ld is closed by server", info->type, conn->session_id);
        return false;
    }
    // converted fd to session_id
    info->fd = conn->session_id;
    info->server_fd = conn->server_fd;
    info->flags = OSW_EVENT_DATA_NORMAL;

    return server_->accept_task((EventData *) info) == OSW_OK;
}

bool BaseFactory::end(SessionId session_id, int flags) {
    SendData _send{};
    _send.info.fd = session_id;
    _send.info.len = 0;
    _send.info.type = OSW_SERVER_EVENT_CLOSE;
    _send.info.reactor_id = OpenSwooleG.process_id;

    Session *session = server_->get_session(session_id);
    if (!session->fd) {
        openswoole_error_log(OSW_LOG_NOTICE,
                         OSW_ERROR_SESSION_NOT_EXIST,
                         "failed to close connection, session#%ld does not exist",
                         session_id);
        return false;
    }

    if (session->reactor_id != OpenSwooleG.process_id) {
        Worker *worker = server_->get_worker(session->reactor_id);
        if (worker->pipe_master->send_async((const char *) &_send.info, sizeof(_send.info)) < 0) {
            openswoole_sys_warning("failed to send %lu bytes to pipe_master", sizeof(_send.info));
            return false;
        }
        return true;
    }

    Connection *conn = server_->get_connection_verify_no_ssl(session_id);
    if (conn == nullptr) {
        openswoole_set_last_error(OSW_ERROR_SESSION_NOT_EXIST);
        return false;
    }
    // Reset send buffer, Immediately close the connection.
    if (flags & Server::CLOSE_RESET) {
        conn->close_reset = 1;
    }
    // Server is initiative to close the connection
    if (flags & Server::CLOSE_ACTIVELY) {
        conn->close_actively = 1;
    }
    if (conn->close_force) {
        goto _do_close;
    } else if (conn->closing) {
        openswoole_warning("session#%ld is closing", session_id);
        return false;
    } else if (conn->closed) {
        return false;
    } else {
    _do_close:
        conn->closing = 1;
        if (server_->onClose != nullptr) {
            DataHead info{};
            info.fd = session_id;
            if (conn->close_actively) {
                info.reactor_id = -1;
            } else {
                info.reactor_id = conn->reactor_id;
            }
            info.server_fd = conn->server_fd;
            server_->onClose(server_, &info);
        }
        conn->closing = 0;
        conn->closed = 1;
        conn->close_errno = 0;

        if (conn->socket == nullptr) {
            openswoole_warning("session#%ld->socket is nullptr", session_id);
            return false;
        }

        if (Buffer::empty(conn->socket->out_buffer) || conn->peer_closed || conn->close_force) {
            Reactor *reactor = OpenSwooleTG.reactor;
            return Server::close_connection(reactor, conn->socket) == OSW_OK;
        } else {
            BufferChunk *chunk = conn->socket->out_buffer->alloc(BufferChunk::TYPE_CLOSE, 0);
            chunk->value.data.val1 = _send.info.type;
            conn->close_queued = 1;
            return true;
        }
    }
}

bool BaseFactory::finish(SendData *data) {
    SessionId session_id = data->info.fd;

    Session *session = server_->get_session(session_id);
    if (session->reactor_id != OpenSwooleG.process_id) {
        openswoole_trace("session->reactor_id=%d, OpenSwooleG.process_id=%d", session->reactor_id, OpenSwooleG.process_id);
        Worker *worker = server_->gs->event_workers.get_worker(session->reactor_id);
        EventData proxy_msg{};

        if (data->info.type == OSW_SERVER_EVENT_RECV_DATA) {
            proxy_msg.info.fd = session_id;
            proxy_msg.info.reactor_id = OpenSwooleG.process_id;
            proxy_msg.info.type = OSW_SERVER_EVENT_PROXY_START;

            size_t send_n = data->info.len;
            size_t offset = 0;

            while (send_n > 0) {
                if (send_n > OSW_IPC_BUFFER_SIZE) {
                    proxy_msg.info.len = OSW_IPC_BUFFER_SIZE;
                } else {
                    proxy_msg.info.type = OSW_SERVER_EVENT_PROXY_END;
                    proxy_msg.info.len = send_n;
                }
                memcpy(proxy_msg.data, data->data + offset, proxy_msg.info.len);
                send_n -= proxy_msg.info.len;
                offset += proxy_msg.info.len;
                size_t __len = sizeof(proxy_msg.info) + proxy_msg.info.len;
                if (worker->pipe_master->send_async((const char *) &proxy_msg, __len) < 0) {
                    openswoole_sys_warning("failed to send %lu bytes to pipe_master", __len);
                    return false;
                }
            }
            openswoole_trace(
                "proxy message, fd=%d, len=%ld", worker->pipe_master->fd, sizeof(proxy_msg.info) + proxy_msg.info.len);
        } else if (data->info.type == OSW_SERVER_EVENT_SEND_FILE) {
            memcpy(&proxy_msg.info, &data->info, sizeof(proxy_msg.info));
            memcpy(proxy_msg.data, data->data, data->info.len);
            size_t __len = sizeof(proxy_msg.info) + proxy_msg.info.len;
            return worker->pipe_master->send_async((const char *) &proxy_msg, __len);
        } else {
            openswoole_warning("unkown event type[%d]", data->info.type);
            return false;
        }
        return true;
    } else {
        return server_->send_to_connection(data) == OSW_OK;
    }
}

BaseFactory::~BaseFactory() {}

}  // namespace openswoole
