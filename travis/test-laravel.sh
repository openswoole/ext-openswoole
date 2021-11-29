#!/bin/sh -e

__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)

php -v
php --ri openswoole

cd ${__DIR__} && \
git clone https://github.com/laravel/octane.git && \
cd ${__DIR__}/octane && \
composer require laravel/framework:"^8.35" --no-update && \
composer update --prefer-dist --no-interaction --no-progress && \
vendor/bin/phpunit --verbose && \
rm -rf cd ${__DIR__}/octane