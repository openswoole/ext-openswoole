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
#include "swoole_pipe.h"

#ifdef HAVE_IO_URING

using namespace swoole;

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

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_EQ(sizeof("hello world"), n);
        EXPECT_STREQ("hello world", buffer);
        reactor->del(ev->socket);
        return SW_OK;
    });

    ret = reactor.add(p.get_socket(false), SW_EVENT_READ);
    ASSERT_EQ(ret, SW_OK);

    ret = p.write((void *) SW_STRS("hello world"));
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

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [](Reactor *reactor, Event *event) -> int {
        char buf[1024];
        size_t l = strlen(test_pkt);
        size_t n = read(event->fd, buf, sizeof(buf));
        EXPECT_EQ(n, l);
        buf[n] = 0;
        EXPECT_EQ(std::string(buf, n), std::string(test_pkt));
        reactor->del(event->socket);
        return SW_OK;
    });

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_WRITE, [](Reactor *reactor, Event *event) -> int {
        size_t l = strlen(test_pkt);
        EXPECT_EQ(write(event->fd, test_pkt, l), l);
        reactor->del(event->socket);
        return SW_OK;
    });

    reactor.add(p.get_socket(false), SW_EVENT_READ);
    reactor.add(p.get_socket(true), SW_EVENT_WRITE);
    reactor.wait(nullptr);
}

TEST(reactor_io_uring, set_events) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    bool write_fired = false;
    bool read_fired = false;

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [&read_fired](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_GT(n, 0);
        read_fired = true;
        reactor->del(ev->socket);
        return SW_OK;
    });

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_WRITE, [&write_fired, &p](Reactor *reactor, Event *ev) -> int {
        write_fired = true;
        // Write data so the read side fires, then switch to read-only
        p.write((void *) SW_STRS("test"));
        reactor->set(ev->socket, SW_EVENT_READ);
        return SW_OK;
    });

    // Start with write interest only on the write end
    auto *write_socket = p.get_socket(true);
    ret = reactor.add(write_socket, SW_EVENT_WRITE);
    ASSERT_EQ(ret, SW_OK);

    // Add read interest on the read end
    ret = reactor.add(p.get_socket(false), SW_EVENT_READ);
    ASSERT_EQ(ret, SW_OK);

    ret = reactor.wait(nullptr);
    ASSERT_EQ(ret, 0);
    ASSERT_TRUE(write_fired);
    ASSERT_TRUE(read_fired);
}

TEST(reactor_io_uring, del_socket) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        // Should not fire after del
        ADD_FAILURE() << "read handler should not fire after del";
        return SW_OK;
    });

    ret = reactor.add(p.get_socket(false), SW_EVENT_READ);
    ASSERT_EQ(ret, SW_OK);
    ASSERT_EQ(reactor.get_event_num(), 1);

    ret = reactor.del(p.get_socket(false));
    ASSERT_EQ(ret, SW_OK);
    ASSERT_EQ(reactor.get_event_num(), 0);
}

TEST(reactor_io_uring, multiple_sockets) {
    const int NUM_PAIRS = 4;
    UnixSocket *pairs[NUM_PAIRS];
    int received = 0;

    for (int i = 0; i < NUM_PAIRS; i++) {
        pairs[i] = new UnixSocket(true, SOCK_DGRAM);
        ASSERT_TRUE(pairs[i]->ready());
    }

    Reactor reactor(1024, Reactor::TYPE_IO_URING);
    reactor.wait_exit = true;
    ASSERT_TRUE(reactor.running);

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [&received](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_GT(n, 0);
        received++;
        reactor->del(ev->socket);
        return SW_OK;
    });

    for (int i = 0; i < NUM_PAIRS; i++) {
        int ret = reactor.add(pairs[i]->get_socket(false), SW_EVENT_READ);
        ASSERT_EQ(ret, SW_OK);
    }

    ASSERT_EQ(reactor.get_event_num(), NUM_PAIRS);

    // Write to all pairs
    for (int i = 0; i < NUM_PAIRS; i++) {
        char msg[32];
        snprintf(msg, sizeof(msg), "msg-%d", i);
        pairs[i]->write(msg, strlen(msg) + 1);
    }

    reactor.wait(nullptr);
    ASSERT_EQ(received, NUM_PAIRS);

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

    reactor.set_handler(SW_FD_PIPE | SW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        ADD_FAILURE() << "should not fire on timeout";
        return SW_OK;
    });

    reactor.add(p.get_socket(false), SW_EVENT_READ);

    // Wait with 50ms timeout, no data written - should timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;  // 50ms
    reactor.wait(&tv);
}

TEST(reactor_io_uring, swoole_event_api) {
    int ret;
    UnixSocket p(true, SOCK_DGRAM);
    ASSERT_TRUE(p.ready());

    ret = swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);
    ASSERT_EQ(ret, SW_OK);
    ASSERT_NE(SwooleTG.reactor, nullptr);

    swoole_event_set_handler(SW_FD_PIPE | SW_EVENT_READ, [](Reactor *reactor, Event *ev) -> int {
        char buffer[64];
        ssize_t n = read(ev->fd, buffer, sizeof(buffer));
        EXPECT_EQ(sizeof("io_uring test"), n);
        EXPECT_STREQ("io_uring test", buffer);
        reactor->del(ev->socket);
        return SW_OK;
    });

    ret = swoole_event_add(p.get_socket(false), SW_EVENT_READ);
    ASSERT_EQ(ret, SW_OK);

    ret = p.write((void *) SW_STRS("io_uring test"));
    ASSERT_EQ(ret, sizeof("io_uring test"));

    ret = swoole_event_wait();
    ASSERT_EQ(ret, SW_OK);
    ASSERT_EQ(SwooleTG.reactor, nullptr);
}

#endif
