
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_coroutine_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_set, 0, 0, 1)
ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_create, 0, 0, 1)
ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_cancel, 0, 0, 1)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_resume, 0, 0, 1)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_exists, 0, 0, 1)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getContext, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_defer, 0, 0, 1)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getBackTrace, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_ARG_INFO(0, options)
ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_printBackTrace, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_ARG_INFO(0, options)
ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getPcid, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getElapsed, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getStackUsage, 0, 0, 0)
ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_getOptions arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_yield arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_isCanceled arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_stats arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_getCid arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_list arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_enableScheduler arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_disableScheduler arginfo_swoole_coroutine_void
#define arginfo_class_Swoole_Coroutine_clearDNSCache arginfo_swoole_coroutine_void

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_exec, 0, 0, 1)
ZEND_ARG_INFO(0, command)
ZEND_ARG_INFO(0, get_error_stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_sleep, 0, 0, 1)
ZEND_ARG_INFO(0, seconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_usleep, 0, 0, 1)
ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_gethostbyname, 0, 0, 1)
ZEND_ARG_INFO(0, domain_name)
ZEND_ARG_INFO(0, family)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_dnsLookup, 0, 0, 1)
ZEND_ARG_INFO(0, domain_name)
ZEND_ARG_INFO(0, timeout)
ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_getaddrinfo, 0, 0, 1)
ZEND_ARG_INFO(0, hostname)
ZEND_ARG_INFO(0, family)
ZEND_ARG_INFO(0, socktype)
ZEND_ARG_INFO(0, protocol)
ZEND_ARG_INFO(0, service)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_readFile, 0, 0, 1)
ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_writeFile, 0, 0, 2)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_statvfs, 0, 0, 1)
ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_wait, 0, 0, 0)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_waitPid, 0, 0, 1)
ZEND_ARG_INFO(0, pid)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_waitSignal, 0, 0, 1)
ZEND_ARG_INFO(0, signo)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_waitEvent, 0, 0, 1)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, events)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_select, 0, 0, 1)
ZEND_ARG_INFO(0, read)
ZEND_ARG_INFO(0, write)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_run, 0, 0, 1)
ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()
