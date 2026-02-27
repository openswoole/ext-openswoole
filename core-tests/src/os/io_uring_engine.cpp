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

#ifdef HAVE_IO_URING

#include "openswoole_io_uring.h"
#include "openswoole_coroutine.h"

using namespace swoole;

static const char *TEST_IO_URING_FILE = "/tmp/swoole_io_uring_test";
static const char *TEST_IO_URING_FILE2 = "/tmp/swoole_io_uring_test2";
static const char *TEST_IO_URING_DIR = "/tmp/swoole_io_uring_test_dir";

TEST(io_uring_engine, create) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);
        EXPECT_EQ(engine->get_pending_count(), 0u);
    });

    swoole_event_wait();
}

TEST(io_uring_engine, open_read_write) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);

        // Open file for writing
        int fd = engine->open(TEST_IO_URING_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -2) {
            // io_uring open not supported on this kernel, skip
            return;
        }
        ASSERT_GE(fd, 0);

        // Write data
        const char *data = "hello io_uring engine";
        ssize_t n = engine->write(fd, data, strlen(data));
        EXPECT_EQ(n, (ssize_t) strlen(data));
        ::close(fd);

        // Open file for reading
        fd = engine->open(TEST_IO_URING_FILE, O_RDONLY, 0);
        ASSERT_GE(fd, 0);

        // Read data back
        char buf[64] = {};
        n = engine->read(fd, buf, sizeof(buf));
        EXPECT_EQ(n, (ssize_t) strlen(data));
        EXPECT_STREQ(buf, data);
        ::close(fd);

        // Clean up
        ::unlink(TEST_IO_URING_FILE);
    });

    swoole_event_wait();
}

TEST(io_uring_engine, fstat_test) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);

        // Create a test file
        int fd = engine->open(TEST_IO_URING_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -2) return;
        ASSERT_GE(fd, 0);

        const char *data = "test data for fstat";
        engine->write(fd, data, strlen(data));
        ::close(fd);

        // Open and fstat
        fd = engine->open(TEST_IO_URING_FILE, O_RDONLY, 0);
        ASSERT_GE(fd, 0);

        struct stat st = {};
        int ret = engine->fstat(fd, &st);
        if (ret != -2) {
            EXPECT_EQ(ret, 0);
            EXPECT_EQ((size_t) st.st_size, strlen(data));
            EXPECT_TRUE(S_ISREG(st.st_mode));
        }

        ::close(fd);
        ::unlink(TEST_IO_URING_FILE);
    });

    swoole_event_wait();
}

TEST(io_uring_engine, mkdir_rmdir) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);

        // Ensure dir does not exist
        ::rmdir(TEST_IO_URING_DIR);

        int ret = engine->mkdir(TEST_IO_URING_DIR, 0755);
        if (ret == -2) return;  // unsupported
        EXPECT_EQ(ret, 0);

        // Verify it exists
        struct stat st = {};
        EXPECT_EQ(::stat(TEST_IO_URING_DIR, &st), 0);
        EXPECT_TRUE(S_ISDIR(st.st_mode));

        // Remove it
        ret = engine->rmdir(TEST_IO_URING_DIR);
        if (ret != -2) {
            EXPECT_EQ(ret, 0);
            EXPECT_NE(::stat(TEST_IO_URING_DIR, &st), 0);
        } else {
            ::rmdir(TEST_IO_URING_DIR);
        }
    });

    swoole_event_wait();
}

TEST(io_uring_engine, unlink_test) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);

        // Create a file
        int fd = engine->open(TEST_IO_URING_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -2) return;
        ASSERT_GE(fd, 0);
        ::close(fd);

        // Unlink it
        int ret = engine->unlink(TEST_IO_URING_FILE);
        if (ret == -2) {
            ::unlink(TEST_IO_URING_FILE);
            return;
        }
        EXPECT_EQ(ret, 0);

        // Verify it's gone
        struct stat st = {};
        EXPECT_NE(::stat(TEST_IO_URING_FILE, &st), 0);
    });

    swoole_event_wait();
}

TEST(io_uring_engine, rename_test) {
    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([](void *) {
        auto *engine = get_or_create_io_uring_engine();
        ASSERT_NE(engine, nullptr);

        // Create a file
        int fd = engine->open(TEST_IO_URING_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -2) return;
        ASSERT_GE(fd, 0);
        engine->write(fd, "rename test", 11);
        ::close(fd);

        // Remove target if it exists
        ::unlink(TEST_IO_URING_FILE2);

        // Rename it
        int ret = engine->rename(TEST_IO_URING_FILE, TEST_IO_URING_FILE2);
        if (ret == -2) {
            ::unlink(TEST_IO_URING_FILE);
            return;
        }
        EXPECT_EQ(ret, 0);

        // Verify old is gone and new exists
        struct stat st = {};
        EXPECT_NE(::stat(TEST_IO_URING_FILE, &st), 0);
        EXPECT_EQ(::stat(TEST_IO_URING_FILE2, &st), 0);

        ::unlink(TEST_IO_URING_FILE2);
    });

    swoole_event_wait();
}

#endif
