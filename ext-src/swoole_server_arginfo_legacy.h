/* This is a generated file, edit the .stub.php file instead.
* Stub hash: 00ca786fb52e1c3217a4480fdfc3b9de326dbb3c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server___construct, 0, 0, 1)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, mode)
ZEND_ARG_INFO(0, sockType)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_listen, 0, 0, 3)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, sockType)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_addlistener arginfo_class_Swoole_Server_listen

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_on, 0, 0, 2)
ZEND_ARG_INFO(0, event)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getCallback, 0, 0, 1)
ZEND_ARG_INFO(0, event)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_set, 0, 0, 1)
ZEND_ARG_INFO(0, settings)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_start arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_send, 0, 0, 2)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, serverSocket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendto, 0, 0, 3)
ZEND_ARG_INFO(0, ip)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, serverSocket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendwait, 0, 0, 2)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_exists, 0, 0, 1)
ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_exist arginfo_class_Swoole_Server_exists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_protect, 0, 0, 1)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, isProtected)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendfile, 0, 0, 2)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, fileName)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_close, 0, 0, 1)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, reset)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_confirm arginfo_class_Swoole_Server_exists

#define arginfo_class_Swoole_Server_pause arginfo_class_Swoole_Server_exists

#define arginfo_class_Swoole_Server_resume arginfo_class_Swoole_Server_exists

#define arginfo_class_Swoole_Server_reload arginfo_class_Swoole_Server___destruct

#define arginfo_class_Swoole_Server_shutdown arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_task, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, workerId)
ZEND_ARG_INFO(0, finishCallback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_taskwait, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, timeout)
ZEND_ARG_INFO(0, workerId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_taskWaitMulti, 0, 0, 1)
ZEND_ARG_INFO(0, tasks)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_taskCo arginfo_class_Swoole_Server_taskWaitMulti

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_finish, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_stop, 0, 0, 1)
ZEND_ARG_INFO(0, workerId)
ZEND_ARG_INFO(0, waitEvent)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_getLastError arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_heartbeat, 0, 0, 0)
ZEND_ARG_INFO(0, closeConn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getClientInfo, 0, 0, 1)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, reactorId)
ZEND_ARG_INFO(0, noCheckConn)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getClientList, 0, 0, 0)
ZEND_ARG_INFO(0, startFd)
ZEND_ARG_INFO(0, pageSize)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_getWorkerId arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getWorkerPid, 0, 0, 0)
ZEND_ARG_INFO(0, workerId)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_getWorkerStatus arginfo_class_Swoole_Server_getWorkerPid

#define arginfo_class_Swoole_Server_getManagerPid arginfo_class_Swoole_Server___destruct

#define arginfo_class_Swoole_Server_getMasterPid arginfo_class_Swoole_Server___destruct

#define arginfo_class_Swoole_Server_connection_info arginfo_class_Swoole_Server_getClientInfo

#define arginfo_class_Swoole_Server_connection_list arginfo_class_Swoole_Server_getClientList

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_sendMessage, 0, 0, 2)
ZEND_ARG_INFO(0, message)
ZEND_ARG_INFO(0, workerId)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_command, 0, 0, 4)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, processId)
ZEND_ARG_INFO(0, processType)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, jsonEncode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_addProcess, 0, 0, 1)
ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_addCommand, 0, 0, 3)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, acceptedProcessTypes)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_stats, 0, 0, 0)
ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_getSocket, 0, 0, 0)
ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_bind, 0, 0, 2)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, uid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_after, 0, 0, 2)
ZEND_ARG_INFO(0, ms)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_tick arginfo_class_Swoole_Server_after

#define arginfo_class_Swoole_Server_clearTimer arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_defer, 0, 0, 1)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_Task_finish arginfo_class_Swoole_Server_finish

#define arginfo_class_Swoole_Server_Task_pack arginfo_class_Swoole_Server_finish

#define arginfo_class_Swoole_Connection_Iterator___construct arginfo_class_Swoole_Server___destruct

#define arginfo_class_Swoole_Connection_Iterator___destruct arginfo_class_Swoole_Server___destruct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Connection_Iterator_rewind, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Connection_Iterator_valid arginfo_class_Swoole_Connection_Iterator_rewind

#define arginfo_class_Swoole_Connection_Iterator_key arginfo_class_Swoole_Connection_Iterator_rewind

#define arginfo_class_Swoole_Connection_Iterator_current arginfo_class_Swoole_Connection_Iterator_rewind

#define arginfo_class_Swoole_Connection_Iterator_next arginfo_class_Swoole_Connection_Iterator_rewind

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetGet, 0, 0, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Connection_Iterator_offsetSet, 0, 0, 2)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Connection_Iterator_offsetUnset arginfo_class_Swoole_Connection_Iterator_offsetGet

#define arginfo_class_Swoole_Connection_Iterator_offsetExists arginfo_class_Swoole_Connection_Iterator_offsetGet

#define arginfo_class_Swoole_Connection_Iterator_count arginfo_class_Swoole_Connection_Iterator_rewind
