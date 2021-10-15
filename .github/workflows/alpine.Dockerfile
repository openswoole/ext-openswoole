ARG PHP_VERSION
ARG ALPINE_VERSION

FROM hyperf/hyperf:${PHP_VERSION}-alpine-v${ALPINE_VERSION}-dev

LABEL maintainer="Open Swoole Group <hello@swoole.co.uk>" version="1.0" license="MIT"

ARG PHP_VERSION

COPY . /opt/www

WORKDIR /opt/www

RUN set -ex \
    && phpize \
    && ./configure --enable-openssl --enable-http2 --enable-swoole-curl --enable-swoole-json \
    && make -s -j$(nproc) && make install \
    && echo "extension=openswoole.so" > /etc/php${PHP_VERSION%\.*}/conf.d/50_openswoole.ini \
    # check
    && php -v \
    && php -m \
    && php --ri openswoole \
    && echo -e "\033[42;37m Build Completed :).\033[0m\n"
