#!/bin/sh -e
__CURRENT__=$(pwd)
__DIR__=$(cd "$(dirname "$0")";pwd)

if [ ! -f "/.dockerenv" ]; then
    echo "" && echo "❌ This script is just for Docker env!"
    exit
fi

#-----------install dependencies------------
export DEBIAN_FRONTEND=noninteractive
apt-get update > /dev/null && apt-get install -y \
    git unzip procps \
    libssl-dev libcurl4-openssl-dev libpq-dev libc-ares-dev libnghttp2-dev \
    libcurl4 libpq5 libc-ares2 libnghttp2-14 \
    > /dev/null 2>&1
docker-php-ext-install curl pcntl sockets mysqli pdo pdo_mysql pdo_pgsql > /dev/null 2>&1 || true
curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

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
make install > /dev/null 2>&1 && echo "" && \
docker-php-ext-enable --ini-name zzz-docker-php-ext-openswoole.ini openswoole > /dev/null 2>&1 && \
echo "zend.max_allowed_stack_size=-1" > /usr/local/etc/php/conf.d/zzz-stack-size.ini && \
php --ri curl && \
php --ri openswoole

