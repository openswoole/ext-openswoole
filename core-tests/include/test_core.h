#pragma once

#include "swoole_api.h"
#include "swoole_client.h"

#include <gtest/gtest.h>

#include <functional>
#include <utility>
#include <initializer_list>
#include <string>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <fstream>


#define TEST_HOST "127.0.0.1"
#define TEST_PORT 9501
#define TEST_TMP_FILE "/tmp/swoole_core_test_file"
#define TEST_TMP_DIR  "/tmp/swoole_core_test_dir"
#define TEST_JPG_FILE "/tests/assets/test.jpg"
#define TEST_JPG_MD5SUM  "753f6d910f0ae85b5eb9a366dea74927"
#define TEST_HTTP_PROXY_PORT 8888
#define TEST_HTTP_PROXY_HOST "127.0.0.1"

#define ASSERT_MEMEQ(x,y,n)   ASSERT_EQ(memcmp((x), (y), n), 0)
#define EXPECT_MEMEQ(x,y,n)   EXPECT_EQ(memcmp((x), (y), n), 0)

namespace swoole { namespace test {
const std::string &get_root_path();
std::string get_jpg_file();
bool is_github_ci();
}};
