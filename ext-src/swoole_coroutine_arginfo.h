/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 562598c7e30348d3e74ef04a64cbe30140ec3e5d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_create, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, params, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_set, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_getOptions, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_exists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, cid, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_resume arginfo_class_Swoole_Coroutine_exists

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_defer, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_yield, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_suspend arginfo_class_Swoole_Coroutine_yield

#define arginfo_class_Swoole_Coroutine_cancel arginfo_class_Swoole_Coroutine_exists

#define arginfo_class_Swoole_Coroutine_isCanceled arginfo_class_Swoole_Coroutine_yield

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_stats, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_getCid, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_getPcid, 0, 0, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cid, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Swoole_Coroutine_getContext, 0, 0, Swoole\\Coroutine\\Context, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cid, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_getBackTrace, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cid, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "DEBUG_BACKTRACE_PROVIDE_OBJECT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_printBackTrace, 0, 0, MAY_BE_FALSE|MAY_BE_VOID)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cid, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "DEBUG_BACKTRACE_PROVIDE_OBJECT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, limit, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_getElapsed arginfo_class_Swoole_Coroutine_getPcid

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_getStackUsage, 0, 0, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cid, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Swoole_Coroutine_list, 0, 0, Swoole\\Coroutine\\Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_enableScheduler arginfo_class_Swoole_Coroutine_yield

#define arginfo_class_Swoole_Coroutine_disableScheduler arginfo_class_Swoole_Coroutine_yield

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_exec, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, get_error_stream, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_sleep, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_statvfs, 0, 1, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_readFile, 0, 1, MAY_BE_FALSE|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_writeFile, 0, 2, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_wait, 0, 0, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_waitPid, 0, 1, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pid, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_waitSignal, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, signalNum, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_waitEvent, 0, 2, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, fd, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, events, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_fread, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, handle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_fgets, 0, 1, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, handle, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_fwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, handle, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_gethostbyname, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, family, IS_LONG, 0, "AF_INET")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_dnsLookup, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "5")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_getaddrinfo, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, family, IS_LONG, 0, "AF_INET")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sockType, IS_LONG, 0, "SOCK_STREAM")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, protocol, IS_LONG, 0, "STREAM_IPPROTO_TCP")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, service, IS_STRING, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_coroutine_void, 0, 0, 0)
ZEND_END_ARG_INFO()