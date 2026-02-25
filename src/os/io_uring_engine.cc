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

#include "swoole_io_uring.h"

#ifdef HAVE_IO_URING

#include "swoole_api.h"
#include "swoole_socket.h"
#include "swoole_reactor.h"
#include "swoole_coroutine.h"

#include <sys/eventfd.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>

namespace swoole {

using network::Socket;

static constexpr int IO_URING_UNSUPPORTED = -2;
static constexpr unsigned int IO_URING_ENTRIES = 256;

IoUringEngine::IoUringEngine() : event_fd_(-1), event_socket_(nullptr), caps_{}, pending_count_(0) {
    if (!SwooleTG.reactor) {
        swoole_warning("no event loop, cannot initialize io_uring engine");
        throw swoole::Exception(SW_ERROR_WRONG_OPERATION);
    }

    struct io_uring_params params = {};
    int ret = io_uring_queue_init_params(IO_URING_ENTRIES, &ring_, &params);
    if (ret < 0) {
        swoole_warning("io_uring_queue_init_params() failed: %s", strerror(-ret));
        throw swoole::Exception(SW_ERROR_SYSTEM_CALL_FAIL);
    }

    // Probe capabilities and run smoke test BEFORE registering the eventfd.
    // This avoids stale eventfd notifications and leftover CQEs from
    // io_uring_wait_cqe_timeout's internal timeout SQE.
    detect_capabilities();

    event_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (event_fd_ < 0) {
        io_uring_queue_exit(&ring_);
        swoole_warning("eventfd() failed: %s", strerror(errno));
        throw swoole::Exception(SW_ERROR_SYSTEM_CALL_FAIL);
    }

    ret = io_uring_register_eventfd(&ring_, event_fd_);
    if (ret < 0) {
        ::close(event_fd_);
        io_uring_queue_exit(&ring_);
        swoole_warning("io_uring_register_eventfd() failed: %s", strerror(-ret));
        throw swoole::Exception(SW_ERROR_SYSTEM_CALL_FAIL);
    }

    event_socket_ = make_socket(event_fd_, (FdType) SW_FD_IO_URING);
    if (!event_socket_) {
        ::close(event_fd_);
        io_uring_queue_exit(&ring_);
        swoole_warning("make_socket() failed for io_uring eventfd");
        throw swoole::Exception(SW_ERROR_SYSTEM_CALL_FAIL);
    }

    if (swoole_event_add(event_socket_, SW_EVENT_READ) < 0) {
        event_socket_->free();
        ::close(event_fd_);
        io_uring_queue_exit(&ring_);
        swoole_warning("swoole_event_add() failed for io_uring eventfd");
        throw swoole::Exception(SW_ERROR_SYSTEM_CALL_FAIL);
    }

    sw_reactor()->set_exit_condition(
        Reactor::EXIT_CONDITION_IO_URING_TASK, [](Reactor *reactor, size_t &event_num) -> bool {
            if (SwooleTG.io_uring_engine && SwooleTG.io_uring_engine->get_pending_count() == 0) {
                event_num--;
            }
            return true;
        });

    sw_reactor()->add_destroy_callback([](void *data) {
        if (!SwooleTG.io_uring_engine) {
            return;
        }
        delete SwooleTG.io_uring_engine;
        SwooleTG.io_uring_engine = nullptr;
    });

    SwooleTG.io_uring_engine = this;

    swoole_trace_log(SW_TRACE_AIO,
                     "io_uring file I/O engine initialized (openat=%d, read=%d, write=%d, statx=%d, "
                     "unlinkat=%d, mkdirat=%d, renameat=%d, fsync=%d)",
                     caps_.openat, caps_.read, caps_.write, caps_.statx,
                     caps_.unlinkat, caps_.mkdirat, caps_.renameat, caps_.fsync);
}

IoUringEngine::~IoUringEngine() {
    if (event_socket_) {
        swoole_event_del(event_socket_);
        event_socket_->free();
        event_socket_ = nullptr;
    }
    if (event_fd_ >= 0) {
        ::close(event_fd_);
        event_fd_ = -1;
    }
    io_uring_queue_exit(&ring_);
}

void IoUringEngine::detect_capabilities() {
    struct io_uring_probe *probe = io_uring_get_probe_ring(&ring_);
    if (!probe) {
        swoole_trace_log(SW_TRACE_AIO, "io_uring_get_probe_ring() returned NULL, no capabilities detected");
        return;
    }

    caps_.openat = io_uring_opcode_supported(probe, IORING_OP_OPENAT);
    caps_.read = io_uring_opcode_supported(probe, IORING_OP_READ);
    caps_.write = io_uring_opcode_supported(probe, IORING_OP_WRITE);
    caps_.statx = io_uring_opcode_supported(probe, IORING_OP_STATX);
    caps_.fsync = io_uring_opcode_supported(probe, IORING_OP_FSYNC);

#ifdef IORING_OP_UNLINKAT
    caps_.unlinkat = io_uring_opcode_supported(probe, IORING_OP_UNLINKAT);
#endif
#ifdef IORING_OP_MKDIRAT
    caps_.mkdirat = io_uring_opcode_supported(probe, IORING_OP_MKDIRAT);
#endif
#ifdef IORING_OP_RENAMEAT
    caps_.renameat = io_uring_opcode_supported(probe, IORING_OP_RENAMEAT);
#endif

    // Verify ring can do actual file I/O with an openat smoke test on /dev/null
    if (caps_.openat) {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
        if (sqe) {
            io_uring_prep_openat(sqe, AT_FDCWD, "/dev/null", O_RDONLY, 0);
            io_uring_sqe_set_data(sqe, nullptr);
            io_uring_submit(&ring_);
            struct io_uring_cqe *cqe;
            struct __kernel_timespec ts = {.tv_sec = 1, .tv_nsec = 0};
            int ret = io_uring_wait_cqe_timeout(&ring_, &cqe, &ts);
            if (ret < 0 || cqe->res < 0) {
                swoole_warning("io_uring file I/O disabled: openat smoke test failed (ret=%d, res=%d). "
                               "File operations will use the thread pool. "
                               "If running in Docker, enable io_uring with: "
                               "--security-opt seccomp=unconfined or a custom seccomp profile allowing io_uring",
                               ret, ret < 0 ? 0 : cqe->res);
                if (ret >= 0) {
                    io_uring_cqe_seen(&ring_, cqe);
                }
                memset(&caps_, 0, sizeof(caps_));
                io_uring_free_probe(probe);
                return;
            }
            // Close the fd we opened for the smoke test
            if (cqe->res >= 0) {
                ::close(cqe->res);
            }
            io_uring_cqe_seen(&ring_, cqe);

            // Drain any leftover CQEs from io_uring_wait_cqe_timeout's
            // internal timeout SQE to keep the ring clean
            struct io_uring_cqe *drain_cqe;
            while (io_uring_peek_cqe(&ring_, &drain_cqe) == 0) {
                io_uring_cqe_seen(&ring_, drain_cqe);
            }
        }
    }

    io_uring_free_probe(probe);
}

int IoUringEngine::submit_and_yield(IoUringRequest *req) {
    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        delete req;
        errno = -ret;
        return -1;
    }

