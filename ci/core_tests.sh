#!/bin/sh
# Run the core C++ tests in Docker the same way .github/workflows/core.yml does.
# Usage: ci/core_tests.sh [php version] [gtest filter]
__DIR__=$(cd "$(dirname "$0")";pwd)
__ROOT__=$(cd "${__DIR__}/..";pwd)

PHP_VERSION=${PHP_VERSION:-${1:-"8.6"}}
GTEST_FILTER=${2:-"*"}
NAME="openswoole-core-tests"

echo "\n🧪 Running core-tests with PHP ${PHP_VERSION}\n"

if ! docker ps > /dev/null 2>&1; then
    echo "\n❌ Docker is not running!"
    exit 1
fi

cleanup() {
    docker rm -f "${NAME}" "${NAME}-tinyproxy" "${NAME}-socks5" > /dev/null 2>&1 || true
}
cleanup
trap cleanup EXIT

docker run -d --name "${NAME}" --privileged --security-opt seccomp:unconfined \
    -v "${__ROOT__}:/ext-openswoole:ro" "openswoole/php:${PHP_VERSION}" tail -f /dev/null > /dev/null || exit 1
# the proxies listen on the test container's localhost, like the services in core.yml
docker run -d --name "${NAME}-tinyproxy" --network "container:${NAME}" vimagick/tinyproxy > /dev/null || exit 1
docker run -d --name "${NAME}-socks5" --network "container:${NAME}" xkuma/socks5 > /dev/null || exit 1

docker exec -e GITHUB_ACTIONS=true -e GTEST_FILTER="${GTEST_FILTER}" "${NAME}" sh -c '
set -e
apt-get update > /dev/null
apt-get install -y cmake googletest libgtest-dev redis-server libboost-stacktrace-dev libbrotli-dev > /dev/null
redis-server --daemonize yes > /dev/null

# build from a copy, so the mounted source tree is not changed
mkdir -p /build
cd /ext-openswoole && tar --exclude=./.git -cf - . | tar -xf - -C /build
cd /build
phpize > /dev/null
./configure --enable-sockets --enable-mysqlnd --enable-http2 --enable-openssl > /dev/null
cmake . > /dev/null
make -j"$(nproc)" > /dev/null
make install > /dev/null
ldconfig

cd core-tests
./run.sh
'
RESULT=$?

if [ ${RESULT} -ne 0 ]; then
    echo "\n❌ Core-tests failed!"
    exit 1
fi
echo "\n🚀🚀🚀 Core-tests completed successfully 🚀🚀🚀\n"
