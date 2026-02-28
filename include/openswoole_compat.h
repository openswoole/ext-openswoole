/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  +----------------------------------------------------------------------+
  | Backward-Compatibility Header                                        |
  | Maps all legacy Swoole names to the canonical OpenSwoole names.      |
  | Include this header to allow code using old names to compile.        |
  +----------------------------------------------------------------------+
*/

#pragma once

/*==========================================================================
 * C++ namespace alias
 *==========================================================================*/
#ifdef __cplusplus
namespace swoole = openswoole;
#endif

/*==========================================================================
 * Global variable compat
 *==========================================================================*/
#define SwooleG   OpenSwooleG
#define SwooleTG  OpenSwooleTG
#define SwooleWG  OpenSwooleWG

/*==========================================================================
 * Version defines compat
 *==========================================================================*/
#define SWOOLE_MAJOR_VERSION     OPENSWOOLE_MAJOR_VERSION
#define SWOOLE_MINOR_VERSION     OPENSWOOLE_MINOR_VERSION
#define SWOOLE_RELEASE_VERSION   OPENSWOOLE_RELEASE_VERSION
#define SWOOLE_EXTRA_VERSION     OPENSWOOLE_EXTRA_VERSION
#define SWOOLE_VERSION           OPENSWOOLE_VERSION
#define SWOOLE_VERSION_ID        OPENSWOOLE_VERSION_ID
#define SWOOLE_API_VERSION_ID    OPENSWOOLE_API_VERSION_ID
#define SWOOLE_BUG_REPORT        OPENSWOOLE_BUG_REPORT
#define SWOOLE_VERSION_H_        OPENSWOOLE_VERSION_H_

/*==========================================================================
 * Compiler / platform macros compat
 *==========================================================================*/
#define SW_EXTERN_C_BEGIN    OSW_EXTERN_C_BEGIN
#define SW_EXTERN_C_END      OSW_EXTERN_C_END
#define sw_inline            osw_inline
#define SW_API               OSW_API
#define sw_likely(x)         osw_likely(x)
#define sw_unlikely(x)       osw_unlikely(x)

/*==========================================================================
 * String / formatting macros compat
 *==========================================================================*/
#define SW_START_LINE          OSW_START_LINE
#define SW_END_LINE            OSW_END_LINE
#define SW_ECHO_RED            OSW_ECHO_RED
#define SW_ECHO_GREEN          OSW_ECHO_GREEN
#define SW_ECHO_YELLOW         OSW_ECHO_YELLOW
#define SW_ECHO_BLUE           OSW_ECHO_BLUE
#define SW_ECHO_MAGENTA        OSW_ECHO_MAGENTA
#define SW_ECHO_CYAN           OSW_ECHO_CYAN
#define SW_ECHO_WHITE          OSW_ECHO_WHITE
#define SW_COLOR_RED           OSW_COLOR_RED
#define SW_COLOR_GREEN         OSW_COLOR_GREEN
#define SW_COLOR_YELLOW        OSW_COLOR_YELLOW
#define SW_COLOR_BLUE          OSW_COLOR_BLUE
#define SW_COLOR_MAGENTA       OSW_COLOR_MAGENTA
#define SW_COLOR_CYAN          OSW_COLOR_CYAN
#define SW_COLOR_WHITE         OSW_COLOR_WHITE
#define SW_SPACE               OSW_SPACE
#define SW_CRLF                OSW_CRLF
#define SW_CRLF_LEN            OSW_CRLF_LEN
#define SW_ASCII_CODE_0        OSW_ASCII_CODE_0
#define SW_ASCII_CODE_Z        OSW_ASCII_CODE_Z

/*==========================================================================
 * Utility macros compat
 *==========================================================================*/
#define SW_MAX(A, B)            OSW_MAX(A, B)
#define SW_MIN(A, B)            OSW_MIN(A, B)
#define SW_LOOP_N(n)            OSW_LOOP_N(n)
#define SW_LOOP                 OSW_LOOP
#define SW_NUM_BILLION          OSW_NUM_BILLION
#define SW_NUM_MILLION          OSW_NUM_MILLION
#define SW_ASSERT(e)            OSW_ASSERT(e)
#define SW_ASSERT_1BYTE(v)      OSW_ASSERT_1BYTE(v)
#define SW_START_SLEEP          OSW_START_SLEEP
#define SW_ARRAY_SIZE(array)    OSW_ARRAY_SIZE(array)
#define SW_CPU_NUM              OSW_CPU_NUM

/*==========================================================================
 * Memory alignment macros compat
 *==========================================================================*/
#define SW_DEFAULT_ALIGNMENT        OSW_DEFAULT_ALIGNMENT
#define SW_MEM_ALIGNED_SIZE(s)      OSW_MEM_ALIGNED_SIZE(s)
#define SW_MEM_ALIGNED_SIZE_EX(s,a) OSW_MEM_ALIGNED_SIZE_EX(s,a)

/*==========================================================================
 * Memory function compat
 *==========================================================================*/
#define sw_malloc       osw_malloc
#define sw_free         osw_free
#define sw_calloc       osw_calloc
#define sw_realloc      osw_realloc
#define sw_strdup       osw_strdup
#define sw_strndup      osw_strndup
#define sw_snprintf     osw_snprintf
#define sw_vsnprintf    osw_vsnprintf
#define sw_memset_zero  osw_memset_zero
#define sw_mem_equal    osw_mem_equal
#define sw_yield()      osw_yield()
#define sw_spinlock     osw_spinlock
#define sw_tg_buffer    osw_tg_buffer
#define sw_mem_pool     osw_mem_pool
#define sw_std_allocator osw_std_allocator
#define sw_error        osw_error

/*==========================================================================
 * String helper macros compat
 *==========================================================================*/
#define SW_STRS(s)               OSW_STRS(s)
#define SW_STRL(s)               OSW_STRL(s)
#define SW_STREQ(str,len,cs)     OSW_STREQ(str,len,cs)
#define SW_STRCASEEQ(str,len,cs) OSW_STRCASEEQ(str,len,cs)
#define SW_STRCT(str,len,cs)     OSW_STRCT(str,len,cs)
#define SW_STRCASECT(str,len,cs) OSW_STRCASECT(str,len,cs)
#define SW_STRINGL(s)            OSW_STRINGL(s)
#define SW_STRINGS(s)            OSW_STRINGS(s)
#define SW_STRINGCVL(s)          OSW_STRINGCVL(s)

/*==========================================================================
 * Result / return code enum compat
 *==========================================================================*/
#define swResultCode     oswResultCode
#define SW_OK            OSW_OK
#define SW_ERR           OSW_ERR

#define swReturnCode     oswReturnCode
#define SW_CONTINUE      OSW_CONTINUE
#define SW_WAIT          OSW_WAIT
#define SW_CLOSE         OSW_CLOSE
#define SW_ERROR         OSW_ERROR
#define SW_READY         OSW_READY
#define SW_INVALID       OSW_INVALID

/*==========================================================================
 * FD type enum compat
 *==========================================================================*/
