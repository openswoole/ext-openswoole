<p align="center"><a href="https://openswoole.com" target="_blank"><img src="https://openswoole.com/images/swoole-logo.svg#gh-light-mode-only" width="200" /></a></p>

<a href="https://github.com/openswoole/ext-openswoole/actions?query=workflow%3Aext-openswoole"><img src="https://github.com/openswoole/ext-openswoole/workflows/ext-openswoole/badge.svg" alt="ext-openswoole" style="max-width: 100%;"></a>
<a href="https://github.com/openswoole/ext-openswoole/actions?query=workflow%3Atest-linux"><img src="https://github.com/openswoole/ext-openswoole/workflows/test-linux/badge.svg" alt="test-linux" style="max-width: 100%;"></a>

## OpenSwoole

OpenSwoole is a programmatic server for PHP with async IO, coroutines and fibers: secure, reliable, high performance

+ __Website__: <https://openswoole.com>
+ __Twitter__: <https://twitter.com/openswoole>
+ __Slack__: <https://goo.gl/forms/wooTTDmhbu30x4qC3>
+ __Discord__: <https://discord.gg/5QC57RNPpw>
+ __IDE Helper__: <https://github.com/openswoole/ide-helper>

## Documentation

Documentation for OpenSwoole can be found on the [OpenSwoole website](https://openswoole.com/docs).

## Installation

> OpenSwoole always provides the most reliable stability and the most powerful features in **the latest released version**. Please ensure as much as possible that you are using the latest version.

### 1. Install or upgrade OpenSwoole from multiple distribution channels

Please check [OpenSwoole Installation Guide](https://openswoole.com/docs/get-started/installation) about how to install OpenSwoole on Ubuntu/CentOS/Windows WSL from Docker, PECL or Binary releases channels.

### 2. Compile from source

#### Compiling requirements

+ Linux, OS X or Cygwin, WSL
+ PHP 8.3.0 or later (The higher the version, the better the performance.)
+ GCC 4.8 or later

Download the source packages from [Releases](https://github.com/openswoole/ext-openswoole/releases) or:

```shell
git clone https://github.com/openswoole/ext-openswoole.git && \
cd ext-openswoole
git checkout v26.2.0
phpize && \
./configure && \
make && make install
```

You can find how to fix [Common Installation Errors](https://openswoole.com/docs/get-started/common-install-errors) if there are errors in the installation.

#### Compile configurations

> for example: `./configure --enable-openssl --enable-sockets`

+ `--enable-openssl` or `--with-openssl-dir=DIR`
+ `--enable-sockets`
+ `--enable-http2`
+ `--enable-mysqlnd` (need mysqlnd, it just for supporting `$mysql->escape` method)
+ `--enable-hook-curl`
+ `--with-postgres[=DIR]`

#### Enable OpenSwoole extension

After compiling and installing the openswoole extension, you have to add a new line `extension=openswoole.so` at the end of `php.ini` or create a ini file at `conf.d` folder to enable OpenSwoole. It is recommended to be added after all the other extensions because openswoole may depend on extensions: sockets, mysqlnd, curl etc.

## Examples

Learn OpenSwoole by exploring the examples: <https://github.com/openswoole/openswoole/tree/master/example>

## Fiber Context and Xdebug Support

OpenSwoole supports using PHP's native Fiber API as the coroutine context backend. This enables compatibility with debugging and profiling tools like Xdebug that rely on PHP's fiber infrastructure to trace execution across coroutines.

### Enable PHP's Fiber Context

Add the following INI setting to your `php.ini`:

```ini
openswoole.use_fiber_context=On
```

Or enable in your code:

```php
Co::set(['use_fiber_context' => true]);
```

When enabled, OpenSwoole uses PHP fibers internally to switch between coroutines instead of the default boost ASM context. This allows tools like Xdebug to properly trace and debug coroutine execution, including step debugging, stack traces, and profiling across coroutine boundaries.

### Using Xdebug with OpenSwoole

1. Install and enable both the `xdebug` and `openswoole` extensions in your `php.ini`:

```ini
zend_extension=xdebug.so
extension=openswoole.so
openswoole.use_fiber_context=On
```

2. Configure Xdebug as usual (e.g. for step debugging):

```ini
xdebug.mode=debug
xdebug.start_with_request=yes
xdebug.client_host=127.0.0.1
xdebug.client_port=9003
```

3. Run your OpenSwoole application and connect your IDE debugger. Xdebug will be able to trace execution across coroutine switches.

> **Note:** Fiber context mode requires PHP 8.3 or later. There is a minor performance overhead compared to the default ASM context, so it is recommended to use this mode only during development and debugging.

4. Xdebug trace log example:

```bash
php -d xdebug.mode=trace \
  -d xdebug.start_with_request=yes \
  -d xdebug.output_dir=/tmp \
  -d xdebug.trace_output_name=openswoole_trace \
  -d xdebug.trace_format=0 \
  -d xdebug.collect_return=1 \
  -d xdebug.use_compression=false \
  examples/helloworld.php

tail -f /tmp/openswoole_trace.xt
```

## Reactor Settings

OpenSwoole uses an event-driven reactor for non-blocking I/O. The reactor type can be configured globally via `Co::set()`:

```php
Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
```

### Available Reactor Types

| Constant | Platform | Description |
|----------|----------|-------------|
| `OPENSWOOLE_SELECT` | All | `select()` system call, portable but limited scalability |
| `OPENSWOOLE_POLL` | Linux, macOS | `poll()` system call, no fd limit like select |
| `OPENSWOOLE_EPOLL` | Linux | `epoll`, high performance for large numbers of connections |
| `OPENSWOOLE_KQUEUE` | macOS, BSD | `kqueue`, high performance on BSD-based systems |
| `OPENSWOOLE_IO_URING` | Linux 5.1+ | `io_uring`, highest performance with async I/O support |

By default, OpenSwoole automatically selects the best available reactor for your platform (`epoll` on Linux, `kqueue` on macOS).

### Server Reactor Threads

For `OpenSwoole\Server`, the number of reactor threads can be configured:

```php
$server = new OpenSwoole\Server('0.0.0.0', 9501);
$server->set([
    'reactor_num' => 4, // defaults to number of CPU cores
]);
```

## Frameworks & Components

> PR are welcome if your framework is using openswoole
 
 - [**Laravel Octane**](https://laravel.com/docs/9.x/octane) Laravel Octane supercharges your application's performance by serving your application using high-powered application servers.
 - [**PHP Runtime**](https://github.com/php-runtime) make it easy to run any kind of PHP Application (Symfony, Laravel, PSR7, Native) with all kinds of Runtimes like OpenSwoole, Bref, Google Cloud Functions, Roadrunner and React PHP with minimal configuration.
 - [**Mezzio Swoole**](https://docs.mezzio.dev/mezzio-swoole/) allows you to run Mezzio and [PSR-15](https://www.php-fig.org/psr/psr-15/) applications on OpenSwoole.

## For Contributors

If you like to involve the maintenance of this repo, it is better to get started by submitting PR, you will be invited to the dev group once there are significant contributions. Or join Slack group firstly, the team will provide mentoring and internal support to help you get started.

* [Report issues and feedback](https://github.com/openswoole/ext-openswoole/issues)
* Submit fixes, features via Pull Request

This project exists thanks to all the historical [[Contributors](https://github.com/openswoole/ext-openswoole/graphs/contributors)].

## Security issues

Security issues should be reported privately, via email, to the OpenSwoole develop team [hello@openswoole.com](mailto:hello@openswoole.com). You should receive a response within 24 hours. If for some reason you do not, please follow up via email to ensure we received your original message.

## License

OpenSwoole is open-sourced software licensed under the [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0.html).