    pending_count_++;

    Coroutine *co = Coroutine::get_current();
    req->coroutine = co;

    if (!co->yield_ex(30.0)) {
        // Timeout or cancellation — mark request as abandoned so process_completions() cleans it up
        req->abandoned = true;
        pending_count_--;
        errno = co->is_timedout() ? ETIMEDOUT : ECANCELED;
        return -1;
    }

    pending_count_--;

    if (req->result < 0) {
        errno = req->error;
        int result = -1;
        delete req;
        return result;
    }

    int result = req->result;
    delete req;
    return result;
}

void IoUringEngine::process_completions() {
    struct io_uring_cqe *cqe;
    unsigned head;
    unsigned count = 0;

    io_uring_for_each_cqe(&ring_, head, cqe) {
        count++;
        auto *req = static_cast<IoUringRequest *>(io_uring_cqe_get_data(cqe));
        if (!req) {
            continue;
        }

        if (req->abandoned) {
            delete req;
            continue;
        }

        if (cqe->res < 0) {
            req->result = -1;
            req->error = -cqe->res;
        } else {
            req->result = cqe->res;
            req->error = 0;
        }

        req->coroutine->resume();
    }

    io_uring_cq_advance(&ring_, count);
}

int IoUringEngine::on_event(Reactor *reactor, Event *event) {
    auto *engine = SwooleTG.io_uring_engine;
    if (!engine) {
        return SW_ERR;
    }

    uint64_t val;
    // Drain the eventfd
    while (::read(engine->event_fd_, &val, sizeof(val)) == sizeof(val)) {
    }

    engine->process_completions();
    return SW_OK;
}

int IoUringEngine::open(const char *pathname, int flags, mode_t mode) {
    if (!caps_.openat) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_openat(sqe, AT_FDCWD, pathname, flags, mode);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
}

ssize_t IoUringEngine::read(int fd, void *buf, size_t count, off_t offset) {
    if (!caps_.read) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_read(sqe, fd, buf, count, offset >= 0 ? offset : -1);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
}

