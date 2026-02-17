#!/bin/sh -e
__CURRENT__=$(pwd)
__DIR__=$(cd "$(dirname "$0")";pwd)

if [ ! -f "/.dockerenv" ]; then
    echo "" && echo "❌ This script is just for Docker env!"
    exit
fi

#-----------install gdb for coredump backtraces------------
apt-get update > /dev/null 2>&1 && apt-get install -y gdb > /dev/null 2>&1 || true
ulimit -c unlimited
echo "/tmp/core.%e.%p" > /proc/sys/kernel/core_pattern 2>/dev/null || true

#-----------compile------------
#-------print error only-------
export TERM=${TERM:-dumb}

cd "${__DIR__}" && cd ../ && \
./clear.sh > /dev/null && \
phpize --clean > /dev/null && \
phpize > /dev/null 2>&1 && \
./configure \
--enable-openssl \
--enable-http2 \
--enable-sockets \
--enable-mysqlnd \
--enable-hook-curl \
--enable-cares \
--with-postgres \
> /dev/null && \
make -j8 > /dev/null | tee /tmp/compile.log && \
(test "`cat /tmp/compile.log`"x = ""x || exit 255) && \
make install > /dev/null 2>&1 && echo "" && \
docker-php-ext-enable --ini-name zzz-docker-php-ext-openswoole.ini openswoole > /dev/null 2>&1 && \
echo "zend.max_allowed_stack_size=-1" > /usr/local/etc/php/conf.d/zzz-stack-size.ini && \
php --ri curl && \
php --ri openswoole
