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
*/

#pragma once

#include "openswoole.h"

#ifdef HAVE_IO_URING
#include <liburing.h>
#include <sys/stat.h>
#include <linux/stat.h>

namespace openswoole {

class Coroutine;

struct IoUringRequest {
    Coroutine *coroutine;
    int result;
    int error;
    bool abandoned;
    struct statx statx_buf;
};

struct IoUringCapabilities {
    bool openat;
    bool read;
    bool write;
    bool statx;
    bool unlinkat;
    bool mkdirat;
    bool renameat;
    bool fsync;
};

class IoUringEngine {
  public:
    IoUringEngine();
    ~IoUringEngine();

    int open(const char *pathname, int flags, mode_t mode);
    ssize_t read(int fd, void *buf, size_t count, off_t offset = -1);
    ssize_t write(int fd, const void *buf, size_t count, off_t offset = -1);
    int fstat(int fd, struct stat *statbuf);
    int fsync(int fd);
    int mkdir(const char *pathname, mode_t mode);
    int rmdir(const char *pathname);
    int unlink(const char *pathname);
    int rename(const char *oldpath, const char *newpath);

    uint32_t get_pending_count() const {
        return pending_count_;
    }

    static int on_event(Reactor *reactor, Event *event);
    void process_completions();

  private:
    void detect_capabilities();
    int submit_and_yield(IoUringRequest *req);

    struct io_uring ring_;
    int event_fd_;
    network::Socket *event_socket_;
    IoUringCapabilities caps_;
    uint32_t pending_count_;
};

IoUringEngine *get_or_create_io_uring_engine();

}  // namespace openswoole

#endif