#define swFdType              oswFdType
#define SW_FD_SESSION         OSW_FD_SESSION
#define SW_FD_STREAM_SERVER   OSW_FD_STREAM_SERVER
#define SW_FD_DGRAM_SERVER    OSW_FD_DGRAM_SERVER
#define SW_FD_PIPE            OSW_FD_PIPE
#define SW_FD_STREAM          OSW_FD_STREAM
#define SW_FD_AIO             OSW_FD_AIO
#define SW_FD_CORO_SOCKET     OSW_FD_CORO_SOCKET
#define SW_FD_CORO_POLL       OSW_FD_CORO_POLL
#define SW_FD_CORO_EVENT      OSW_FD_CORO_EVENT
#define SW_FD_SIGNAL          OSW_FD_SIGNAL
#define SW_FD_DNS_RESOLVER    OSW_FD_DNS_RESOLVER
#define SW_FD_CARES           OSW_FD_CARES
#define SW_FD_IO_URING        OSW_FD_IO_URING
#define SW_FD_USER            OSW_FD_USER
#define SW_FD_STREAM_CLIENT   OSW_FD_STREAM_CLIENT
#define SW_FD_DGRAM_CLIENT    OSW_FD_DGRAM_CLIENT

/*==========================================================================
 * Socket flag / type enum compat
 *==========================================================================*/
#define swSocketFlag      oswSocketFlag
#define SW_SOCK_NONBLOCK  OSW_SOCK_NONBLOCK
#define SW_SOCK_CLOEXEC   OSW_SOCK_CLOEXEC
#define SW_SOCK_SSL       OSW_SOCK_SSL

#define swSocketType           oswSocketType
#define SW_SOCK_TCP            OSW_SOCK_TCP
#define SW_SOCK_UDP            OSW_SOCK_UDP
#define SW_SOCK_TCP6           OSW_SOCK_TCP6
#define SW_SOCK_UDP6           OSW_SOCK_UDP6
#define SW_SOCK_UNIX_STREAM    OSW_SOCK_UNIX_STREAM
#define SW_SOCK_UNIX_DGRAM     OSW_SOCK_UNIX_DGRAM
#define SW_SOCK_RAW            OSW_SOCK_RAW

/*==========================================================================
 * Event type enum compat
 *==========================================================================*/
#define swEventType        oswEventType
#define SW_EVENT_NULL      OSW_EVENT_NULL
#define SW_EVENT_DEAULT    OSW_EVENT_DEAULT
#define SW_EVENT_READ      OSW_EVENT_READ
#define SW_EVENT_WRITE     OSW_EVENT_WRITE
#define SW_EVENT_RDWR      OSW_EVENT_RDWR
#define SW_EVENT_ERROR     OSW_EVENT_ERROR
#define SW_EVENT_ONCE      OSW_EVENT_ONCE

/*==========================================================================
 * Fork type enum compat
 *==========================================================================*/
#define swForkType         oswForkType
#define SW_FORK_SPAWN      OSW_FORK_SPAWN
#define SW_FORK_EXEC       OSW_FORK_EXEC
#define SW_FORK_DAEMON     OSW_FORK_DAEMON
#define SW_FORK_PRECHECK   OSW_FORK_PRECHECK

/*==========================================================================
 * Event data flag enum compat
 *==========================================================================*/
#define swEventDataFlag        oswEventDataFlag
#define SW_EVENT_DATA_NORMAL   OSW_EVENT_DATA_NORMAL
#define SW_EVENT_DATA_PTR      OSW_EVENT_DATA_PTR
#define SW_EVENT_DATA_CHUNK    OSW_EVENT_DATA_CHUNK
#define SW_EVENT_DATA_BEGIN    OSW_EVENT_DATA_BEGIN
#define SW_EVENT_DATA_END      OSW_EVENT_DATA_END
#define SW_EVENT_DATA_OBJ_PTR  OSW_EVENT_DATA_OBJ_PTR
#define SW_EVENT_DATA_POP_PTR  OSW_EVENT_DATA_POP_PTR

/*==========================================================================
 * Task macros compat
 *==========================================================================*/
#define SW_TASK_TYPE(t)    OSW_TASK_TYPE(t)
#define swTaskType         oswTaskType
#define SW_TASK_TMPFILE    OSW_TASK_TMPFILE
#define SW_TASK_SERIALIZE  OSW_TASK_SERIALIZE
#define SW_TASK_NONBLOCK   OSW_TASK_NONBLOCK
#define SW_TASK_CALLBACK   OSW_TASK_CALLBACK
#define SW_TASK_WAITALL    OSW_TASK_WAITALL
#define SW_TASK_COROUTINE  OSW_TASK_COROUTINE
#define SW_TASK_PEEK       OSW_TASK_PEEK
#define SW_TASK_NOREPLY    OSW_TASK_NOREPLY

/*==========================================================================
 * DNS lookup flag enum compat
 *==========================================================================*/
#define swDNSLookupFlag        oswDNSLookupFlag
#define SW_DNS_LOOKUP_RANDOM   OSW_DNS_LOOKUP_RANDOM

/*==========================================================================
 * Process type enum compat
 *==========================================================================*/
#define swProcessType          oswProcessType
#define SW_PROCESS_MASTER      OSW_PROCESS_MASTER
#define SW_PROCESS_WORKER      OSW_PROCESS_WORKER
#define SW_PROCESS_MANAGER     OSW_PROCESS_MANAGER
#define SW_PROCESS_TASKWORKER  OSW_PROCESS_TASKWORKER
#define SW_PROCESS_USERWORKER  OSW_PROCESS_USERWORKER

/*==========================================================================
 * Pipe type enum compat
 *==========================================================================*/
#define swPipeType         oswPipeType
#define SW_PIPE_WORKER     OSW_PIPE_WORKER
#define SW_PIPE_MASTER     OSW_PIPE_MASTER
#define SW_PIPE_READ       OSW_PIPE_READ
#define SW_PIPE_WRITE      OSW_PIPE_WRITE
#define SW_PIPE_NONBLOCK   OSW_PIPE_NONBLOCK

/*==========================================================================
 * C typedef compat (old sw* → new osw*)
 *==========================================================================*/
#ifdef __cplusplus
typedef oswReactor    swReactor;
typedef oswString     swString;
typedef oswTimer      swTimer;
typedef oswSocket     swSocket;
typedef oswProtocol   swProtocol;
typedef oswEventData  swEventData;
typedef oswDataHead   swDataHead;
typedef oswEvent      swEvent;
#endif

/* Server types - use macros since oswServer etc. are defined in openswoole_server.h */
#define swServer     oswServer
#define swListenPort oswListenPort
#define swRecvData   oswRecvData

/* Signal type compat */
#define swSignalHandler  oswSignalHandler

/*==========================================================================
 * Atomic type compat
 *==========================================================================*/
#define sw_atomic_int32_t      osw_atomic_int32_t
#define sw_atomic_uint32_t     osw_atomic_uint32_t
#define sw_atomic_int64_t      osw_atomic_int64_t
#define sw_atomic_uint64_t     osw_atomic_uint64_t
#define sw_atomic_long_t       osw_atomic_long_t
#define sw_atomic_ulong_t      osw_atomic_ulong_t
#define sw_atomic_t            osw_atomic_t
#define sw_atomic_cmp_set      osw_atomic_cmp_set
#define sw_atomic_fetch_add    osw_atomic_fetch_add
#define sw_atomic_fetch_sub    osw_atomic_fetch_sub
#define sw_atomic_memory_barrier  osw_atomic_memory_barrier
#define sw_atomic_add_fetch    osw_atomic_add_fetch
#define sw_atomic_sub_fetch    osw_atomic_sub_fetch
#define sw_atomic_cpu_pause    osw_atomic_cpu_pause

