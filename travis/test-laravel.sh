#!/bin/sh -e

__CURRENT__=`pwd`
__DIR__=$(cd "$(dirname "$0")";pwd)

php -v
php --ri openswoole

export COMPOSER_ALLOW_SUPERUSER=1;

rm -rf cd ${__DIR__}/octane && \
cd ${__DIR__} && \
git clone https://github.com/laravel/octane.git --depth 1 && \
cd ${__DIR__}/octane && \
composer require laravel/framework:"^8.35" --no-update && \
composer update --prefer-dist --no-interaction --no-progress && \
vendor/bin/phpunit --verbose && \
rm -rf ${__DIR__}/octane
