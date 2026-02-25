/*
 +----------------------------------------------------------------------+
 | Open Swoole                                                          |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 */

#include "swoole.h"
#include "swoole_socket.h"
#include "swoole_reactor.h"

#ifdef HAVE_IO_URING
#include <liburing.h>
#include <poll.h>

namespace swoole {

using network::Socket;

enum IoUringOpType {
    IO_URING_OP_POLL = 0,
    IO_URING_OP_CANCEL = 1,
    // Reserved for future async I/O operations
    // IO_URING_OP_READ = 2,
    // IO_URING_OP_WRITE = 3,
    // IO_URING_OP_ACCEPT = 4,
};

/**
 * Tagged pointer encoding: Socket* + op type in 64-bit user_data.
 * Socket* is 8-byte aligned, so low 4 bits are always zero and
 * available for storing the operation tag.
 */
static const uint64_t IO_URING_TAG_BITS = 4;
static const uint64_t IO_URING_TAG_MASK = (1ULL << IO_URING_TAG_BITS) - 1;

static inline uint64_t encode_user_data(Socket *socket, IoUringOpType op) {
    return reinterpret_cast<uint64_t>(socket) | static_cast<uint64_t>(op);
}

static inline Socket *decode_socket(uint64_t user_data) {
    return reinterpret_cast<Socket *>(user_data & ~IO_URING_TAG_MASK);
}

static inline IoUringOpType decode_op(uint64_t user_data) {
    return static_cast<IoUringOpType>(user_data & IO_URING_TAG_MASK);
}

class ReactorIoUring : public ReactorImpl {
  private:
    struct io_uring ring_;
    bool ring_initialized_ = false;
    bool use_multishot_ = false;
    struct io_uring_cqe **cqes_ = nullptr;

  public:
    ReactorIoUring(Reactor *_reactor, int max_events);
    ~ReactorIoUring();
    bool ready() override;
    int add(Socket *socket, int events) override;
    int set(Socket *socket, int events) override;
    int del(Socket *socket) override;
    int wait(struct timeval *) override;

    static inline unsigned get_poll_mask(int sw_events) {
        unsigned mask = 0;
        if (Reactor::isset_read_event(sw_events)) {
            mask |= POLLIN;
        }
        if (Reactor::isset_write_event(sw_events)) {
            mask |= POLLOUT;
        }
        if (Reactor::isset_error_event(sw_events)) {
            mask |= POLLRDHUP | POLLHUP | POLLERR;
        }
        return mask;
    }

  private:
    int submit_poll_add(Socket *socket, int events);
    int submit_poll_remove(Socket *socket);
};

ReactorImpl *make_reactor_io_uring(Reactor *_reactor, int max_events) {
    return new ReactorIoUring(_reactor, max_events);
}

ReactorIoUring::ReactorIoUring(Reactor *_reactor, int max_events) : ReactorImpl(_reactor) {
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));

    if (io_uring_queue_init_params(max_events, &ring_, &params) < 0) {
        swoole_sys_warning("io_uring_queue_init_params failed");
        return;
    }
    ring_initialized_ = true;

    // Detect multishot poll support (kernel 5.13+)
#ifdef IORING_POLL_ADD_MULTI
    {
        struct io_uring_probe *probe = io_uring_get_probe_ring(&ring_);
        if (probe) {
            if (io_uring_opcode_supported(probe, IORING_OP_POLL_ADD)) {
                use_multishot_ = true;
            }
            io_uring_free_probe(probe);
        }
    }
#endif

    cqes_ = new struct io_uring_cqe *[max_events];
    reactor_->max_event_num = max_events;
    reactor_->native_handle = ring_.ring_fd;
}

bool ReactorIoUring::ready() {
    return ring_initialized_;
}

ReactorIoUring::~ReactorIoUring() {
    if (ring_initialized_) {
        io_uring_queue_exit(&ring_);
    }
    delete[] cqes_;
}