/*==========================================================================
 * Config macros compat (swoole_config.h)
 *==========================================================================*/
#define SW_MAX_FDTYPE                  OSW_MAX_FDTYPE
#define SW_MAX_HOOK_TYPE               OSW_MAX_HOOK_TYPE
#define SW_MAX_CONCURRENT_TASK         OSW_MAX_CONCURRENT_TASK
#define SW_MAX_CONNECTION              OSW_MAX_CONNECTION
#define SW_MAX_LISTEN_PORT             OSW_MAX_LISTEN_PORT
#define SW_MAX_SOCKETS_DEFAULT         OSW_MAX_SOCKETS_DEFAULT
#define SW_MAX_WORKER_NCPU             OSW_MAX_WORKER_NCPU
#define SW_MAX_THREAD_NCPU             OSW_MAX_THREAD_NCPU
#define SW_MAX_FILE_CONTENT            OSW_MAX_FILE_CONTENT
#define SW_STACK_BUFFER_SIZE           OSW_STACK_BUFFER_SIZE
#define SW_BUFFER_SIZE_STD             OSW_BUFFER_SIZE_STD
#define SW_BUFFER_SIZE_BIG             OSW_BUFFER_SIZE_BIG
#define SW_BUFFER_SIZE_UDP             OSW_BUFFER_SIZE_UDP
#define SW_BUFFER_MIN_SIZE             OSW_BUFFER_MIN_SIZE
#define SW_SEND_BUFFER_SIZE            OSW_SEND_BUFFER_SIZE
#define SW_INPUT_BUFFER_SIZE           OSW_INPUT_BUFFER_SIZE
#define SW_OUTPUT_BUFFER_SIZE          OSW_OUTPUT_BUFFER_SIZE
#define SW_BACKLOG                     OSW_BACKLOG
#define SW_IPC_MAX_SIZE                OSW_IPC_MAX_SIZE
#define SW_IPC_BUFFER_SIZE             OSW_IPC_BUFFER_SIZE
#define SW_DGRAM_HEADER_SIZE           OSW_DGRAM_HEADER_SIZE
#define SW_REACTOR_MAXEVENTS           OSW_REACTOR_MAXEVENTS
#define SW_REACTOR_MAX_THREAD          OSW_REACTOR_MAX_THREAD
#define SW_REACTOR_RECV_AGAIN          OSW_REACTOR_RECV_AGAIN
#define SW_REACTOR_CONTINUE            OSW_REACTOR_CONTINUE
#define SW_SESSION_LIST_SIZE           OSW_SESSION_LIST_SIZE
#define SW_ACCEPT_MAX_COUNT            OSW_ACCEPT_MAX_COUNT
#define SW_ACCEPT_RETRY_TIME           OSW_ACCEPT_RETRY_TIME
#define SW_SIGNO_MAX                   OSW_SIGNO_MAX
#define SW_DATA_EOF                    OSW_DATA_EOF
#define SW_DATA_EOF_MAXLEN             OSW_DATA_EOF_MAXLEN
#define SW_TASKWAIT_TIMEOUT            OSW_TASKWAIT_TIMEOUT
#define SW_AIO_THREAD_NUM_MULTIPLE     OSW_AIO_THREAD_NUM_MULTIPLE
#define SW_AIO_MAX_FILESIZE            OSW_AIO_MAX_FILESIZE
#define SW_AIO_EVENT_NUM               OSW_AIO_EVENT_NUM
#define SW_AIO_DEFAULT_CHUNK_SIZE      OSW_AIO_DEFAULT_CHUNK_SIZE
#define SW_AIO_MAX_CHUNK_SIZE          OSW_AIO_MAX_CHUNK_SIZE
#define SW_AIO_MAX_EVENTS              OSW_AIO_MAX_EVENTS
#define SW_AIO_HANDLER_MAX_SIZE        OSW_AIO_HANDLER_MAX_SIZE
#define SW_AIO_TASK_MAX_WAIT_TIME      OSW_AIO_TASK_MAX_WAIT_TIME
#define SW_AIO_THREAD_MAX_IDLE_TIME    OSW_AIO_THREAD_MAX_IDLE_TIME
#define SW_THREADPOOL_QUEUE_LEN        OSW_THREADPOOL_QUEUE_LEN
#define SW_IP_MAX_LENGTH               OSW_IP_MAX_LENGTH
#define SW_WORKER_WAIT_TIMEOUT         OSW_WORKER_WAIT_TIMEOUT
#define SW_WORKER_MAX_WAIT_TIME        OSW_WORKER_MAX_WAIT_TIME
#define SW_WORKER_MAX_RECV_CHUNK_COUNT OSW_WORKER_MAX_RECV_CHUNK_COUNT
#define SW_WORKER_MIN_REQUEST          OSW_WORKER_MIN_REQUEST
#define SW_SYSTEMD_FDS_START           OSW_SYSTEMD_FDS_START
#define SW_HOST_MAXSIZE                OSW_HOST_MAXSIZE
#define SW_LOG_BUFFER_SIZE             OSW_LOG_BUFFER_SIZE
#define SW_LOG_DATE_STRLEN             OSW_LOG_DATE_STRLEN
#define SW_LOG_DEFAULT_DATE_FORMAT     OSW_LOG_DEFAULT_DATE_FORMAT
#define SW_LOG_NO_SRCINFO              OSW_LOG_NO_SRCINFO
#define SW_LOG_TRACE_OPEN              OSW_LOG_TRACE_OPEN
#define SW_CLIENT_BUFFER_SIZE          OSW_CLIENT_BUFFER_SIZE
#define SW_CLIENT_CONNECT_TIMEOUT      OSW_CLIENT_CONNECT_TIMEOUT
#define SW_CLIENT_MAX_PORT             OSW_CLIENT_MAX_PORT
#define SW_DEFAULT_C_STACK_SIZE        OSW_DEFAULT_C_STACK_SIZE
#define SW_CONTEXT_PROTECT_STACK_PAGE  OSW_CONTEXT_PROTECT_STACK_PAGE
#define SW_CONTEXT_DETECT_STACK_USAGE  OSW_CONTEXT_DETECT_STACK_USAGE
#define SW_CORO_SUPPORT_BAILOUT        OSW_CORO_SUPPORT_BAILOUT
#define SW_CORO_SWAP_BAILOUT           OSW_CORO_SWAP_BAILOUT
#define SW_ERROR_MSG_SIZE              OSW_ERROR_MSG_SIZE
#define SW_SPINLOCK_LOOP_N             OSW_SPINLOCK_LOOP_N
#define SW_GLOBAL_MEMORY_PAGESIZE      OSW_GLOBAL_MEMORY_PAGESIZE
#define SW_STRING_BUFFER_DEFAULT       OSW_STRING_BUFFER_DEFAULT
#define SW_STRING_BUFFER_MAXLEN        OSW_STRING_BUFFER_MAXLEN
#define SW_STRING_BUFFER_GARBAGE_MIN   OSW_STRING_BUFFER_GARBAGE_MIN
#define SW_STRING_BUFFER_GARBAGE_RATIO OSW_STRING_BUFFER_GARBAGE_RATIO
#define SW_RINGBUFFER_FREE_N_MAX       OSW_RINGBUFFER_FREE_N_MAX
#define SW_RINGBUFFER_WARNING          OSW_RINGBUFFER_WARNING
#define SW_RINGQUEUE_LEN               OSW_RINGQUEUE_LEN
#define SW_HASHMAP_KEY_MAXLEN          OSW_HASHMAP_KEY_MAXLEN
#define SW_HASHMAP_INIT_BUCKET_N       OSW_HASHMAP_INIT_BUCKET_N
#define SW_TABLE_KEY_SIZE              OSW_TABLE_KEY_SIZE
#define SW_TABLE_CONFLICT_PROPORTION   OSW_TABLE_CONFLICT_PROPORTION
#define SW_TABLE_USE_PHP_HASH          OSW_TABLE_USE_PHP_HASH
#define SW_TABLE_FORCE_UNLOCK_TIME     OSW_TABLE_FORCE_UNLOCK_TIME
#define SW_TABLE_DEBUG                 OSW_TABLE_DEBUG
#define SW_UNIXSOCK_MAX_BUF_SIZE       OSW_UNIXSOCK_MAX_BUF_SIZE
#define SW_SOCKET_BUFFER_SIZE          OSW_SOCKET_BUFFER_SIZE
#define SW_SOCKET_MAX_DEFAULT          OSW_SOCKET_MAX_DEFAULT
#define SW_SOCKET_OVERFLOW_WAIT        OSW_SOCKET_OVERFLOW_WAIT
#define SW_SOCKET_RETRY_COUNT          OSW_SOCKET_RETRY_COUNT
#define SW_SOCKET_DEFAULT_CONNECT_TIMEOUT  OSW_SOCKET_DEFAULT_CONNECT_TIMEOUT
#define SW_SOCKET_DEFAULT_READ_TIMEOUT     OSW_SOCKET_DEFAULT_READ_TIMEOUT
#define SW_SOCKET_DEFAULT_WRITE_TIMEOUT    OSW_SOCKET_DEFAULT_WRITE_TIMEOUT
#define SW_SOCKET_DEFAULT_DNS_TIMEOUT      OSW_SOCKET_DEFAULT_DNS_TIMEOUT
#define SW_DNS_HOST_BUFFER_SIZE        OSW_DNS_HOST_BUFFER_SIZE
#define SW_DNS_SERVER_PORT             OSW_DNS_SERVER_PORT
#define SW_DNS_RESOLV_CONF             OSW_DNS_RESOLV_CONF
#define SW_FILE_CHUNK_SIZE             OSW_FILE_CHUNK_SIZE
#define SW_SENDFILE_CHUNK_SIZE         OSW_SENDFILE_CHUNK_SIZE
#define SW_SENDFILE_MAXLEN             OSW_SENDFILE_MAXLEN
#define SW_TASK_TMP_DIR                OSW_TASK_TMP_DIR
#define SW_TASK_TMP_FILE               OSW_TASK_TMP_FILE
#define SW_TASK_TMP_PATH_SIZE          OSW_TASK_TMP_PATH_SIZE
#define SW_TIMER_MAX_MS                OSW_TIMER_MAX_MS
#define SW_TIMER_MAX_SEC               OSW_TIMER_MAX_SEC
#define SW_TIMER_MIN_MS                OSW_TIMER_MIN_MS
#define SW_TIMER_MIN_SEC               OSW_TIMER_MIN_SEC
#define SW_TIMER_MIN_MICRO_SEC         OSW_TIMER_MIN_MICRO_SEC
#define SW_MALLOC_TRIM_INTERVAL        OSW_MALLOC_TRIM_INTERVAL
#define SW_MALLOC_TRIM_PAD             OSW_MALLOC_TRIM_PAD
#define SW_MSGMAX                      OSW_MSGMAX

