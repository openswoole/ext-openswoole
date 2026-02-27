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

#include "test_core.h"
#include "openswoole_pipe.h"

#ifdef HAVE_IO_URING

using namespace openswoole;

static const char *test_pkt = "hello io_uring\r\n";

TEST(reactor_io_uring, create) {
    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    ASSERT_TRUE(reactor.running);
    ASSERT_GT(reactor.max_event_num, 0);
    ASSERT_NE(reactor.native_handle, -1);
    ASSERT_NE(reactor.write, nullptr);
    ASSERT_NE(reactor.close, nullptr);
}

TEST(reactor_io_uring, wait_read) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_EQ(sizeof("hello world"), n);
        EXPECT_STREQ("hello world", buffer);
        reactor->del(ev->socket);
        return OSW_OK;
    });

    ret = reactor.add(p.get_socket(false), OSW_EVENT_READ);
    ASSERT_EQ(ret, OSW_OK);

    ret = p.write((void *) OSW_STRS("hello world"));
    ASSERT_EQ(ret, sizeof("hello world"));

    ret = reactor.wait(nullptr);
    ASSERT_EQ(ret, 0);
}

TEST(reactor_io_uring, pipe_bidirectional) {
    Pipe p(true);
    ASSERT_TRUE(p.ready());

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *event) -> int {
        char buf[1024];
        size_t l = strlen(test_pkt);
        size_t n = read(event->fd, buf, sizeof(buf));
        EXPECT_EQ(n, l);
        buf[n] = 0;
        EXPECT_EQ(std::string(buf, n), std::string(test_pkt));
        reactor->del(event->socket);
        return OSW_OK;
    });

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_WRITE, [](Reactor *reactor, Event *event) -> int {
        size_t l = strlen(test_pkt);
        EXPECT_EQ(write(event->fd, test_pkt, l), l);
        reactor->del(event->socket);
        return OSW_OK;
    });

    reactor.add(p.get_socket(false), OSW_EVENT_READ);
    reactor.add(p.get_socket(true), OSW_EVENT_WRITE);
    reactor.wait(nullptr);
}

static bool set_events_write_fired = false;
static bool set_events_read_fired = false;
static UnixSocket *set_events_pipe = nullptr;

TEST(reactor_io_uring, set_events) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());
    set_events_pipe = &p;
    set_events_write_fired = false;
    set_events_read_fired = false;

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_GT(n, 0);
        set_events_read_fired = true;
        reactor->del(ev->socket);
        return OSW_OK;
    });

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_WRITE, [](Reactor *reactor, Event *ev) -> int {
        set_events_write_fired = true;
        // Write data so the read side fires, then switch to read-only
        set_events_pipe->write((void *) OSW_STRS("test"));
        reactor->set(ev->socket, OSW_EVENT_READ);
        return OSW_OK;
    });

    // Start with write interest only on the write end
    auto *write_socket = p.get_socket(true);
    ret = reactor.add(write_socket, OSW_EVENT_WRITE);
    ASSERT_EQ(ret, OSW_OK);

    // Add read interest on the read end
    ret = reactor.add(p.get_socket(false), OSW_EVENT_READ);
    ASSERT_EQ(ret, OSW_OK);

    ret = reactor.wait(nullptr);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(set_events_write_fired);
    ASSERT_TRUE(set_events_read_fired);
}

TEST(reactor_io_uring, del_socket) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        // Should not fire after del
        ADD_FAILURE() << "read handler should not fire after del";
        return OSW_OK;
    });

    ret = reactor.add(p.get_socket(false), OSW_EVENT_READ);
    ASSERT_EQ(ret, OSW_OK);
    ASSERT_EQ(reactor.get_event_num(), 1);

    ret = reactor.del(p.get_socket(false));
    ASSERT_EQ(ret, OSW_OK);
    ASSERT_EQ(reactor.get_event_num(), 0);
}

static int multiple_sockets_received = 0;

TEST(reactor_io_uring, multiple_sockets) {
    const int NUM_PAIRS = 4;
    UnixSocket *pairs[NUM_PAIRS];
    multiple_sockets_received = 0;

    for (int i = 0; i < NUM_PAIRS; i++) {
        pairs[i] = new UnixSocket(true, SOCK_DGRAM);
        ASSERT_TRUE(pairs[i]->ready());
    }

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_GT(n, 0);
        multiple_sockets_received++;
        reactor->del(ev->socket);
        return OSW_OK;
    });

    for (int i = 0; i < NUM_PAIRS; i++) {
        int ret = reactor.add(pairs[i]->get_socket(false), OSW_EVENT_READ);
        ASSERT_EQ(ret, OSW_OK);
    }

    ASSERT_EQ(reactor.get_event_num(), NUM_PAIRS);

    // Write to all pairs
    for (int i = 0; i < NUM_PAIRS; i++) {
        char msg[32];
        snprintf(msg, sizeof(msg), "msg-%d", i);
        pairs[i]->write(msg, strlen(msg) + 1);
    }

    reactor.wait(nullptr);
    ASSERT_EQ(multiple_sockets_received, NUM_PAIRS);

    for (int i = 0; i < NUM_PAIRS; i++) {
        delete pairs[i];
    }
}

TEST(reactor_io_uring, timeout) {
    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.once = true;
    ASSERT_TRUE(reactor.running);

    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    reactor.set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        ADD_FAILURE() << "should not fire on timeout";
        return OSW_OK;
    });

    reactor.add(p.get_socket(false), OSW_EVENT_READ);

    // Wait with 50ms timeout, no data written - should timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;  // 50ms
    reactor.wait(&tv);
}

TEST(reactor_io_uring, openswoole_event_api) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    ret = openswoole_event_init(OSW_EVENTLOOP_WAIT_EXIT);
    ASSERT_EQ(ret, OSW_OK);
    ASSERT_NE(SwooleTG.reactor, nullptr);

    openswoole_event_set_handler(OSW_FD_PIPE | OSW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_EQ(sizeof("io_uring test"), n);
        EXPECT_STREQ("io_uring test", buffer);
        reactor->del(ev->socket);
        return OSW_OK;
    });

    ret = openswoole_event_add(p.get_socket(false), OSW_EVENT_READ);
    ASSERT_EQ(ret, OSW_OK);

    ret = p.write((void *) OSW_STRS("io_uring test"));
    ASSERT_EQ(ret, sizeof("io_uring test"));

    ret = openswoole_event_wait();
    ASSERT_EQ(ret, OSW_OK);
    ASSERT_EQ(SwooleTG.reactor, nullptr);
}

#endif
