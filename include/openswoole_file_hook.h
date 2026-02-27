/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#ifndef OSW_FILE_HOOK_H_
#define OSW_FILE_HOOK_H_

#include "openswoole_coroutine_c_api.h"

#define access(pathname, mode) openswoole_coroutine_access(pathname, mode)
#define open(pathname, flags, mode) openswoole_coroutine_open(pathname, flags, mode)
#define read(fd, buf, count) openswoole_coroutine_read(fd, buf, count)
#define write(fd, buf, count) openswoole_coroutine_write(fd, buf, count)
#define lseek(fd, offset, whence) openswoole_coroutine_lseek(fd, offset, whence)
#define fstat(fd, statbuf) openswoole_coroutine_fstat(fd, statbuf)
#define readlink(fd, buf, size) openswoole_coroutine_readlink(fd, buf, size)
#define unlink(pathname) openswoole_coroutine_unlink(pathname)
#define mkdir(pathname, mode) openswoole_coroutine_mkdir(pathname, mode)
#define rmdir(pathname) openswoole_coroutine_rmdir(pathname)
#define rename(oldpath, newpath) openswoole_coroutine_rename(oldpath, newpath)

#define fopen(pathname, mode) openswoole_coroutine_fopen(pathname, mode)
#define fdopen(fd, mode) openswoole_coroutine_fdopen(fd, mode)
#define freopen(pathname, mode, stream) openswoole_coroutine_freopen(pathname, mode, stream)
#define fread(ptr, size, nmemb, stream) openswoole_coroutine_fread(ptr, size, nmemb, stream)
#define fwrite(ptr, size, nmemb, stream) openswoole_coroutine_fwrite(ptr, size, nmemb, stream)
#define fgets(s, size, stream) openswoole_coroutine_fgets(s, size, stream)
#define fputs(s, stream) openswoole_coroutine_fputs(s, stream)
#define feof(stream) openswoole_coroutine_feof(stream)
#define fclose(stream) openswoole_coroutine_fclose(stream)

#define opendir(name) openswoole_coroutine_opendir(name)
#define readdir(dir) openswoole_coroutine_readdir(dir)
#define closedir(dir) openswoole_coroutine_closedir(dir)

#endif