/*==========================================================================
 * SSL macros compat
 *==========================================================================*/
#define SW_SSL_BUFFER_SIZE         OSW_SSL_BUFFER_SIZE
#define SW_SSL_CIPHER_LIST         OSW_SSL_CIPHER_LIST
#define SW_SSL_ECDH_CURVE         OSW_SSL_ECDH_CURVE
#define SW_SSL_ALL                OSW_SSL_ALL
#define SW_SUPPORT_DTLS           OSW_SUPPORT_DTLS

/*==========================================================================
 * HTTP macros compat
 *==========================================================================*/
#define SW_HTTP_SERVER_SOFTWARE           OSW_HTTP_SERVER_SOFTWARE
#define SW_HTTP_HEADER_KEY_SIZE           OSW_HTTP_HEADER_KEY_SIZE
#define SW_HTTP_HEADER_MAX_SIZE           OSW_HTTP_HEADER_MAX_SIZE
#define SW_HTTP_COOKIE_KEYLEN             OSW_HTTP_COOKIE_KEYLEN
#define SW_HTTP_COOKIE_VALLEN             OSW_HTTP_COOKIE_VALLEN
#define SW_HTTP_RESPONSE_INIT_SIZE        OSW_HTTP_RESPONSE_INIT_SIZE
#define SW_HTTP_DATE_FORMAT               OSW_HTTP_DATE_FORMAT
#define SW_HTTP_ASCTIME_DATE              OSW_HTTP_ASCTIME_DATE
#define SW_HTTP_RFC1123_DATE_GMT          OSW_HTTP_RFC1123_DATE_GMT
#define SW_HTTP_RFC1123_DATE_UTC          OSW_HTTP_RFC1123_DATE_UTC
#define SW_HTTP_RFC850_DATE               OSW_HTTP_RFC850_DATE
#define SW_HTTP_UPLOAD_TMPDIR_SIZE        OSW_HTTP_UPLOAD_TMPDIR_SIZE
#define SW_HTTP_PAGE_404                  OSW_HTTP_PAGE_404
#define SW_HTTP_PARAM_MAX_NUM             OSW_HTTP_PARAM_MAX_NUM
#define SW_HTTP_SEND_TWICE                OSW_HTTP_SEND_TWICE
#define SW_HTTP_BAD_REQUEST_PACKET        OSW_HTTP_BAD_REQUEST_PACKET
#define SW_HTTP_REQUEST_ENTITY_TOO_LARGE_PACKET  OSW_HTTP_REQUEST_ENTITY_TOO_LARGE_PACKET
#define SW_HTTP_SERVICE_UNAVAILABLE_PACKET  OSW_HTTP_SERVICE_UNAVAILABLE_PACKET
#define SW_HTTP_REQUEST_TIMEOUT_PACKET    OSW_HTTP_REQUEST_TIMEOUT_PACKET
#define SW_HTTP_CLIENT_USERAGENT          OSW_HTTP_CLIENT_USERAGENT
#define SW_HTTP_CLIENT_BOUNDARY_PREKEY    OSW_HTTP_CLIENT_BOUNDARY_PREKEY
#define SW_HTTP_CLIENT_BOUNDARY_TOTAL_SIZE  OSW_HTTP_CLIENT_BOUNDARY_TOTAL_SIZE
#define SW_HTTP_FORM_RAW_DATA_FMT         OSW_HTTP_FORM_RAW_DATA_FMT
#define SW_HTTP_FORM_RAW_DATA_FMT_LEN     OSW_HTTP_FORM_RAW_DATA_FMT_LEN
#define SW_HTTP_FORM_FILE_DATA_FMT        OSW_HTTP_FORM_FILE_DATA_FMT
#define SW_HTTP_FORM_FILE_DATA_FMT_LEN    OSW_HTTP_FORM_FILE_DATA_FMT_LEN
#define SW_HTTP_FORM_KEYLEN               OSW_HTTP_FORM_KEYLEN
#define SW_HTTPS_PROXY_HANDSHAKE_RESPONSE OSW_HTTPS_PROXY_HANDSHAKE_RESPONSE
#define SW_HTTP_COMPRESSION_MIN_LENGTH_DEFAULT  OSW_HTTP_COMPRESSION_MIN_LENGTH_DEFAULT

