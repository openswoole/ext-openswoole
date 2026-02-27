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
  | @author   Tianfeng Han  <mikan.tenny@gmail.com>                      |
  +----------------------------------------------------------------------+
*/

#include "test_coroutine.h"

using namespace openswoole;
using namespace openswoole::test;

using swoole::coroutine::Socket;
using swoole::coroutine::System;

static const char *test_file = "/tmp/swoole-core-test";

static constexpr int DATA_SIZE = 8 * 1024 * 1024;
static constexpr int DATA_SIZE_2 = 64 * 1024;

TEST(coroutine_system, file) {
    test::coroutine::run([](void *arg) {
        std::shared_ptr<String> buf = std::make_shared<String>(DATA_SIZE);
        ASSERT_EQ(openswoole_random_bytes(buf->str, buf->size - 1), buf->size - 1);
        buf->str[buf->size - 1] = 0;
        ASSERT_EQ(System::write_file(test_file, buf->str, buf->size, true, 0), buf->size);
        auto data = System::read_file(test_file, true);
        ASSERT_TRUE(data.get());
        ASSERT_STREQ(buf->str, data->str);
        unlink(test_file);
    });
}

TEST(coroutine_system, flock) {
    std::shared_ptr<String> buf = std::make_shared<String>(65536);
    ASSERT_EQ(openswoole_random_bytes(buf->str, buf->size - 1), buf->size - 1);
    buf->str[buf->size - 1] = 0;

    openswoole_event_init(OSW_EVENTLOOP_WAIT_EXIT);

    Coroutine::create([&buf](void *) {
        int fd = openswoole_coroutine_open(test_file, File::WRITE | File::CREATE, 0666);
        ASSERT_TRUE(fd > 0);
        openswoole_coroutine_flock_ex(test_file, fd, LOCK_EX);

        for (int i = 0; i < 4; i++) {
            Coroutine::create([&buf](void *) {
                int fd = openswoole_coroutine_open(test_file, File::READ, 0);
                ASSERT_TRUE(fd > 0);
                openswoole_coroutine_flock_ex(test_file, fd, LOCK_SH);
                String read_buf(DATA_SIZE_2);
                auto rn = openswoole_coroutine_read(fd, read_buf.str, read_buf.size - 1);
                ASSERT_EQ(rn, read_buf.size - 1);
                read_buf.str[read_buf.size - 1] = 0;
                openswoole_coroutine_flock_ex(test_file, fd, LOCK_UN);
                EXPECT_STREQ(read_buf.str, buf->str);
                openswoole_coroutine_close(fd);
            });
        }

        auto wn = openswoole_coroutine_write(fd, buf->str, buf->size - 1);
        ASSERT_EQ(wn, buf->size - 1);
        openswoole_coroutine_flock_ex(test_file, fd, LOCK_UN);
        openswoole_coroutine_close(fd);
    });

    openswoole_event_wait();
    unlink(test_file);
}

TEST(coroutine_system, cancel_sleep) {
    test::coroutine::run([](void *arg) {
        auto co = Coroutine::get_current_safe();
        Coroutine::create([co](void *) {
            System::sleep(0.002);
            co->cancel();
        });
        System::sleep(1000);
    });
}
