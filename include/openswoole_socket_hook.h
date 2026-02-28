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

#ifndef OSW_SOCKET_HOOK_H_
#define OSW_SOCKET_HOOK_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "openswoole_coroutine_c_api.h"

#define socket(domain, type, protocol) openswoole_coroutine_socket(domain, type, protocol)
#define send(sockfd, buf, len, flags) openswoole_coroutine_send(sockfd, buf, len, flags)
#define read(sockfd, buf, len) openswoole_coroutine_read(sockfd, buf, len)
#define write(sockfd, buf, len) openswoole_coroutine_write(sockfd, buf, len)
#define recv(sockfd, buf, len, flags) openswoole_coroutine_recv(sockfd, buf, len, flags)
#define close(fd) openswoole_coroutine_close(fd)
#define connect(sockfd, addr, addrlen) openswoole_coroutine_connect(sockfd, addr, addrlen)
#define poll(fds, nfds, timeout) openswoole_coroutine_poll(fds, nfds, timeout)
#define sendmsg(sockfd, msg, flags) openswoole_coroutine_sendmsg(sockfd, msg, flags)
#define recvmsg(sockfd, msg, flags) openswoole_coroutine_recvmsg(sockfd, msg, flags)
#define getaddrinfo(name, service, req, pai) openswoole_coroutine_getaddrinfo(name, service, req, pai)
#define gethostbyname(name) openswoole_coroutine_gethostbyname(name)

#ifdef __cplusplus
}
#endif

#endif
