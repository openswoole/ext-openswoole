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

#ifndef OSW_COROUTINE_API_H_
#define OSW_COROUTINE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <stdio.h>
#include <netdb.h>
#include <poll.h>
#include <dirent.h>

/**
 * base
 */
uint8_t openswoole_coroutine_is_in();
long openswoole_coroutine_get_current_id();
void openswoole_coroutine_sleep(int sec);
void openswoole_coroutine_usleep(int usec);
/**
 * file
 */
int openswoole_coroutine_access(const char *pathname, int mode);
int openswoole_coroutine_open(const char *pathname, int flags, mode_t mode);
ssize_t openswoole_coroutine_read(int fd, void *buf, size_t count);
ssize_t openswoole_coroutine_write(int fd, const void *buf, size_t count);
off_t openswoole_coroutine_lseek(int fd, off_t offset, int whence);
int openswoole_coroutine_fstat(int fd, struct stat *statbuf);
int openswoole_coroutine_readlink(const char *pathname, char *buf, size_t len);
int openswoole_coroutine_unlink(const char *pathname);
int openswoole_coroutine_mkdir(const char *pathname, mode_t mode);
int openswoole_coroutine_rmdir(const char *pathname);
int openswoole_coroutine_rename(const char *oldpath, const char *newpath);
int openswoole_coroutine_flock(int fd, int operation);
int openswoole_coroutine_flock_ex(const char *filename, int fd, int operation);
int openswoole_coroutine_statvfs(const char *path, struct statvfs *buf);
/**
 * stdio
 */
FILE *openswoole_coroutine_fopen(const char *pathname, const char *mode);
FILE *openswoole_coroutine_fdopen(int fd, const char *mode);
FILE *openswoole_coroutine_freopen(const char *pathname, const char *mode, FILE *stream);
size_t openswoole_coroutine_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t openswoole_coroutine_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
char *openswoole_coroutine_fgets(char *s, int size, FILE *stream);
int openswoole_coroutine_fputs(const char *s, FILE *stream);
int openswoole_coroutine_feof(FILE *stream);
int openswoole_coroutine_fclose(FILE *stream);
/**
 * dir
 */
DIR *openswoole_coroutine_opendir(const char *name);
struct dirent *openswoole_coroutine_readdir(DIR *dirp);
int openswoole_coroutine_closedir(DIR *dirp);
/**
 * socket
 */
int openswoole_coroutine_socket(int domain, int type, int protocol);
int openswoole_coroutine_socket_create(int fd);
uint8_t openswoole_coroutine_socket_exists(int fd);
ssize_t openswoole_coroutine_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t openswoole_coroutine_sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t openswoole_coroutine_recv(int sockfd, void *buf, size_t len, int flags);
ssize_t openswoole_coroutine_recvmsg(int sockfd, struct msghdr *msg, int flags);
int openswoole_coroutine_close(int fd);
int openswoole_coroutine_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int openswoole_coroutine_poll(struct pollfd *fds, nfds_t nfds, int timeout);
int openswoole_coroutine_socket_set_timeout(int fd, int which, double timeout);
int openswoole_coroutine_socket_set_connect_timeout(int fd, double timeout);
int openswoole_coroutine_socket_wait_event(int fd, int event, double timeout);
int openswoole_coroutine_getaddrinfo(const char *name,
                                 const char *service,
                                 const struct addrinfo *req,
                                 struct addrinfo **pai);
struct hostent *openswoole_coroutine_gethostbyname(const char *name);
/**
 * wait
 */
size_t openswoole_coroutine_wait_count();
pid_t openswoole_coroutine_waitpid(pid_t __pid, int *__stat_loc, int __options);
pid_t openswoole_coroutine_wait(int *__stat_loc);

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif
