#include "test_coroutine.h"

#include <chrono>
#include <netdb.h>

using swoole::Coroutine;
using swoole::Timer;
using swoole::coroutine::System;
using swoole::test::coroutine;

TEST(coroutine_gethostbyname, resolve_cache) {
    auto now_us = []() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
    };
    coroutine::run([&](void *arg) {
        System::set_dns_cache_capacity(10);
        std::string addr1 = System::gethostbyname("localhost", AF_INET);
        ASSERT_NE(addr1, "");
        int64_t start = now_us();
        for (int i = 0; i < 100; ++i) {
            std::string addr2 = System::gethostbyname("localhost", AF_INET);
            ASSERT_EQ(addr1, addr2);
        }
        double with_cache = (now_us() - start) / 100.0;

        System::set_dns_cache_capacity(0);
        start = now_us();
        for (int i = 0; i < 20; ++i) {
            std::string addr2 = System::gethostbyname("localhost", AF_INET);
            ASSERT_NE(addr2, "");
        }
        double without_cache = (now_us() - start) / 20.0;

        // per lookup: a cache hit does not go through the thread pool
        ASSERT_GT(without_cache, with_cache);
    });
}

TEST(coroutine_gethostbyname, impl_async) {
    coroutine::run([](void *arg) {
        auto result = swoole::coroutine::gethostbyname_impl_with_async("localhost", AF_INET);
        ASSERT_EQ(result.empty(), false);
    });
}

TEST(coroutine_gethostbyname, resolve_cache_inet4_and_inet6) {
    // needs "::1 localhost" in /etc/hosts
    struct addrinfo hints = {}, *res = nullptr;
    hints.ai_family = AF_INET6;
    if (getaddrinfo("localhost", nullptr, &hints, &res) != 0) {
        GTEST_SKIP() << "localhost has no IPv6 address";
    }
    freeaddrinfo(res);
    coroutine::run([](void *arg) {
        System::set_dns_cache_capacity(10);

        std::string addr1 = System::gethostbyname("localhost", AF_INET);
        std::string addr2 = System::gethostbyname("localhost", AF_INET6);

        ASSERT_NE(addr1, "");
        ASSERT_NE(addr2, "");
        ASSERT_EQ(addr1.find(":"), addr1.npos);
        ASSERT_NE(addr2.find(":"), addr2.npos);

        int64_t start = Timer::get_absolute_msec();

        for (int i = 0; i < 100; ++i) {
            std::string addr3 = System::gethostbyname("localhost", AF_INET);
            std::string addr4 = System::gethostbyname("localhost", AF_INET6);

            ASSERT_EQ(addr1, addr3);
            ASSERT_EQ(addr2, addr4);
        }

        ASSERT_LT(Timer::get_absolute_msec() - start, 5);
    });
}