/*==========================================================================
 * WebSocket macros compat
 *==========================================================================*/
#define SW_WEBSOCKET_VERSION              OSW_WEBSOCKET_VERSION
#define SW_WEBSOCKET_KEY_LENGTH           OSW_WEBSOCKET_KEY_LENGTH
#define SW_WEBSOCKET_GUID                 OSW_WEBSOCKET_GUID
#define SW_WEBSOCKET_HEADER_LEN           OSW_WEBSOCKET_HEADER_LEN
#define SW_WEBSOCKET_MASK_LEN             OSW_WEBSOCKET_MASK_LEN
#define SW_WEBSOCKET_MASK_DATA            OSW_WEBSOCKET_MASK_DATA
#define SW_WEBSOCKET_EXT                  OSW_WEBSOCKET_EXT
#define SW_WEBSOCKET_EXTENSION_DEFLATE    OSW_WEBSOCKET_EXTENSION_DEFLATE
#define SW_WEBSOCKET_SEC_KEY_LEN          OSW_WEBSOCKET_SEC_KEY_LEN
#define SW_WEBSOCKET_QUEUE_SIZE           OSW_WEBSOCKET_QUEUE_SIZE
#define SW_WEBSOCKET_OPCODE_MAX           OSW_WEBSOCKET_OPCODE_MAX
#define SW_WEBSOCKET_CLOSE_CODE_LEN       OSW_WEBSOCKET_CLOSE_CODE_LEN
#define SW_WEBSOCKET_CLOSE_REASON_MAX_LEN OSW_WEBSOCKET_CLOSE_REASON_MAX_LEN

/*==========================================================================
 * MQTT macros compat
 *==========================================================================*/
#define SW_MQTT_MIN_LENGTH_SIZE    OSW_MQTT_MIN_LENGTH_SIZE
#define SW_MQTT_MAX_LENGTH_SIZE    OSW_MQTT_MAX_LENGTH_SIZE
#define SW_MQTT_MAX_PAYLOAD_SIZE   OSW_MQTT_MAX_PAYLOAD_SIZE
#define SW_MQTT_SETRETAIN(flags)   OSW_MQTT_SETRETAIN(flags)
#define SW_MQTT_SETQOS(flags,q)    OSW_MQTT_SETQOS(flags,q)
#define SW_MQTT_SETDUP(flags)      OSW_MQTT_SETDUP(flags)

/*==========================================================================
 * Redis macros compat
 *==========================================================================*/
#define SW_REDIS_MAX_COMMAND_SIZE   OSW_REDIS_MAX_COMMAND_SIZE
#define SW_REDIS_MAX_LINES         OSW_REDIS_MAX_LINES
#define SW_REDIS_MAX_STRING_SIZE   OSW_REDIS_MAX_STRING_SIZE
#define SW_REDIS_RETURN_NIL        OSW_REDIS_RETURN_NIL

/*==========================================================================
 * MySQL macros compat
 *==========================================================================*/
#define SW_MYSQL_DEFAULT_HOST      OSW_MYSQL_DEFAULT_HOST
#define SW_MYSQL_DEFAULT_PORT      OSW_MYSQL_DEFAULT_PORT
#define SW_MYSQL_DEFAULT_CHARSET   OSW_MYSQL_DEFAULT_CHARSET

/*==========================================================================
 * SOCKS proxy macros compat
 *==========================================================================*/
#define SW_SOCKS5_VERSION_CODE     OSW_SOCKS5_VERSION_CODE

/*==========================================================================
 * PHP-layer macros compat (ext-src)
 *==========================================================================*/
#define SW_HAVE_COUNTABLE          OSW_HAVE_COUNTABLE
#define SW_CHECK_RETURN(s)         OSW_CHECK_RETURN(s)
#define SW_LOCK_CHECK_RETURN(s)    OSW_LOCK_CHECK_RETURN(s)
#define SW_MAX_FIND_COUNT          OSW_MAX_FIND_COUNT
#define SW_PHP_CLIENT_BUFFER_SIZE  OSW_PHP_CLIENT_BUFFER_SIZE
#define SW_FLAG_KEEP               OSW_FLAG_KEEP
#define SW_FLAG_ASYNC              OSW_FLAG_ASYNC
#define SW_FLAG_SYNC               OSW_FLAG_SYNC
#define SW_STRERROR_SYSTEM         OSW_STRERROR_SYSTEM
#define SW_STRERROR_GAI            OSW_STRERROR_GAI
#define SW_STRERROR_DNS            OSW_STRERROR_DNS
#define SW_STRERROR_SWOOLE         OSW_STRERROR_OPENSWOOLE

/*==========================================================================
 * PHP class entry macros compat
 *==========================================================================*/
#define SW_INIT_CLASS_ENTRY_BASE         OSW_INIT_CLASS_ENTRY_BASE
#define SW_INIT_CLASS_ENTRY              OSW_INIT_CLASS_ENTRY
#define SW_INIT_CLASS_ENTRY_EX           OSW_INIT_CLASS_ENTRY_EX
#define SW_INIT_CLASS_ENTRY_EX2          OSW_INIT_CLASS_ENTRY_EX2
#define SW_INIT_CLASS_ENTRY_DATA_OBJECT  OSW_INIT_CLASS_ENTRY_DATA_OBJECT
#define SW_CLASS_ALIAS                   OSW_CLASS_ALIAS
#define SW_SET_CLASS_NOT_SERIALIZABLE    OSW_SET_CLASS_NOT_SERIALIZABLE
#define SW_SET_CLASS_CLONEABLE           OSW_SET_CLASS_CLONEABLE
#define SW_SET_CLASS_UNSET_PROPERTY_HANDLER  OSW_SET_CLASS_UNSET_PROPERTY_HANDLER
#define SW_SET_CLASS_CREATE              OSW_SET_CLASS_CREATE
#define SW_SET_CLASS_DTOR                OSW_SET_CLASS_DTOR
#define SW_SET_CLASS_FREE                OSW_SET_CLASS_FREE
#define SW_SET_CLASS_CREATE_AND_FREE     OSW_SET_CLASS_CREATE_AND_FREE
#define SW_SET_CLASS_CUSTOM_OBJECT       OSW_SET_CLASS_CUSTOM_OBJECT
#define SW_PREVENT_USER_DESTRUCT         OSW_PREVENT_USER_DESTRUCT
#define SW_FUNCTION_ALIAS                OSW_FUNCTION_ALIAS
#define SW_REGISTER_NULL_CONSTANT        OSW_REGISTER_NULL_CONSTANT
#define SW_REGISTER_BOOL_CONSTANT        OSW_REGISTER_BOOL_CONSTANT
#define SW_REGISTER_LONG_CONSTANT        OSW_REGISTER_LONG_CONSTANT
#define SW_REGISTER_DOUBLE_CONSTANT      OSW_REGISTER_DOUBLE_CONSTANT
#define SW_REGISTER_STRING_CONSTANT      OSW_REGISTER_STRING_CONSTANT
#define SW_REGISTER_STRINGL_CONSTANT     OSW_REGISTER_STRINGL_CONSTANT
#define SW_RETURN_PROPERTY               OSW_RETURN_PROPERTY