int ReactorIoUring::submit_poll_add(Socket *socket, int events) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        swoole_sys_warning("io_uring_get_sqe failed (SQ full), fd=%d", socket->fd);
        return SW_ERR;
    }

    unsigned poll_mask = get_poll_mask(events);
    io_uring_prep_poll_add(sqe, socket->fd, poll_mask);
    io_uring_sqe_set_data64(sqe, encode_user_data(socket, IO_URING_OP_POLL));

#ifdef IORING_POLL_ADD_MULTI
    if (use_multishot_) {
        sqe->len |= IORING_POLL_ADD_MULTI;
    }
#endif

    return SW_OK;
}

int ReactorIoUring::submit_poll_remove(Socket *socket) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        swoole_sys_warning("io_uring_get_sqe failed (SQ full) for cancel, fd=%d", socket->fd);
        return SW_ERR;
    }

    io_uring_prep_poll_remove(sqe, encode_user_data(socket, IO_URING_OP_POLL));
    io_uring_sqe_set_data64(sqe, encode_user_data(socket, IO_URING_OP_CANCEL));

    return SW_OK;
}

int ReactorIoUring::add(Socket *socket, int events) {
    if (submit_poll_add(socket, events) < 0) {
        swoole_sys_warning(
            "failed to add events[fd=%d#%d, type=%d, events=%d]", socket->fd, reactor_->id, socket->fd_type, events);
        return SW_ERR;
    }

    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        swoole_sys_warning(
            "io_uring_submit failed to add events[fd=%d#%d, type=%d, events=%d]",
            socket->fd, reactor_->id, socket->fd_type, events);
        return SW_ERR;
    }

    reactor_->_add(socket, events);
    swoole_trace_log(
        SW_TRACE_EVENT, "add events[fd=%d#%d, type=%d, events=%d]", socket->fd, reactor_->id, socket->fd_type, events);

    return SW_OK;
}

int ReactorIoUring::del(Socket *_socket) {
    if (_socket->removed) {
        swoole_error_log(SW_LOG_WARNING,
                         SW_ERROR_EVENT_SOCKET_REMOVED,
                         "failed to delete events[%d], it has already been removed",
                         _socket->fd);
        return SW_ERR;
    }

    submit_poll_remove(_socket);
    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        after_removal_failure(_socket);
        return SW_ERR;
    }

    swoole_trace_log(SW_TRACE_REACTOR, "remove event[reactor_id=%d|fd=%d]", reactor_->id, _socket->fd);
    reactor_->_del(_socket);

    return SW_OK;
}

int ReactorIoUring::set(Socket *socket, int events) {
    // Remove old poll registration and add new one in same batch
    submit_poll_remove(socket);
    if (submit_poll_add(socket, events) < 0) {
        swoole_sys_warning(
            "failed to set events[fd=%d#%d, type=%d, events=%d]", socket->fd, reactor_->id, socket->fd_type, events);
        return SW_ERR;
    }

    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        swoole_sys_warning(
            "io_uring_submit failed to set events[fd=%d#%d, type=%d, events=%d]",
            socket->fd, reactor_->id, socket->fd_type, events);
        return SW_ERR;
    }

    swoole_trace_log(SW_TRACE_EVENT, "set event[reactor_id=%d, fd=%d, events=%d]", reactor_->id, socket->fd, events);
    reactor_->_set(socket, events);

    return SW_OK;
}

