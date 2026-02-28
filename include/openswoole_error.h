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

#pragma once

enum swErrorCode {
    /**
     * Prevent repetition with errno [syscall error]
     */
    OSW_ERROR_BEGIN = 500,

    /**
     * common error
     */
    OSW_ERROR_MALLOC_FAIL = 501,
    OSW_ERROR_SYSTEM_CALL_FAIL,
    OSW_ERROR_PHP_FATAL_ERROR,
    OSW_ERROR_NAME_TOO_LONG,
    OSW_ERROR_INVALID_PARAMS,
    OSW_ERROR_QUEUE_FULL,
    OSW_ERROR_OPERATION_NOT_SUPPORT,
    OSW_ERROR_PROTOCOL_ERROR,
    OSW_ERROR_WRONG_OPERATION,

    OSW_ERROR_FILE_NOT_EXIST = 700,
    OSW_ERROR_FILE_TOO_LARGE,
    OSW_ERROR_FILE_EMPTY,

    OSW_ERROR_DNSLOOKUP_DUPLICATE_REQUEST = 710,
    OSW_ERROR_DNSLOOKUP_RESOLVE_FAILED,
    OSW_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT,
    OSW_ERROR_DNSLOOKUP_UNSUPPORTED,
    OSW_ERROR_DNSLOOKUP_NO_SERVER,

    OSW_ERROR_BAD_IPV6_ADDRESS = 720,
    OSW_ERROR_UNREGISTERED_SIGNAL,

    // EventLoop
    OSW_ERROR_EVENT_SOCKET_REMOVED = 800,

    /**
     * connection error
     */
    OSW_ERROR_SESSION_CLOSED_BY_SERVER = 1001,
    OSW_ERROR_SESSION_CLOSED_BY_CLIENT,
    OSW_ERROR_SESSION_CLOSING,
    OSW_ERROR_SESSION_CLOSED,
    OSW_ERROR_SESSION_NOT_EXIST,
    OSW_ERROR_SESSION_INVALID_ID,
    OSW_ERROR_SESSION_DISCARD_TIMEOUT_DATA,
    OSW_ERROR_SESSION_DISCARD_DATA,
    OSW_ERROR_OUTPUT_BUFFER_OVERFLOW,
    OSW_ERROR_OUTPUT_SEND_YIELD,
    OSW_ERROR_SSL_NOT_READY,
    OSW_ERROR_SSL_CANNOT_USE_SENFILE,
    OSW_ERROR_SSL_EMPTY_PEER_CERTIFICATE,
    OSW_ERROR_SSL_VERIFY_FAILED,
    OSW_ERROR_SSL_BAD_CLIENT,
    OSW_ERROR_SSL_BAD_PROTOCOL,
    OSW_ERROR_SSL_RESET,
    OSW_ERROR_SSL_HANDSHAKE_FAILED,

    OSW_ERROR_PACKAGE_LENGTH_TOO_LARGE = 1201,
    OSW_ERROR_PACKAGE_LENGTH_NOT_FOUND,
    OSW_ERROR_DATA_LENGTH_TOO_LARGE,

    /**
     * task error
     */
    OSW_ERROR_TASK_PACKAGE_TOO_BIG = 2001,
    OSW_ERROR_TASK_DISPATCH_FAIL,
    OSW_ERROR_TASK_TIMEOUT,

    /**
     * http2 protocol error
     */
    OSW_ERROR_HTTP2_STREAM_ID_TOO_BIG = 3001,
    OSW_ERROR_HTTP2_STREAM_NO_HEADER,
    OSW_ERROR_HTTP2_STREAM_NOT_FOUND,
    OSW_ERROR_HTTP2_STREAM_IGNORE,

    /**
     * AIO
     */
    OSW_ERROR_AIO_BAD_REQUEST = 4001,
    OSW_ERROR_AIO_CANCELED,
    OSW_ERROR_AIO_TIMEOUT,

    /**
     * Client
     */
    OSW_ERROR_CLIENT_NO_CONNECTION = 5001,