/*==========================================================================
 * PHP hashtable / array macros compat
 *==========================================================================*/
#define SW_HASHTABLE_FOREACH_START       OSW_HASHTABLE_FOREACH_START
#define SW_HASHTABLE_FOREACH_START2      OSW_HASHTABLE_FOREACH_START2
#define SW_HASHTABLE_FOREACH_END         OSW_HASHTABLE_FOREACH_END
#define SW_PHP_OB_START                  OSW_PHP_OB_START
#define SW_PHP_OB_END                    OSW_PHP_OB_END

/*==========================================================================
 * PHP zval macros compat
 *==========================================================================*/
#define SW_Z8_OBJ_P                  OSW_Z8_OBJ_P
#define SW_Z_OBJCE_NAME_VAL_P        OSW_Z_OBJCE_NAME_VAL_P
#define SW_ZVAL_SOCKET               OSW_ZVAL_SOCKET
#define SW_Z_SOCKET_P                OSW_Z_SOCKET_P

/*==========================================================================
 * PHP error macros compat
 *==========================================================================*/
#define php_swoole_fatal_error       php_openswoole_fatal_error
#define php_swoole_error             php_openswoole_error
#define php_swoole_sys_error         php_openswoole_sys_error
#define SWOOLE_SOCKETS_SUPPORT       OPENSWOOLE_SOCKETS_SUPPORT

/*==========================================================================
 * Callback enum compat
 *==========================================================================*/
#define SW_CLIENT_CB_onConnect       OSW_CLIENT_CB_onConnect
#define SW_CLIENT_CB_onReceive       OSW_CLIENT_CB_onReceive
#define SW_CLIENT_CB_onClose         OSW_CLIENT_CB_onClose
#define SW_CLIENT_CB_onError         OSW_CLIENT_CB_onError
#define SW_CLIENT_CB_onBufferFull    OSW_CLIENT_CB_onBufferFull
#define SW_CLIENT_CB_onBufferEmpty   OSW_CLIENT_CB_onBufferEmpty
#define SW_CLIENT_CB_onSSLReady      OSW_CLIENT_CB_onSSLReady

/*==========================================================================
 * PHP accessor macro compat
 *==========================================================================*/
#define SWOOLE_G(v)      OPENSWOOLE_G(v)

/*==========================================================================
 * Config.m4 related compat
 *==========================================================================*/
#define ENABLE_PHP_SWOOLE   ENABLE_PHP_OPENSWOOLE

/*==========================================================================
 * Use-flag macros: config.m4 generates SW_* defines, code uses OSW_*.
 * Map in BOTH directions so either name works.
 *==========================================================================*/
/* SW_ → OSW_ (config.m4 defines SW_, code checks OSW_) */
#ifdef SW_USE_ASM_CONTEXT
#ifndef OSW_USE_ASM_CONTEXT
#define OSW_USE_ASM_CONTEXT SW_USE_ASM_CONTEXT
#endif
#endif

#ifdef SW_USE_OPENSSL
#ifndef OSW_USE_OPENSSL
#define OSW_USE_OPENSSL SW_USE_OPENSSL
#endif
#endif

#ifdef SW_USE_HTTP2
#ifndef OSW_USE_HTTP2
#define OSW_USE_HTTP2 SW_USE_HTTP2
#endif
#endif

#ifdef SW_USE_POSTGRES
#ifndef OSW_USE_POSTGRES
#define OSW_USE_POSTGRES SW_USE_POSTGRES
#endif
#endif

#ifdef SW_USE_CARES
#ifndef OSW_USE_CARES
#define OSW_USE_CARES SW_USE_CARES
#endif
#endif

#ifdef SW_SOCKETS
#ifndef OSW_SOCKETS
#define OSW_SOCKETS SW_SOCKETS
#endif
#endif

#ifdef SW_DEBUG
#ifndef OSW_DEBUG
#define OSW_DEBUG SW_DEBUG
#endif
#endif

#ifdef SW_USE_JEMALLOC
#ifndef OSW_USE_JEMALLOC
#define OSW_USE_JEMALLOC SW_USE_JEMALLOC
#endif
#endif

#ifdef SW_USE_TCMALLOC
#ifndef OSW_USE_TCMALLOC
#define OSW_USE_TCMALLOC SW_USE_TCMALLOC
#endif
#endif

#ifdef SW_USE_MONOTONIC_TIME
#ifndef OSW_USE_MONOTONIC_TIME
#define OSW_USE_MONOTONIC_TIME SW_USE_MONOTONIC_TIME
#endif
#endif

#ifdef SW_USE_MALLOC_TRIM
#ifndef OSW_USE_MALLOC_TRIM
#define OSW_USE_MALLOC_TRIM SW_USE_MALLOC_TRIM
#endif
#endif

#ifdef SW_USE_EVENTFD
#ifndef OSW_USE_EVENTFD
#define OSW_USE_EVENTFD SW_USE_EVENTFD
#endif
#endif

#ifdef SW_USE_FIBER_CONTEXT
#ifndef OSW_USE_FIBER_CONTEXT
#define OSW_USE_FIBER_CONTEXT SW_USE_FIBER_CONTEXT
#endif
#endif

#ifdef SW_USE_THREAD_CONTEXT
#ifndef OSW_USE_THREAD_CONTEXT
#define OSW_USE_THREAD_CONTEXT SW_USE_THREAD_CONTEXT
#endif
#endif

#ifdef SW_USE_IOURING
#ifndef OSW_USE_IOURING
#define OSW_USE_IOURING SW_USE_IOURING
#endif
#endif

/* OSW_ → SW_ (reverse, for any code still using old names) */
#ifdef OSW_USE_ASM_CONTEXT
#ifndef SW_USE_ASM_CONTEXT
#define SW_USE_ASM_CONTEXT OSW_USE_ASM_CONTEXT
#endif
#endif

#ifdef OSW_USE_OPENSSL
#ifndef SW_USE_OPENSSL
#define SW_USE_OPENSSL OSW_USE_OPENSSL
#endif
#endif

#ifdef OSW_USE_HTTP2
#ifndef SW_USE_HTTP2
#define SW_USE_HTTP2 OSW_USE_HTTP2
#endif
#endif

#ifdef OSW_USE_POSTGRES
#ifndef SW_USE_POSTGRES
#define SW_USE_POSTGRES OSW_USE_POSTGRES
#endif
#endif

#ifdef OSW_USE_CARES
#ifndef SW_USE_CARES
#define SW_USE_CARES OSW_USE_CARES
#endif
#endif

#ifdef OSW_SOCKETS
#ifndef SW_SOCKETS
#define SW_SOCKETS OSW_SOCKETS
#endif
#endif

#ifdef OSW_DEBUG
#ifndef SW_DEBUG
#define SW_DEBUG OSW_DEBUG
#endif
#endif

