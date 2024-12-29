ARG PHP_VERSION
ARG ALPINE_VERSION

FROM php:${PHP_VERSION}-cli-alpine${ALPINE_VERSION}

LABEL maintainer="Open Swoole Group <hello@openswoole.com>" version="1.0" license="MIT"

ARG PHP_VERSION

COPY . /opt/www

WORKDIR /opt/www

ENV PHPIZE_DEPS="autoconf dpkg-dev dpkg file g++ gcc libc-dev make pkgconf re2c pcre-dev pcre2-dev zlib-dev libtool automake"

RUN set -ex \
    && apk update \
    && apk add --no-cache libstdc++ openssl git bash \
    && apk add --no-cache $PHPIZE_DEPS php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-dev php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-pear libaio-dev openssl-dev curl-dev \
    && apk add --no-cache ca-certificates \
    curl \
    wget \
    tar \
    xz \
    libressl \
    tzdata \
    pcre \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-bcmath \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-curl \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-ctype \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-dom \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-gd \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-iconv \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-mbstring \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-mysqlnd \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-openssl \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-pdo \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-pdo_mysql \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-pdo_sqlite \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-phar \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-posix \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-redis \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-sockets \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-sodium \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-sysvshm \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-sysvmsg \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-sysvsem \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-zip \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-zlib \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-xml \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-xmlreader \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-pcntl \
    php${PHP_VERSION:0:1}${PHP_VERSION:2:1}-opcache \
    && ln -sf /usr/bin/php8 /usr/local/bin/php \
    && ln -sf /usr/bin/phpize8 /usr/local/bin/phpize \
    && ln -sf /usr/bin/php-config8 /usr/local/bin/php-config \
    && php -v \
    && php -m \
    # ---------- clear works ----------
    && rm -rf /var/cache/apk/* /tmp/* /usr/share/man

RUN set -ex \
    && phpize \
    && ./configure --enable-openssl --enable-http2 --enable-hook-curl \
    && make -s -j$(nproc) && make install \
    && echo /etc/php${PHP_VERSION:0:1}${PHP_VERSION:2:1}/conf.d/50_openswoole.ini \
    && echo "extension=openswoole.so" > /etc/php${PHP_VERSION:0:1}${PHP_VERSION:2:1}/conf.d/50_openswoole.ini \
    && php -v \
    && php -m \
    && php --ri openswoole \
    && echo -e "\033[42;37m Build Completed :).\033[0m\n"
