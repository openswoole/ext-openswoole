#!/bin/sh
__CURRENT__=$(pwd)
__DIR__=$(cd "$(dirname "$0")";pwd)
__ROOT__=$(cd "${__DIR__}/..";pwd)

export PHP_VERSION=${PHP_VERSION:-${1:-"8.3"}}

echo "\n🧪 Running core-tests with PHP ${PHP_VERSION}\n"

# -----------Check Docker-----------
check_docker_dependency(){
    if [ "$(docker -v 2>&1 | grep "version")"x = ""x ]; then
        echo "\n❌ Docker not found!"
        exit 1
    elif [ "$(docker ps 2>&1 | grep Cannot)"x != ""x ]; then
        echo "\n❌ Docker is not running!"
        exit 1
    fi
}

check_docker_dependency

CONTAINER_NAME="openswoole-core-tests"

# -----------Cleanup old container-----------
docker rm -f "${CONTAINER_NAME}" > /dev/null 2>&1 || true

# -----------Start container-----------
echo "📦 Starting container..."
docker run -d \
    --name "${CONTAINER_NAME}" \
    --privileged \
    --security-opt seccomp:unconfined \
    -v "${__ROOT__}:/ext-openswoole:rw" \
    -w /ext-openswoole \
    "openswoole/php:${PHP_VERSION}" \
    tail -f /dev/null

if [ $? -ne 0 ]; then
    echo "\n❌ Failed to start container!"
    exit 1
fi

# -----------Run core-tests inside container-----------
echo "\n⏳ Building and running core-tests...\n"

docker exec "${CONTAINER_NAME}" sh -c '
set -e

# Install build dependencies
apt-get update > /dev/null 2>&1
apt-get install -y cmake libgtest-dev > /dev/null 2>&1
apt-get install -y liburing-dev > /dev/null 2>&1 || true

# Build GTest libraries if not already built
if [ ! -f /usr/lib/libgtest.a ] && [ ! -f /usr/local/lib/libgtest.a ]; then
    echo "📦 Building GTest..."
    cd /usr/src/gtest 2>/dev/null || cd /usr/src/googletest/googletest
    cmake . > /dev/null 2>&1
    make -j$(nproc) > /dev/null 2>&1
    cp lib/*.a /usr/lib/ 2>/dev/null || cp *.a /usr/lib/ 2>/dev/null || true
    cd /ext-openswoole
fi

# Compile openswoole extension
echo "📦 Compiling openswoole..."
cd /ext-openswoole
./clear.sh > /dev/null 2>&1 || true
phpize --clean > /dev/null 2>&1 || true
phpize > /dev/null 2>&1

CONFIGURE_OPTS="--enable-openssl --enable-http2 --enable-sockets --enable-mysqlnd --enable-hook-curl --enable-cares"

if ./configure $CONFIGURE_OPTS --enable-io-uring > /dev/null 2>&1; then
    echo "  io_uring enabled"
else
    ./configure $CONFIGURE_OPTS > /dev/null 2>&1
fi

make -j$(nproc) > /dev/null 2>&1
make install > /dev/null 2>&1

# Create lib symlink so core-tests can link against -lopenswoole
mkdir -p /ext-openswoole/lib
ln -sf /ext-openswoole/modules/openswoole.so /ext-openswoole/lib/libopenswoole.so

# Build core-tests
echo "📦 Building core-tests..."
cd /ext-openswoole/core-tests
rm -rf build
mkdir build && cd build
cmake .. -Dswoole_dir=/ext-openswoole > /dev/null 2>&1
make -j$(nproc) 2>&1

# Run core-tests
echo ""
echo "🚀 Running core-tests..."
echo ""
./bin/core_tests
'

RESULT=$?

# -----------Cleanup-----------
echo "\n🧹 Cleaning up..."
docker rm -f "${CONTAINER_NAME}" > /dev/null 2>&1 || true

if [ ${RESULT} -ne 0 ]; then
    echo "\n❌ Core-tests failed!"
    exit 1
fi

echo "\n🚀🚀🚀 Core-tests completed successfully 🚀🚀🚀\n"
