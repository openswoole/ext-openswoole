
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_process_pool_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool___construct, 0, 0, 1)
ZEND_ARG_INFO(0, worker_num)
ZEND_ARG_INFO(0, ipc_type)
ZEND_ARG_INFO(0, msgqueue_key)
ZEND_ARG_INFO(0, enable_coroutine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool_set, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool_on, 0, 0, 2)
ZEND_ARG_INFO(0, event_name)
ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool_getProcess, 0, 0, 0)
ZEND_ARG_INFO(0, worker_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool_listen, 0, 0, 1)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_Pool_write, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Process_Pool___destruct arginfo_swoole_process_pool_void
#define arginfo_class_Swoole_Process_Pool_detach arginfo_swoole_process_pool_void
#define arginfo_class_Swoole_Process_Pool_start arginfo_swoole_process_pool_void
#define arginfo_class_Swoole_Process_Pool_stop arginfo_swoole_process_pool_void
#define arginfo_class_Swoole_Process_Pool_shutdown arginfo_swoole_process_pool_void
