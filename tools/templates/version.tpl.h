/*
  +----------------------------------------------------------------------+
  | Open Swoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  +----------------------------------------------------------------------+
*/

#ifndef SWOOLE_VERSION_H_
#define SWOOLE_VERSION_H_

#define SWOOLE_MAJOR_VERSION <?=$next->major."\n" ?>
#define SWOOLE_MINOR_VERSION <?=$next->minor."\n" ?>
#define SWOOLE_RELEASE_VERSION <?=$next->release."\n" ?>
#define SWOOLE_EXTRA_VERSION "<?=$next->extra ?>"
#define SWOOLE_VERSION "<?=$next->getVersion() ?>"
#define SWOOLE_VERSION_ID <?=$next->getVersionId()."\n" ?>
#define SWOOLE_API_VERSION_ID 0x202012a

#define SWOOLE_BUG_REPORT                                                                                              \
    "A bug occurred in OpenSwoole-v" SWOOLE_VERSION ", please report it.\n"                                            \
    "Please submit bug report at:\n"                                                                                   \
    ">> https://github.com/openswoole/swoole-src/issues \n"                                                            \
#endif
