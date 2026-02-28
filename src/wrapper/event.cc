/**
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

#include "openswoole_api.h"
#include "openswoole_reactor.h"
#include "openswoole_client.h"
#include "openswoole_coroutine_socket.h"
#include "openswoole_coroutine_system.h"

#include <mutex>

using namespace openswoole;

using openswoole::network::Socket;

static std::mutex init_lock;

#ifdef __MACH__
Reactor *osw_reactor() {
    return OpenSwooleTG.reactor;
}
#endif

int openswoole_event_init(int flags) {
    if (!OpenSwooleG.init) {
        std::unique_lock<std::mutex> lock(init_lock);
        openswoole_init();
    }

    Reactor *reactor = new Reactor(OSW_REACTOR_MAXEVENTS, static_cast<Reactor::Type>(OpenSwooleG.reactor_type));
    if (!reactor->ready()) {
        return OSW_ERR;
    }

    if (flags & OSW_EVENTLOOP_WAIT_EXIT) {
        reactor->wait_exit = 1;
    }

    coroutine::Socket::init_reactor(reactor);
    coroutine::System::init_reactor(reactor);
    network::Client::init_reactor(reactor);

    OpenSwooleTG.reactor = reactor;

    return OSW_OK;
}

int openswoole_event_add(Socket *socket, int events) {
    return OpenSwooleTG.reactor->add(socket, events);
}

int openswoole_event_add_or_update(openswoole::network::Socket *_socket, int event) {
    if (event == OSW_EVENT_READ) {
        return OpenSwooleTG.reactor->add_read_event(_socket);
    } else if (event == OSW_EVENT_WRITE) {
        return OpenSwooleTG.reactor->add_write_event(_socket);
    } else {
        assert(0);
        return OSW_ERR;
    }
}

int openswoole_event_set(Socket *socket, int events) {
    return OpenSwooleTG.reactor->set(socket, events);
}

int openswoole_event_del(Socket *socket) {
    return OpenSwooleTG.reactor->del(socket);
}

int openswoole_event_wait() {
    Reactor *reactor = OpenSwooleTG.reactor;
    int retval = 0;
    if (!reactor->wait_exit or !reactor->if_exit()) {
        retval = OpenSwooleTG.reactor->wait(nullptr);
    }
    openswoole_event_free();
    return retval;
}

int openswoole_event_free() {
    if (!OpenSwooleTG.reactor) {
        return OSW_ERR;
    }
    delete OpenSwooleTG.reactor;
    OpenSwooleTG.reactor = nullptr;
    return OSW_OK;
}

void openswoole_event_defer(Callback cb, void *private_data) {
    OpenSwooleTG.reactor->defer(cb, private_data);
}

/**
 * @return OSW_OK or OSW_ERR
 */
ssize_t openswoole_event_write(Socket *socket, const void *data, size_t len) {
    return OpenSwooleTG.reactor->write(OpenSwooleTG.reactor, socket, data, len);
}

ssize_t openswoole_event_writev(openswoole::network::Socket *socket, const iovec *iov, size_t iovcnt) {
    return OpenSwooleTG.reactor->writev(OpenSwooleTG.reactor, socket, iov, iovcnt);
}

bool openswoole_event_set_handler(int fdtype, ReactorHandler handler) {
    return OpenSwooleTG.reactor->set_handler(fdtype, handler);
}

bool openswoole_event_isset_handler(int fdtype) {
    return OpenSwooleTG.reactor->isset_handler(fdtype);
}

bool openswoole_event_is_available() {
    return OpenSwooleTG.reactor and !OpenSwooleTG.reactor->destroyed;
}