#ifdef OSW_USE_JEMALLOC
#ifndef SW_USE_JEMALLOC
#define SW_USE_JEMALLOC OSW_USE_JEMALLOC
#endif
#endif

#ifdef OSW_USE_TCMALLOC
#ifndef SW_USE_TCMALLOC
#define SW_USE_TCMALLOC OSW_USE_TCMALLOC
#endif
#endif

#ifdef OSW_USE_MONOTONIC_TIME
#ifndef SW_USE_MONOTONIC_TIME
#define SW_USE_MONOTONIC_TIME OSW_USE_MONOTONIC_TIME
#endif
#endif

#ifdef OSW_USE_MALLOC_TRIM
#ifndef SW_USE_MALLOC_TRIM
#define SW_USE_MALLOC_TRIM OSW_USE_MALLOC_TRIM
#endif
#endif

#ifdef OSW_USE_EVENTFD
#ifndef SW_USE_EVENTFD
#define SW_USE_EVENTFD OSW_USE_EVENTFD
#endif
#endif

#ifdef OSW_USE_FIBER_CONTEXT
#ifndef SW_USE_FIBER_CONTEXT
#define SW_USE_FIBER_CONTEXT OSW_USE_FIBER_CONTEXT
#endif
#endif

#ifdef OSW_USE_THREAD_CONTEXT
#ifndef SW_USE_THREAD_CONTEXT
#define SW_USE_THREAD_CONTEXT OSW_USE_THREAD_CONTEXT
#endif
#endif

#ifdef OSW_USE_IOURING
#ifndef SW_USE_IOURING
#define SW_USE_IOURING OSW_USE_IOURING
#endif
#endif

/*==========================================================================
 * swoole_* function name compat (these are actual C/C++ functions)
 *==========================================================================*/
#define swoole_strdup        openswoole_strdup
#define swoole_strndup       openswoole_strndup
#define swoole_strlcpy       openswoole_strlcpy
#define swoole_strlchr       openswoole_strlchr
#define swoole_strnstr       openswoole_strnstr
#define swoole_strnpos       openswoole_strnpos
#define swoole_strrnpos      openswoole_strrnpos
#define swoole_strtolower    openswoole_strtolower
#define swoole_tolower       openswoole_tolower
#define swoole_toupper       openswoole_toupper
#define swoole_streq         openswoole_streq
#define swoole_strcaseeq     openswoole_strcaseeq
#define swoole_strct         openswoole_strct
#define swoole_strcasect     openswoole_strcasect
#define swoole_size_align    openswoole_size_align
#define swoole_random_string openswoole_random_string
#define swoole_random_bytes  openswoole_random_bytes
#define swoole_common_multiple  openswoole_common_multiple
#define swoole_common_divisor   openswoole_common_divisor
#define swoole_itoa          openswoole_itoa
#define swoole_mkdir_recursive  openswoole_mkdir_recursive
#define swoole_rand          openswoole_rand
#define swoole_system_random openswoole_system_random
#define swoole_version_compare  openswoole_version_compare
#define swoole_print_backtrace  openswoole_print_backtrace
#define swoole_string_format    openswoole_string_format
#define swoole_get_env       openswoole_get_env
#define swoole_get_systemd_listen_fds  openswoole_get_systemd_listen_fds
#define swoole_init          openswoole_init
#define swoole_clean         openswoole_clean
#define swoole_fork          openswoole_fork
#define swoole_redirect_stdout  openswoole_redirect_stdout
#define swoole_shell_exec    openswoole_shell_exec
#define swoole_daemon        openswoole_daemon
#define swoole_set_task_tmpdir  openswoole_set_task_tmpdir
#define swoole_tmpfile       openswoole_tmpfile
#define swoole_set_cpu_affinity  openswoole_set_cpu_affinity
#define swoole_clock_gettime openswoole_clock_gettime
#define swoole_time_until    openswoole_time_until
#define swoole_set_last_error   openswoole_set_last_error
#define swoole_get_last_error   openswoole_get_last_error
#define swoole_get_thread_id    openswoole_get_thread_id
#define swoole_get_process_type openswoole_get_process_type
#define swoole_get_process_id   openswoole_get_process_id
#define swoole_strerror      openswoole_strerror
#define swoole_throw_error   openswoole_throw_error
#define swoole_set_log_level openswoole_set_log_level
#define swoole_set_trace_flags  openswoole_set_trace_flags
#define swoole_set_dns_server   openswoole_set_dns_server
#define swoole_set_hosts_path   openswoole_set_hosts_path
#define swoole_get_dns_server   openswoole_get_dns_server
#define swoole_load_resolv_conf openswoole_load_resolv_conf

/*==========================================================================
 * PHP function compat (php_swoole_* → php_openswoole_*)
 *==========================================================================*/
#define php_swoole_fatal_error       php_openswoole_fatal_error
#define php_swoole_error             php_openswoole_error
#define php_swoole_sys_error         php_openswoole_sys_error
#define php_swoole_array_length      php_openswoole_array_length
#define php_swoole_array_get_value   php_openswoole_array_get_value
#define php_swoole_array_get_value_ex  php_openswoole_array_get_value_ex
#define php_swoole_array_length_safe php_openswoole_array_length_safe
#define php_swoole_sha1              php_openswoole_sha1
#define php_swoole_sha256            php_openswoole_sha256
#define php_swoole_websocket_frame_is_object  php_openswoole_websocket_frame_is_object
#define php_swoole_get_send_data     php_openswoole_get_send_data
#define php_swoole_convert_to_fd     php_openswoole_convert_to_fd
#define php_swoole_convert_to_fd_ex  php_openswoole_convert_to_fd_ex
#define php_swoole_convert_to_socket php_openswoole_convert_to_socket
#define php_swoole_signal_isset_handler  php_openswoole_signal_isset_handler
#define php_swoole_socktype          php_openswoole_socktype

/*==========================================================================
 * PHP class entry variable compat (swoole_*_ce → openswoole_*_ce)
 *==========================================================================*/
#define swoole_event_ce              openswoole_event_ce
#define swoole_timer_ce              openswoole_timer_ce
#define swoole_socket_coro_ce        openswoole_socket_coro_ce
#define swoole_client_ce             openswoole_client_ce
#define swoole_server_ce             openswoole_server_ce
#define swoole_server_handlers       openswoole_server_handlers
#define swoole_connection_iterator_ce  openswoole_connection_iterator_ce
#define swoole_process_ce            openswoole_process_ce
#define swoole_http_server_ce        openswoole_http_server_ce
#define swoole_http_server_handlers  openswoole_http_server_handlers
#define swoole_websocket_server_ce   openswoole_websocket_server_ce
#define swoole_websocket_frame_ce    openswoole_websocket_frame_ce
#define swoole_server_port_ce        openswoole_server_port_ce
#define swoole_exception_ce          openswoole_exception_ce
#define swoole_exception_handlers    openswoole_exception_handlers
#define swoole_error_ce              openswoole_error_ce

/*==========================================================================
 * sw_zend_* helper function compat
 *==========================================================================*/
