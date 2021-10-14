#!/bin/sh -e
__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)

cd ${__DIR__} && cd ../ && \
pecl config-show && \
php ./tools/pecl-package.php && package_file="`ls | grep openswoole-*tgz`" && \
echo "\n" | pecl install -f ${package_file} | tee pecl.log && \
cat pecl.log | grep "successfully" && \
pecl uninstall openswoole && \
rm -f pecl.log