    /**
     * Socket
     */
    OSW_ERROR_SOCKET_CLOSED = 6001,
    OSW_ERROR_SOCKET_POLL_TIMEOUT,

    /**
     * Proxy
     */
    OSW_ERROR_SOCKS5_UNSUPPORT_VERSION = 7001,
    OSW_ERROR_SOCKS5_UNSUPPORT_METHOD,
    OSW_ERROR_SOCKS5_AUTH_FAILED,
    OSW_ERROR_SOCKS5_SERVER_ERROR,
    OSW_ERROR_SOCKS5_HANDSHAKE_FAILED,

    OSW_ERROR_HTTP_PROXY_HANDSHAKE_ERROR = 7101,
    OSW_ERROR_HTTP_INVALID_PROTOCOL,
    OSW_ERROR_HTTP_PROXY_HANDSHAKE_FAILED,
    OSW_ERROR_HTTP_PROXY_BAD_RESPONSE,

    OSW_ERROR_WEBSOCKET_BAD_CLIENT = 8501,
    OSW_ERROR_WEBSOCKET_BAD_OPCODE,
    OSW_ERROR_WEBSOCKET_UNCONNECTED,
    OSW_ERROR_WEBSOCKET_HANDSHAKE_FAILED,
    OSW_ERROR_WEBSOCKET_PACK_FAILED,

    /**
     * server global error
     */
    OSW_ERROR_SERVER_MUST_CREATED_BEFORE_CLIENT = 9001,
    OSW_ERROR_SERVER_TOO_MANY_SOCKET,
    OSW_ERROR_SERVER_WORKER_TERMINATED,
    OSW_ERROR_SERVER_INVALID_LISTEN_PORT,
    OSW_ERROR_SERVER_TOO_MANY_LISTEN_PORT,
    OSW_ERROR_SERVER_PIPE_BUFFER_FULL,
    OSW_ERROR_SERVER_NO_IDLE_WORKER,
    OSW_ERROR_SERVER_ONLY_START_ONE,
    OSW_ERROR_SERVER_SEND_IN_MASTER,
    OSW_ERROR_SERVER_INVALID_REQUEST,
    OSW_ERROR_SERVER_CONNECT_FAIL,

    /**
     * Process exit timeout, forced to end.
     */
    OSW_ERROR_SERVER_WORKER_EXIT_TIMEOUT,
    OSW_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA,
    OSW_ERROR_SERVER_WORKER_UNPROCESSED_DATA,

    /**
     * Coroutine
     */
    OSW_ERROR_CO_OUT_OF_COROUTINE = 10001,
    OSW_ERROR_CO_HAS_BEEN_BOUND,
    OSW_ERROR_CO_HAS_BEEN_DISCARDED,

    OSW_ERROR_CO_MUTEX_DOUBLE_UNLOCK,
    OSW_ERROR_CO_BLOCK_OBJECT_LOCKED,
    OSW_ERROR_CO_BLOCK_OBJECT_WAITING,
    OSW_ERROR_CO_YIELD_FAILED,
    OSW_ERROR_CO_GETCONTEXT_FAILED,
    OSW_ERROR_CO_SWAPCONTEXT_FAILED,
    OSW_ERROR_CO_MAKECONTEXT_FAILED,

    OSW_ERROR_CO_IOCPINIT_FAILED,
    OSW_ERROR_CO_PROTECT_STACK_FAILED,
    OSW_ERROR_CO_STD_THREAD_LINK_ERROR,
    OSW_ERROR_CO_DISABLED_MULTI_THREAD,

    OSW_ERROR_CO_CANNOT_CANCEL,
    OSW_ERROR_CO_NOT_EXISTS,
    OSW_ERROR_CO_CANCELED,
    OSW_ERROR_CO_TIMEDOUT,

    OSW_ERROR_END
};

namespace openswoole {
class Exception {
  public:
    int code;
    const char *msg;

    Exception(int code);
};
}  // namespace openswoole