#define sw_zend_bailout              osw_zend_bailout
#define sw_malloc_zval               osw_malloc_zval
#define sw_zval_dup                  osw_zval_dup
#define sw_zval_free                 osw_zval_free
#define sw_copy_to_stack             osw_copy_to_stack
#define sw_zend_string_recycle       osw_zend_string_recycle
#define sw_zend_register_function_alias  osw_zend_register_function_alias
#define sw_zend_register_class_alias osw_zend_register_class_alias
#define sw_zend_create_object        osw_zend_create_object
#define sw_zend_create_object_deny   osw_zend_create_object_deny
#define sw_zend_class_unset_property_deny  osw_zend_class_unset_property_deny
#define sw_zend_read_property        osw_zend_read_property
#define sw_zend_update_property_null_ex  osw_zend_update_property_null_ex
#define sw_zend_read_property_ex     osw_zend_read_property_ex
#define sw_zend_read_property_not_null    osw_zend_read_property_not_null
#define sw_zend_read_property_not_null_ex osw_zend_read_property_not_null_ex
#define sw_zend_update_and_read_property_array  osw_zend_update_and_read_property_array
#define sw_php_math_round            osw_php_math_round
#define sw_zend_class_clone_deny     osw_zend_class_clone_deny

/*==========================================================================
 * PHP MINIT/RINIT/RSHUTDOWN function name compat
 *==========================================================================*/
#define php_swoole_event_minit       php_openswoole_event_minit
#define php_swoole_util_minit        php_openswoole_util_minit
#define php_swoole_atomic_minit      php_openswoole_atomic_minit
#define php_swoole_lock_minit        php_openswoole_lock_minit
#define php_swoole_process_minit     php_openswoole_process_minit
#define php_swoole_process_pool_minit  php_openswoole_process_pool_minit
#define php_swoole_table_minit       php_openswoole_table_minit
#define php_swoole_timer_minit       php_openswoole_timer_minit
#define php_swoole_coroutine_minit   php_openswoole_coroutine_minit
#define php_swoole_coroutine_system_minit  php_openswoole_coroutine_system_minit
#define php_swoole_coroutine_scheduler_minit  php_openswoole_coroutine_scheduler_minit
#define php_swoole_channel_coro_minit  php_openswoole_channel_coro_minit
#define php_swoole_runtime_minit     php_openswoole_runtime_minit
#define php_swoole_socket_coro_minit php_openswoole_socket_coro_minit
#define php_swoole_client_minit      php_openswoole_client_minit
#define php_swoole_client_coro_minit php_openswoole_client_coro_minit
#define php_swoole_http_client_coro_minit  php_openswoole_http_client_coro_minit
#define php_swoole_http2_client_coro_minit php_openswoole_http2_client_coro_minit
#define php_swoole_server_minit      php_openswoole_server_minit
#define php_swoole_server_port_minit php_openswoole_server_port_minit
#define php_swoole_http_request_minit  php_openswoole_http_request_minit
#define php_swoole_http_response_minit php_openswoole_http_response_minit
#define php_swoole_http_server_minit php_openswoole_http_server_minit
#define php_swoole_websocket_server_minit  php_openswoole_websocket_server_minit
#define php_swoole_postgresql_coro_minit   php_openswoole_postgresql_coro_minit
#define php_swoole_coroutine_rinit   php_openswoole_coroutine_rinit
#define php_swoole_runtime_rinit     php_openswoole_runtime_rinit
#define php_swoole_async_coro_rshutdown  php_openswoole_async_coro_rshutdown
#define php_swoole_coroutine_rshutdown   php_openswoole_coroutine_rshutdown
#define php_swoole_runtime_rshutdown php_openswoole_runtime_rshutdown
#define php_swoole_server_rshutdown  php_openswoole_server_rshutdown
#define php_swoole_reactor_init      php_openswoole_reactor_init
#define php_swoole_set_global_option php_openswoole_set_global_option
#define php_swoole_set_coroutine_option  php_openswoole_set_coroutine_option
#define php_swoole_set_aio_option    php_openswoole_set_aio_option
#define php_swoole_register_shutdown_function  php_openswoole_register_shutdown_function
#define php_swoole_register_shutdown_function_prepend  php_openswoole_register_shutdown_function_prepend
#define php_swoole_event_init        php_openswoole_event_init
#define php_swoole_event_wait        php_openswoole_event_wait
#define php_swoole_event_exit        php_openswoole_event_exit
#define php_swoole_runtime_mshutdown php_openswoole_runtime_mshutdown

/*==========================================================================
 * PHP request status enum compat
 *==========================================================================*/
#define PHP_SWOOLE_RINIT_BEGIN                     PHP_OPENSWOOLE_RINIT_BEGIN
#define PHP_SWOOLE_RINIT_END                       PHP_OPENSWOOLE_RINIT_END
#define PHP_SWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN     PHP_OPENSWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN
#define PHP_SWOOLE_RSHUTDOWN_BEGIN                 PHP_OPENSWOOLE_RSHUTDOWN_BEGIN
#define PHP_SWOOLE_RSHUTDOWN_END                   PHP_OPENSWOOLE_RSHUTDOWN_END

/*==========================================================================
 * PHP FD type enum compat
 *==========================================================================*/
#define PHP_SWOOLE_FD_STREAM_CLIENT    PHP_OPENSWOOLE_FD_STREAM_CLIENT
#define PHP_SWOOLE_FD_DGRAM_CLIENT     PHP_OPENSWOOLE_FD_DGRAM_CLIENT
#define PHP_SWOOLE_FD_MYSQL            PHP_OPENSWOOLE_FD_MYSQL
#define PHP_SWOOLE_FD_REDIS            PHP_OPENSWOOLE_FD_REDIS
#define PHP_SWOOLE_FD_HTTPCLIENT       PHP_OPENSWOOLE_FD_HTTPCLIENT
#define PHP_SWOOLE_FD_PROCESS_STREAM   PHP_OPENSWOOLE_FD_PROCESS_STREAM
#define PHP_SWOOLE_FD_MYSQL_CORO       PHP_OPENSWOOLE_FD_MYSQL_CORO
#define PHP_SWOOLE_FD_REDIS_CORO       PHP_OPENSWOOLE_FD_REDIS_CORO
#define PHP_SWOOLE_FD_POSTGRESQL       PHP_OPENSWOOLE_FD_POSTGRESQL
#define PHP_SWOOLE_FD_SOCKET           PHP_OPENSWOOLE_FD_SOCKET
#define PHP_SWOOLE_FD_CO_CURL          PHP_OPENSWOOLE_FD_CO_CURL

/*==========================================================================
 * Header guard compat
 *==========================================================================*/
#define SWOOLE_CONFIG_H_       OPENSWOOLE_CONFIG_H_
#define SWOOLE_LOG_H_          OPENSWOOLE_LOG_H_
#define SWOOLE_ATOMIC_H_       OPENSWOOLE_ATOMIC_H_
#define SWOOLE_ERROR_H_        OPENSWOOLE_ERROR_H_
#define SW_HASH_H_             OSW_HASH_H_
#define SW_C_API_H_            OSW_C_API_H_
#define SW_COROUTINE_API_H_    OSW_COROUTINE_API_H_
#define SW_FILE_HOOK_H_        OSW_FILE_HOOK_H_
#define SW_SOCKET_HOOK_H_      OSW_SOCKET_HOOK_H_
#define PHP_SWOOLE_PRIVATE_H   PHP_OPENSWOOLE_PRIVATE_H
#define PHP_SWOOLE_H           PHP_OPENSWOOLE_H