ssize_t IoUringEngine::write(int fd, const void *buf, size_t count, off_t offset) {
    if (!caps_.write) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_write(sqe, fd, buf, count, offset >= 0 ? offset : -1);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
}

int IoUringEngine::fstat(int fd, struct stat *statbuf) {
    if (!caps_.statx) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    // Use STATX_BASIC_STATS to get all fields equivalent to fstat
    static const char empty_path[] = "";
    io_uring_prep_statx(sqe, fd, empty_path, AT_EMPTY_PATH, STATX_BASIC_STATS, &req->statx_buf);
    io_uring_sqe_set_data(sqe, req);

    int ret = io_uring_submit(&ring_);
    if (ret < 0) {
        delete req;
        errno = -ret;
        return -1;
    }

    pending_count_++;

    Coroutine *co = Coroutine::get_current();
    req->coroutine = co;

    if (!co->yield_ex(30.0)) {
        req->abandoned = true;
        pending_count_--;
        errno = co->is_timedout() ? ETIMEDOUT : ECANCELED;
        return -1;
    }

    pending_count_--;

    if (req->result < 0) {
        errno = req->error;
        delete req;
        return -1;
    }

    // Convert statx to stat
    struct statx *stx = &req->statx_buf;
    memset(statbuf, 0, sizeof(*statbuf));
    statbuf->st_dev = makedev(stx->stx_dev_major, stx->stx_dev_minor);
    statbuf->st_ino = stx->stx_ino;
    statbuf->st_mode = stx->stx_mode;
    statbuf->st_nlink = stx->stx_nlink;
    statbuf->st_uid = stx->stx_uid;
    statbuf->st_gid = stx->stx_gid;
    statbuf->st_rdev = makedev(stx->stx_rdev_major, stx->stx_rdev_minor);
    statbuf->st_size = stx->stx_size;
    statbuf->st_blksize = stx->stx_blksize;
    statbuf->st_blocks = stx->stx_blocks;
    statbuf->st_atim.tv_sec = stx->stx_atime.tv_sec;
    statbuf->st_atim.tv_nsec = stx->stx_atime.tv_nsec;
    statbuf->st_mtim.tv_sec = stx->stx_mtime.tv_sec;
    statbuf->st_mtim.tv_nsec = stx->stx_mtime.tv_nsec;
    statbuf->st_ctim.tv_sec = stx->stx_ctime.tv_sec;
    statbuf->st_ctim.tv_nsec = stx->stx_ctime.tv_nsec;

    delete req;
    return 0;
}

int IoUringEngine::fsync(int fd) {
    if (!caps_.fsync) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_fsync(sqe, fd, 0);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
}

int IoUringEngine::mkdir(const char *pathname, mode_t mode) {
#ifdef IORING_OP_MKDIRAT
    if (!caps_.mkdirat) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_mkdirat(sqe, AT_FDCWD, pathname, mode);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
#else
    return IO_URING_UNSUPPORTED;
#endif
}

int IoUringEngine::rmdir(const char *pathname) {
#ifdef IORING_OP_UNLINKAT
    if (!caps_.unlinkat) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_unlinkat(sqe, AT_FDCWD, pathname, AT_REMOVEDIR);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
#else
    return IO_URING_UNSUPPORTED;
#endif
}

int IoUringEngine::unlink(const char *pathname) {
#ifdef IORING_OP_UNLINKAT
    if (!caps_.unlinkat) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_unlinkat(sqe, AT_FDCWD, pathname, 0);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
#else
    return IO_URING_UNSUPPORTED;
#endif
}

int IoUringEngine::rename(const char *oldpath, const char *newpath) {
#ifdef IORING_OP_RENAMEAT
    if (!caps_.renameat) {
        return IO_URING_UNSUPPORTED;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring_);
    if (!sqe) {
        errno = EAGAIN;
        return -1;
    }

    auto *req = new IoUringRequest{};
    io_uring_prep_renameat(sqe, AT_FDCWD, oldpath, AT_FDCWD, newpath, 0);
    io_uring_sqe_set_data(sqe, req);

    return submit_and_yield(req);
#else
    return IO_URING_UNSUPPORTED;
#endif
}

IoUringEngine *get_or_create_io_uring_engine() {
    if (SwooleTG.io_uring_engine) {
        return SwooleTG.io_uring_engine;
    }

    if (!SwooleTG.reactor) {
        return nullptr;
    }

    try {
        return new IoUringEngine();
    } catch (...) {
        swoole_warning("failed to create io_uring file I/O engine, falling back to thread pool");
        return nullptr;
    }
}

}  // namespace swoole

#endif
