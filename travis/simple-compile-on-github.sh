#!/bin/sh -e
# shellcheck disable=SC2034
# shellcheck disable=SC2006
__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)
__INI__=$(php -i | grep /.+/conf.d -oE | tail -1)

php --ini

if [ "${GITHUB_ACTIONS}" = true ]; then
  # shellcheck disable=SC2028
  echo "\n❌ This script is just for Github!"
  exit 255
fi

sudo apt-get install -y libc-ares-dev

cd "${__DIR__}" && cd ../ && \
./clear.sh > /dev/null && \
phpize --clean > /dev/null && \
phpize > /dev/null && \
./configure --enable-openssl --enable-sockets --enable-mysqlnd --enable-http2 --enable-hook-curl --enable-cares --with-postgres \
 > /dev/null && \
make -j8 > /dev/null | tee /tmp/compile.log && \
(test "`cat /tmp/compile.log`"x = ""x || exit 255) && \
make install

echo "\n[openswoole]\nextension=openswoole.so" >> "${__INI__}/zz_openswoole.ini" && \
php --ri curl && \
php --ri openswoole
