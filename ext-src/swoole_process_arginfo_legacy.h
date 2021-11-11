
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process___construct, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
    ZEND_ARG_INFO(0, redirect_stdin_and_stdout)
    ZEND_ARG_INFO(0, pipe_type)
    ZEND_ARG_INFO(0, enable_coroutine)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_process_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_wait, 0, 0, 0)
    ZEND_ARG_INFO(0, blocking)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_signal, 0, 0, 2)
    ZEND_ARG_INFO(0, signal_no)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_alarm, 0, 0, 1)
    ZEND_ARG_INFO(0, usec)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_kill, 0, 0, 1)
    ZEND_ARG_INFO(0, pid)
    ZEND_ARG_INFO(0, signal_no)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_daemon, 0, 0, 0)
    ZEND_ARG_INFO(0, nochdir)
    ZEND_ARG_INFO(0, noclose)
    ZEND_ARG_INFO(0, pipes)
ZEND_END_ARG_INFO()

#ifdef HAVE_CPU_AFFINITY
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_setAffinity, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, cpu_settings, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_setPriority, 0, 0, 2)
    ZEND_ARG_INFO(0, which)
    ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_getPriority, 0, 0, 1)
    ZEND_ARG_INFO(0, which)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_setTimeout, 0, 0, 1)
    ZEND_ARG_INFO(0, seconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_setBlocking, 0, 0, 1)
    ZEND_ARG_INFO(0, blocking)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_useQueue, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_write, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_read, 0, 0, 0)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_push, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_pop, 0, 0, 0)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_exit, 0, 0, 0)
    ZEND_ARG_INFO(0, exit_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Process_exec, 0, 0, 2)
    ZEND_ARG_INFO(0, exec_file)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_process_name, 0, 0, 1)
    ZEND_ARG_INFO(0, process_name)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Process___destruct arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_statQueue arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_freeQueue arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_start arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_write arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_close arginfo_swoole_process_void
#define arginfo_class_Swoole_Process_exportSocket arginfo_swoole_process_void
