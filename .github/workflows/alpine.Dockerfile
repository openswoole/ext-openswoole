ARG PHP_VERSION
ARG ALPINE_VERSION

FROM php:${PHP_VERSION}-cli-alpine${ALPINE_VERSION}

LABEL maintainer="Open Swoole Group <hello@swoole.co.uk>" version="1.0" license="MIT"

ARG PHP_VERSION

COPY . /opt/www

WORKDIR /opt/www

ENV PHPIZE_DEPS="autoconf dpkg-dev dpkg file g++ gcc libc-dev make pkgconf re2c pcre-dev pcre2-dev zlib-dev libtool automake"

RUN set -ex \
    && apk update \
    && apk add --no-cache libstdc++ openssl git bash \
    && apk add --no-cache $PHPIZE_DEPS php${PHP_VERSION%\.*\.*}-dev php${PHP_VERSION%\.*\.*}-pear libaio-dev openssl-dev curl-dev \
    && apk add --no-cache ca-certificates \
    curl \
    wget \
    tar \
    xz \
    libressl \
    tzdata \
    pcre \
    php${PHP_VERSION%\.*\.*}-bcmath \
    php${PHP_VERSION%\.*\.*}-curl \
    php${PHP_VERSION%\.*\.*}-ctype \
    php${PHP_VERSION%\.*\.*}-dom \
    php${PHP_VERSION%\.*\.*}-gd \
    php${PHP_VERSION%\.*\.*}-iconv \
    php${PHP_VERSION%\.*\.*}-mbstring \
    php${PHP_VERSION%\.*\.*}-mysqlnd \
    php${PHP_VERSION%\.*\.*}-openssl \
    php${PHP_VERSION%\.*\.*}-pdo \
    php${PHP_VERSION%\.*\.*}-pdo_mysql \
    php${PHP_VERSION%\.*\.*}-pdo_sqlite \
    php${PHP_VERSION%\.*\.*}-phar \
    php${PHP_VERSION%\.*\.*}-posix \
    php${PHP_VERSION%\.*\.*}-redis \
    php${PHP_VERSION%\.*\.*}-sockets \
    php${PHP_VERSION%\.*\.*}-sodium \
    php${PHP_VERSION%\.*\.*}-sysvshm \
    php${PHP_VERSION%\.*\.*}-sysvmsg \
    php${PHP_VERSION%\.*\.*}-sysvsem \
    php${PHP_VERSION%\.*\.*}-zip \
    php${PHP_VERSION%\.*\.*}-zlib \
    php${PHP_VERSION%\.*\.*}-xml \
    php${PHP_VERSION%\.*\.*}-xmlreader \
    php${PHP_VERSION%\.*\.*}-pcntl \
    php${PHP_VERSION%\.*\.*}-opcache \
    && ln -sf /usr/bin/php8 /usr/local/bin/php \
    && ln -sf /usr/bin/phpize8 /usr/local/bin/phpize \
    && ln -sf /usr/bin/php-config8 /usr/local/bin/php-config \
    && php -v \
    && php -m \
    # ---------- clear works ----------
    && rm -rf /var/cache/apk/* /tmp/* /usr/share/man

RUN set -ex \
    && phpize \
    && ./configure --enable-openssl --enable-http2 --enable-swoole-curl --enable-swoole-json \
    && make -s -j$(nproc) && make install \
    && echo /etc/php${PHP_VERSION%\.*\.*}/conf.d/50_openswoole.ini \
    && echo "extension=openswoole.so" > /etc/php${PHP_VERSION%\.*\.*}/conf.d/50_openswoole.ini \
    && php -v \
    && php -m \
    && php --ri openswoole \
    && echo -e "\033[42;37m Build Completed :).\033[0m\n"
