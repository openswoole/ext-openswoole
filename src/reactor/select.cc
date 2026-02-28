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
#include "openswoole_socket.h"
#include "openswoole_reactor.h"
#include <unordered_map>

#include <sys/select.h>

namespace openswoole {

using network::Socket;

class ReactorSelect : public ReactorImpl {
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    std::unordered_map<int, Socket *> fds;
    int maxfd;

  public:
    ReactorSelect(Reactor *reactor);
    ~ReactorSelect() {}
    bool ready() override {
        return true;
    }
    int add(Socket *socket, int events) override;
    int set(Socket *socket, int events) override;
    int del(Socket *socket) override;
    int wait(struct timeval *) override;
};

#define OSW_FD_SET(fd, set)                                                                                             \
    do {                                                                                                               \
        if (fd < FD_SETSIZE) FD_SET(fd, set);                                                                          \
    } while (0)

#define OSW_FD_CLR(fd, set)                                                                                             \
    do {                                                                                                               \
        if (fd < FD_SETSIZE) FD_CLR(fd, set);                                                                          \
    } while (0)

#define OSW_FD_ISSET(fd, set) ((fd < FD_SETSIZE) && FD_ISSET(fd, set))

ReactorImpl *make_reactor_select(Reactor *_reactor) {
    return new ReactorSelect(_reactor);
}

ReactorSelect::ReactorSelect(Reactor *reactor) : ReactorImpl(reactor) {
    maxfd = 0;
}

int ReactorSelect::add(Socket *socket, int events) {
    int fd = socket->fd;
    if (fd > FD_SETSIZE) {
        openswoole_warning("max fd value is FD_SETSIZE(%d).\n", FD_SETSIZE);
        return OSW_ERR;
    }

    reactor_->_add(socket, events);
    fds.emplace(fd, socket);
    if (fd > maxfd) {
        maxfd = fd;
    }

    return OSW_OK;
}

int ReactorSelect::del(Socket *socket) {
    if (socket->removed) {
        openswoole_error_log(OSW_LOG_WARNING,
                         OSW_ERROR_EVENT_SOCKET_REMOVED,
                         "failed to delete event[%d], it has already been removed",
                         socket->fd);
        return OSW_ERR;
    }
    int fd = socket->fd;
    if (fds.erase(fd) == 0) {
        openswoole_warning("swReactorSelect: fd[%d] not found", fd);
        return OSW_ERR;
    }
    OSW_FD_CLR(fd, &rfds);
    OSW_FD_CLR(fd, &wfds);
    OSW_FD_CLR(fd, &efds);
    reactor_->_del(socket);
    return OSW_OK;
}

int ReactorSelect::set(Socket *socket, int events) {
    auto i = fds.find(socket->fd);
    if (i == fds.end()) {
        openswoole_warning("swReactorSelect: sock[%d] not found", socket->fd);
        return OSW_ERR;
    }
    reactor_->_set(socket, events);
    return OSW_OK;
}

int ReactorSelect::wait(struct timeval *timeo) {
    Event event;
    ReactorHandler handler;
    struct timeval timeout;
    int ret;

    if (reactor_->timeout_msec == 0) {
        if (timeo == nullptr) {
            reactor_->timeout_msec = -1;
        } else {
            reactor_->timeout_msec = timeo->tv_sec * 1000 + timeo->tv_usec / 1000;
        }
    }

    reactor_->before_wait();

    while (reactor_->running) {
        FD_ZERO(&(rfds));
        FD_ZERO(&(wfds));
        FD_ZERO(&(efds));

        if (reactor_->onBegin != nullptr) {
            reactor_->onBegin(reactor_);
        }

        for (auto i = fds.begin(); i != fds.end(); i++) {
            int fd = i->first;
            int events = i->second->events;
            if (Reactor::isset_read_event(events)) {
                OSW_FD_SET(fd, &(rfds));
            }
            if (Reactor::isset_write_event(events)) {
                OSW_FD_SET(fd, &(wfds));
            }
            if (Reactor::isset_error_event(events)) {
                OSW_FD_SET(fd, &(efds));
            }
        }

        if (reactor_->timeout_msec < 0) {
            timeout.tv_sec = UINT_MAX;
            timeout.tv_usec = 0;
        } else if (reactor_->defer_tasks) {
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
        } else {
            timeout.tv_sec = reactor_->timeout_msec / 1000;
            timeout.tv_usec = reactor_->timeout_msec - timeout.tv_sec * 1000;
        }

        ret = select(maxfd + 1, &(rfds), &(wfds), &(efds), &timeout);
        if (ret < 0) {
            if (!reactor_->catch_error()) {
                openswoole_sys_warning("select error");
                break;
            } else {
                goto _continue;
            }
        } else if (ret == 0) {
            reactor_->execute_end_callbacks(true);
            OSW_REACTOR_CONTINUE;
        } else {
            for (int fd = 0; fd <= maxfd; fd++) {
                auto i = fds.find(fd);
                if (i == fds.end()) {
                    continue;
                }
                event.socket = i->second;
                event.fd = event.socket->fd;
                event.reactor_id = reactor_->id;
                event.type = event.socket->fd_type;

                // read
                if (OSW_FD_ISSET(event.fd, &(rfds)) && !event.socket->removed) {
                    handler = reactor_->get_handler(OSW_EVENT_READ, event.type);
                    ret = handler(reactor_, &event);
                    if (ret < 0) {
                        openswoole_sys_warning(
                            "[Reactor#%d] select event[type=READ, fd=%d] handler fail", reactor_->id, event.fd);
                    }
                }
                // write
                if (OSW_FD_ISSET(event.fd, &(wfds)) && !event.socket->removed) {
                    handler = reactor_->get_handler(OSW_EVENT_WRITE, event.type);
                    ret = handler(reactor_, &event);
                    if (ret < 0) {
                        openswoole_sys_warning(
                            "[Reactor#%d] select event[type=WRITE, fd=%d] handler fail", reactor_->id, event.fd);
                    }
                }
                // error
                if (OSW_FD_ISSET(event.fd, &(efds)) && !event.socket->removed) {
                    handler = reactor_->get_handler(OSW_EVENT_ERROR, event.type);
                    ret = handler(reactor_, &event);
                    if (ret < 0) {
                        openswoole_sys_warning(
                            "[Reactor#%d] select event[type=ERROR, fd=%d] handler fail", reactor_->id, event.fd);
                    }
                }
                if (!event.socket->removed && (event.socket->events & OSW_EVENT_ONCE)) {
                    del(event.socket);
                }
            }
        }
    _continue:
        reactor_->execute_end_callbacks(false);
        OSW_REACTOR_CONTINUE;
    }
    return OSW_OK;
}

}  // namespace openswoole
