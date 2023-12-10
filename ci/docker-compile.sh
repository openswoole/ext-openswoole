#!/bin/sh -e
__CURRENT__=$(pwd)
__DIR__=$(cd "$(dirname "$0")";pwd)

if [ ! -f "/.dockerenv" ]; then
    echo "" && echo "❌ This script is just for Docker env!"
    exit
fi

#-----------compile------------
#-------print error only-------
cd "${__DIR__}" && cd ../ && \
./clear.sh > /dev/null && \
phpize --clean > /dev/null && \
phpize > /dev/null && \
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
make install && echo "" && \
docker-php-ext-enable --ini-name zzz-docker-php-ext-openswoole.ini openswoole && \
echo "zend.max_allowed_stack_size=-1" > /usr/local/etc/php/conf.d/zzz-stack-size.ini && \
php --ri curl && \
php --ri openswoole

