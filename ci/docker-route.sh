#!/bin/sh -e
__CURRENT__=$(pwd)
__DIR__=$(cd "$(dirname "$0")";pwd)

# enter the dir
cd "${__DIR__}"

# map SWOOLE_BRANCH (from docker-compose) to CI_BRANCH (used by run-tests.sh)
[ -n "${SWOOLE_BRANCH}" ] && export CI_BRANCH="${SWOOLE_BRANCH}"

# show system info
date && echo ""
uname -a && echo ""

# show php info
php -v && echo ""

# compile in docker
echo "" && echo "📦 Compile test in docker..." && echo ""
./docker-compile.sh

# run unit tests
echo "" && echo "📋 PHP unit tests in docker..." && echo ""
./run-tests.sh
