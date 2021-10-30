<h1>
<img width="200" height="120" align=center alt="Swoole Logo" src="https://www.swoole.co.uk/images/swoole-logo.svg" />
</h1>

[![lib-openswoole](https://github.com/openswoole/swoole-src/workflows/lib-openswoole/badge.svg)](https://github.com/openswoole/swoole-src/actions?query=workflow%3Alib-openswoole)
[![ext-openswoole](https://github.com/openswoole/swoole-src/workflows/ext-openswoole/badge.svg)](https://github.com/openswoole/swoole-src/actions?query=workflow%3Aext-openswoole)
[![test-linux](https://github.com/openswoole/swoole-src/workflows/test-linux/badge.svg)](https://github.com/openswoole/swoole-src/actions?query=workflow%3Atest-linux)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/23970/badge.svg)](https://scan.coverity.com/projects/open-swoole-src)
[![codecov](https://codecov.io/gh/openswoole/swoole-src/branch/master/graph/badge.svg)](https://codecov.io/gh/openswoole/swoole-src)

**Open Swoole is an event-driven asynchronous & coroutine-based concurrency networking communication engine with high performance written in C++ for PHP.**

## Documentation & Community

+ __Documentation__: <https://www.swoole.co.uk/docs>
+ __Twitter__: <https://twitter.com/openswoole>
+ __Slack Group__: <https://swoole.slack.com>
+ __IDE Helper & API__: <https://github.com/openswoole/ide-helper>

## Frameworks & Components

> PR are welcome if your framework is using openswoole

 - [**PHP Runtime**](https://github.com/php-runtime) make it easy to run any kind of PHP Application (Symfony, Laravel, PSR7, Native) with all kind of Runtimes like OpenSwoole, Bref, Google Cloud Functions, Roadrunner and React PHP with minimal configuration.

## Installation

> As with any open source project, Open Swoole always provides the most reliable stability and the most powerful features in **the latest released version**. Please ensure as much as possible that you are using the latest version.

### Compiling requirements

+ Linux, OS X or Cygwin, WSL
+ PHP 7.2.0 or later (The higher the version, the better the performance.)
+ GCC 4.8 or later

### 1. Install via PECL (beginners)

```shell
pecl install openswoole
```

### 2. Install from source (recommended)

Please download the source packages from [Releases](https://github.com/openswoole/swoole-src/releases) or:

```shell
git clone https://github.com/openswoole/swoole-src.git && \
cd swoole-src
git checkout v4.7.2
```

Compile and install at the source folder:

```shell
phpize && \
./configure && \
make && make install
```

#### Enable extension in PHP

After compiling and installing to the system successfully, you have to add a new line `extension=openswoole.so` to `php.ini` to enable Open Swoole extension. It is recommended to be added after all the other extensions because openswoole may depend on extensions: sockets, mysqlnd, curl etc.

#### Extra compiler configurations

> for example: `./configure --enable-openssl --enable-sockets`

+ `--enable-openssl` or `--with-openssl-dir=DIR`
+ `--enable-sockets`
+ `--enable-http2`
+ `--enable-mysqlnd` (need mysqlnd, it just for supporting `$mysql->escape` method)
+ `--enable-swoole-json`
+ `--enable-swoole-curl`

### Upgrade

>  If you upgrade from source, don't forget to `make clean` before you upgrade your swoole

1. `pecl upgrade openswoole`
2. `cd swoole-src && git pull && make clean && make && sudo make install`
3. if you change your PHP version, please re-run `phpize clean && phpize` then try to compile

## Contribution

Your contributions to Open Swoole development are welcome.

* [Report issues and feedback](https://github.com/openswoole/swoole-src/issues)
* Submit fixes, features via Pull Request

## Contributors

This project exists thanks to all the contributors who contribute to the orignal Swoole project. [[Contributors](https://github.com/openswoole/swoole-src/graphs/contributors)].

## Security issues

Security issues should be reported privately, via email, to the Open Swoole develop team [hello@swoole.co.uk](mailto:hello@swoole.co.uk). You should receive a response within 24 hours. If for some reason you do not, please follow up via email to ensure we received your original message.

## License

Apache License Version 2.0 see http://www.apache.org/licenses/LICENSE-2.0.html