int ReactorIoUring::wait(struct timeval *timeo) {
    Event event;
    ReactorHandler handler;
    int i, n, ret;

    int reactor_id = reactor_->id;
    int max_event_num = reactor_->max_event_num;
    bool need_submit;

    if (reactor_->timeout_msec == 0) {
        if (timeo == nullptr) {
            reactor_->timeout_msec = -1;
        } else {
            reactor_->timeout_msec = timeo->tv_sec * 1000 + timeo->tv_usec / 1000;
        }
    }

    reactor_->before_wait();

    while (reactor_->running) {
        if (reactor_->onBegin != nullptr) {
            reactor_->onBegin(reactor_);
        }

        int timeout_msec = reactor_->get_timeout_msec();
        struct io_uring_cqe *cqe;

        if (timeout_msec < 0) {
            ret = io_uring_wait_cqe(&ring_, &cqe);
        } else {
            struct __kernel_timespec ts;
            ts.tv_sec = timeout_msec / 1000;
            ts.tv_nsec = (timeout_msec % 1000) * 1000000LL;
            ret = io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
        }

        if (ret < 0) {
            if (ret == -ETIME || ret == -EINTR) {
                if (ret == -ETIME) {
                    reactor_->execute_end_callbacks(true);
                    SW_REACTOR_CONTINUE;
                }
                // EINTR: retry
                goto _continue;
            }
            swoole_warning("[Reactor#%d] io_uring_wait_cqe failed, ret=%d", reactor_id, ret);
            return SW_ERR;
        }

        n = io_uring_peek_batch_cqe(&ring_, cqes_, max_event_num);
        if (n == 0) {
            reactor_->execute_end_callbacks(true);
            SW_REACTOR_CONTINUE;
        }

        need_submit = false;

        for (i = 0; i < n; i++) {
            struct io_uring_cqe *completion = cqes_[i];
            uint64_t user_data = io_uring_cqe_get_data64(completion);
            IoUringOpType op = decode_op(user_data);

            // Ignore cancel completions
            if (op == IO_URING_OP_CANCEL) {
                continue;
            }

            Socket *socket = decode_socket(user_data);
            if (!socket) {
                continue;
            }

            // CQE result < 0 is an error; result > 0 is the poll mask
            int revents;
            if (completion->res < 0) {
                // Skip cancelled polls (from set() or del())
                if (completion->res == -ECANCELED) {
                    continue;
                }
                // Treat other negative results as error events
                revents = POLLERR;
            } else {
                revents = completion->res;
            }

            event.reactor_id = reactor_id;
            event.socket = socket;
            event.type = socket->fd_type;
            event.fd = socket->fd;

            if (revents & (POLLRDHUP | POLLERR | POLLHUP)) {
                socket->event_hup = 1;
            }

            // read
            if ((revents & POLLIN) && !socket->removed) {
                handler = reactor_->get_handler(SW_EVENT_READ, event.type);
                ret = handler(reactor_, &event);
                if (ret < 0) {
                    swoole_sys_warning("POLLIN handle failed. fd=%d", event.fd);
                }
            }
            // write
            if ((revents & POLLOUT) && !socket->removed) {
                handler = reactor_->get_handler(SW_EVENT_WRITE, event.type);
                ret = handler(reactor_, &event);
                if (ret < 0) {
                    swoole_sys_warning("POLLOUT handle failed. fd=%d", event.fd);
                }
            }
            // error
            if ((revents & (POLLRDHUP | POLLERR | POLLHUP)) && !socket->removed) {
                if ((revents & POLLIN) || (revents & POLLOUT)) {
                    goto _next;
                }
                handler = reactor_->get_error_handler(event.type);
                ret = handler(reactor_, &event);
                if (ret < 0) {
                    swoole_sys_warning("POLLERR handle failed. fd=%d", event.fd);
                }
            }

        _next:
            if (!socket->removed && (socket->events & SW_EVENT_ONCE)) {
                reactor_->_del(socket);
            }
            // Re-arm poll if needed
            else if (!socket->removed) {
                bool needs_rearm;
                if (use_multishot_) {
#ifdef IORING_CQE_F_MORE
                    // Kernel deactivated multishot poll when this flag is absent
                    needs_rearm = !(completion->flags & IORING_CQE_F_MORE);
#else
                    needs_rearm = false;
#endif
                } else {
                    // Single-shot polls always need re-arm
                    needs_rearm = true;
                }
                if (needs_rearm) {
                    submit_poll_add(socket, socket->events);
                    need_submit = true;
                }
            }
        }

        io_uring_cq_advance(&ring_, n);

        // Batch submit any re-armed polls
        if (need_submit) {
            io_uring_submit(&ring_);
        }

    _continue:
        reactor_->execute_end_callbacks(false);
        SW_REACTOR_CONTINUE;
    }
    return 0;
}

}  // namespace swoole
#endif
