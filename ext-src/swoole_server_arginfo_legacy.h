
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, sock_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_send, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, send_data)
    ZEND_ARG_INFO(0, server_socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendwait, 0, 0, 2)
    ZEND_ARG_INFO(0, conn_fd)
    ZEND_ARG_INFO(0, send_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_exists, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_protect, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, is_protected)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendto, 0, 0, 3)
    ZEND_ARG_INFO(0, ip)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, send_data)
    ZEND_ARG_INFO(0, server_socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendfile, 0, 0, 2)
    ZEND_ARG_INFO(0, conn_fd)
    ZEND_ARG_INFO(0, filename)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_close, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, reset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_pause, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_resume, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

#ifdef SWOOLE_SOCKETS_SUPPORT
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getSocket, 0, 0, 0)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_on, 0, 0, 2)
    ZEND_ARG_INFO(0, event_name)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_handle, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_setHandler, 0, 0, 1)
    ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getCallback, 0, 0, 1)
    ZEND_ARG_INFO(0, event_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_listen, 0, 0, 3)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, sock_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_task, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, worker_id)
    ZEND_ARG_CALLABLE_INFO(0, finish_callback, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_taskwait, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, timeout)
    ZEND_ARG_INFO(0, worker_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_taskCo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, tasks, 0)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_taskWaitMulti, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, tasks, 0)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_finish, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_Task_pack, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_heartbeat, 0, 0, 1)
    ZEND_ARG_INFO(0, reactor_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_stop, 0, 0, 0)
    ZEND_ARG_INFO(0, worker_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_bind, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, uid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendMessage, 0, 0, 2)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, dst_worker_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_addProcess, 0, 0, 1)
    ZEND_ARG_OBJ_INFO(0, process, swoole_process, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getClientInfo, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, reactor_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getWorkerStatus, 0, 0, 0)
    ZEND_ARG_INFO(0, worker_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getClientList, 0, 0, 1)
    ZEND_ARG_INFO(0, start_fd)
    ZEND_ARG_INFO(0, find_count)
ZEND_END_ARG_INFO()

//arginfo_swoole_connection_iterator_

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_current, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Connection_Iterator_key arginfo_class_Swoole_Connection_Iterator_current

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_valid, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_next, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Connection_Iterator_rewind  arginfo_class_Swoole_Connection_Iterator_next

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetExists, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetGet, 0, 1, IS_MIXED, 0)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetSet, 0, 2, IS_VOID, 0)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetUnset, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server___destruct arginfo_swoole_void
#define arginfo_class_Swoole_Server_start arginfo_swoole_void
#define arginfo_class_Swoole_Server_confirm arginfo_class_Swoole_Server_exists
#define arginfo_class_Swoole_Server_pause arginfo_class_Swoole_Server_exists
#define arginfo_class_Swoole_Server_resume arginfo_class_Swoole_Server_exists
#define arginfo_class_Swoole_Server_reload arginfo_swoole_void
#define arginfo_class_Swoole_Server_shutdown arginfo_swoole_void
#define arginfo_class_Swoole_Server_getLastError arginfo_swoole_void
#define arginfo_class_Swoole_Server_getWorkerId arginfo_swoole_void
#define arginfo_class_Swoole_Server_getWorkerPid arginfo_swoole_void
#define arginfo_class_Swoole_Server_getManagerPid arginfo_swoole_void
#define arginfo_class_Swoole_Server_getMasterPid arginfo_swoole_void
#define arginfo_class_Swoole_Server_stats arginfo_swoole_void
#define arginfo_class_Swoole_Connection_Iterator___destruct arginfo_swoole_void
#define arginfo_class_Swoole_Connection_Iterator___construct arginfo_swoole_void
