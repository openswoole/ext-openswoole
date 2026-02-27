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
*/

#include "php_openswoole_private.h"
#include "openswoole_mime_type.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>

#if defined(__MACH__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <net/if_dl.h>
#endif

#include "openswoole_util_arginfo.h"

zend_class_entry *openswoole_util_ce;
static zend_object_handlers openswoole_util_handlers;

OSW_EXTERN_C_BEGIN
static PHP_METHOD(openswoole_util, getVersion);
static PHP_METHOD(openswoole_util, getCPUNum);
static PHP_METHOD(openswoole_util, getLocalIp);
static PHP_METHOD(openswoole_util, getLocalMac);
static PHP_METHOD(openswoole_util, getLastErrorCode);
static PHP_METHOD(openswoole_util, getErrorMessage);
static PHP_METHOD(openswoole_util, errorCode);
static PHP_METHOD(openswoole_util, clearError);
static PHP_METHOD(openswoole_util, log);
static PHP_METHOD(openswoole_util, hashcode);
static PHP_METHOD(openswoole_util, mimeTypeAdd);
static PHP_METHOD(openswoole_util, mimeTypeSet);
static PHP_METHOD(openswoole_util, mimeTypeDel);
static PHP_METHOD(openswoole_util, mimeTypeGet);
static PHP_METHOD(openswoole_util, mimeTypeList);
static PHP_METHOD(openswoole_util, mimeTypeExists);
static PHP_METHOD(openswoole_util, setProcessName);
static PHP_METHOD(openswoole_util, setAio);

OSW_EXTERN_C_END

// clang-format off
static const zend_function_entry openswoole_util_methods[] = {
  PHP_ME(openswoole_util, getVersion, arginfo_class_OpenSwoole_Util_getVersion, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, getCPUNum, arginfo_class_OpenSwoole_Util_getCPUNum, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, getLocalIp, arginfo_class_OpenSwoole_Util_getLocalIp, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, getLocalMac, arginfo_class_OpenSwoole_Util_getLocalMac, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, getLastErrorCode, arginfo_class_OpenSwoole_Util_getLastErrorCode, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, getErrorMessage, arginfo_class_OpenSwoole_Util_getErrorMessage, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, errorCode, arginfo_class_OpenSwoole_Util_errorCode, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, clearError, arginfo_class_OpenSwoole_Util_clearError, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, log, arginfo_class_OpenSwoole_Util_log, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, hashcode, arginfo_class_OpenSwoole_Util_hashcode, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeAdd, arginfo_class_OpenSwoole_Util_mimeTypeAdd, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeSet, arginfo_class_OpenSwoole_Util_mimeTypeSet, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeDel, arginfo_class_OpenSwoole_Util_mimeTypeDel, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeGet, arginfo_class_OpenSwoole_Util_mimeTypeGet, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeList, arginfo_class_OpenSwoole_Util_mimeTypeList, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, mimeTypeExists, arginfo_class_OpenSwoole_Util_mimeTypeExists, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, setProcessName, arginfo_class_OpenSwoole_Util_setProcessName, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(openswoole_util, setAio, arginfo_class_OpenSwoole_Util_setAio, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_FE_END
};
// clang-format on

void php_openswoole_util_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY(openswoole_util, "OpenSwoole\\Util", nullptr, nullptr, openswoole_util_methods);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_util);
    OSW_SET_CLASS_CLONEABLE(openswoole_util, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_util, osw_zend_class_unset_property_deny);
}

static PHP_METHOD(openswoole_util, getVersion) {
    RETURN_STRING(OPENSWOOLE_VERSION);
}

static PHP_METHOD(openswoole_util, getCPUNum) {
    RETURN_LONG(OSW_CPU_NUM);
}

static uint32_t hashkit_one_at_a_time(const char *key, size_t key_length) {
    const char *ptr = key;
    uint32_t value = 0;

    while (key_length--) {
        uint32_t val = (uint32_t) *ptr++;
        value += val;
        value += (value << 10);
        value ^= (value >> 6);
    }
    value += (value << 3);
    value ^= (value >> 11);
    value += (value << 15);

    return value;
}

static PHP_METHOD(openswoole_util, hashcode) {
    char *data;
    size_t l_data;
    zend_long type = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(data, l_data)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    switch (type) {
    case 0:
        RETURN_LONG(zend_hash_func(data, l_data));
    case 1:
        RETURN_LONG(hashkit_one_at_a_time(data, l_data));
    default:
        RETURN_FALSE;
    }
}

static PHP_METHOD(openswoole_util, clearError) {
    openswoole_set_last_error(0);
}

static PHP_METHOD(openswoole_util, errorCode) {
    RETURN_LONG(errno);
}

static PHP_METHOD(openswoole_util, getLastErrorCode) {
    RETURN_LONG(openswoole_get_last_error());
}

static PHP_METHOD(openswoole_util, getErrorMessage) {
    zend_long openswoole_errno;
    zend_long error_type = OSW_STRERROR_SYSTEM;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_LONG(openswoole_errno)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(error_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (error_type == OSW_STRERROR_GAI) {
        RETURN_STRING(gai_strerror(openswoole_errno));
    } else if (error_type == OSW_STRERROR_DNS) {
        RETURN_STRING(hstrerror(openswoole_errno));
    } else if (error_type == OSW_STRERROR_SWOOLE || (openswoole_errno > OSW_ERROR_BEGIN && openswoole_errno < OSW_ERROR_END)) {
        RETURN_STRING(openswoole_strerror(openswoole_errno));
    } else {
        RETURN_STRING(strerror(openswoole_errno));
    }
}

static PHP_METHOD(openswoole_util, setProcessName) {
    zend_function *cli_set_process_title =
        (zend_function *) zend_hash_str_find_ptr(EG(function_table), ZEND_STRL("cli_set_process_title"));
    if (!cli_set_process_title) {
        php_openswoole_fatal_error(E_WARNING, "openswoole_set_process_name only support in CLI mode");
        RETURN_FALSE;
    }
    cli_set_process_title->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_METHOD(openswoole_util, log) {
    char *msg;
    size_t l_msg;
    zend_long level = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(level)
    Z_PARAM_STRING(msg, l_msg)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    osw_logger()->put(level, msg, l_msg);
}

static PHP_METHOD(openswoole_util, getLocalIp) {
    struct sockaddr_in *s4;
    struct ifaddrs *ipaddrs, *ifa;
    void *in_addr;
    char ip[64];

    if (getifaddrs(&ipaddrs) != 0) {
        php_openswoole_sys_error(E_WARNING, "getifaddrs() failed");
        RETURN_FALSE;
    }
    array_init(return_value);
    for (ifa = ipaddrs; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || !(ifa->ifa_flags & IFF_UP)) {
            continue;
        }

        switch (ifa->ifa_addr->sa_family) {
        case AF_INET:
            s4 = (struct sockaddr_in *) ifa->ifa_addr;
            in_addr = &s4->sin_addr;
            break;
        case AF_INET6:
            // struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            // in_addr = &s6->sin6_addr;
            continue;
        default:
            continue;
        }
        if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, ip, sizeof(ip))) {
            php_error_docref(nullptr, E_WARNING, "%s: inet_ntop failed", ifa->ifa_name);
        } else {
            // if (ifa->ifa_addr->sa_family == AF_INET && ntohl(((struct in_addr *) in_addr)->s_addr) ==
            // INADDR_LOOPBACK)
            if (strcmp(ip, "127.0.0.1") == 0) {
                continue;
            }
            add_assoc_string(return_value, ifa->ifa_name, ip);
        }
    }
    freeifaddrs(ipaddrs);
}

static PHP_METHOD(openswoole_util, getLocalMac) {
    auto add_assoc_address = [](zval *zv, const char *name, const unsigned char *addr) {
        char buf[32];
        osw_snprintf(
            OSW_STRS(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        add_assoc_string(zv, name, buf);
    };
#ifdef SIOCGIFHWADDR
    struct ifconf ifc;
    struct ifreq buf[16];

    int sock;
    int i = 0, num = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        php_openswoole_sys_error(E_WARNING, "new socket failed");
        RETURN_FALSE;
    }
    array_init(return_value);

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t) buf;
    if (!ioctl(sock, SIOCGIFCONF, (char *) &ifc)) {
        num = ifc.ifc_len / sizeof(struct ifreq);
        while (i < num) {
            if (!(ioctl(sock, SIOCGIFHWADDR, (char *) &buf[i]))) {
                add_assoc_address(return_value, buf[i].ifr_name, (unsigned char *) buf[i].ifr_hwaddr.sa_data);
            }
            i++;
        }
    }
    close(sock);
#else
#ifdef LLADDR
    ifaddrs *ifas, *ifa;
    if (getifaddrs(&ifas) == 0) {
        array_init(return_value);
        for (ifa = ifas; ifa; ifa = ifa->ifa_next) {
            if ((ifa->ifa_addr->sa_family == AF_LINK) && ifa->ifa_addr) {
                add_assoc_address(
                    return_value, ifa->ifa_name, (unsigned char *) (LLADDR((struct sockaddr_dl *) ifa->ifa_addr)));
            }
        }
        freeifaddrs(ifas);
    }
#else
    php_error_docref(nullptr, E_WARNING, "openswoole_get_local_mac is not supported");
    RETURN_FALSE;
#endif
#endif
}

static PHP_METHOD(openswoole_util, mimeTypeAdd) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(openswoole::mime_type::add(ZSTR_VAL(suffix), ZSTR_VAL(mime_type)));
}

static PHP_METHOD(openswoole_util, mimeTypeSet) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    openswoole::mime_type::set(ZSTR_VAL(suffix), ZSTR_VAL(mime_type));
}

static PHP_METHOD(openswoole_util, mimeTypeDel) {
    zend_string *suffix;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(suffix)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(openswoole::mime_type::del(ZSTR_VAL(suffix)));
}

static PHP_METHOD(openswoole_util, mimeTypeGet) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_STRING(openswoole::mime_type::get(ZSTR_VAL(filename)).c_str());
}

static PHP_METHOD(openswoole_util, mimeTypeExists) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(openswoole::mime_type::exists(ZSTR_VAL(filename)));
}

static PHP_METHOD(openswoole_util, mimeTypeList) {
    array_init(return_value);
    for (auto &i : openswoole::mime_type::list()) {
        add_next_index_string(return_value, i.second.c_str());
    }
}

void php_openswoole_set_aio_option(HashTable *vht) {
    zval *ztmp;
    /* AIO */
    if (php_openswoole_array_get_value(vht, "aio_core_worker_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = OSW_MAX(1, OSW_MIN(v, UINT32_MAX));
        OpenSwooleG.aio_core_worker_num = v;
    }
    if (php_openswoole_array_get_value(vht, "aio_worker_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = OSW_MAX(1, OSW_MIN(v, UINT32_MAX));
        OpenSwooleG.aio_worker_num = v;
    }
    if (php_openswoole_array_get_value(vht, "aio_max_wait_time", ztmp)) {
        OpenSwooleG.aio_max_wait_time = zval_get_double(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "aio_max_idle_time", ztmp)) {
        OpenSwooleG.aio_max_idle_time = zval_get_double(ztmp);
    }
}

static PHP_METHOD(openswoole_util, setAio) {
    if (osw_reactor()) {
        php_openswoole_fatal_error(E_ERROR, "eventLoop has already been created. unable to change settings");
        RETURN_FALSE;
    }

    zval *zset = nullptr;
    HashTable *vht;
    zval *ztmp;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ARRAY(zset)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    vht = Z_ARRVAL_P(zset);

    php_openswoole_set_global_option(vht);
    php_openswoole_set_aio_option(vht);

    if (php_openswoole_array_get_value(vht, "enable_signalfd", ztmp)) {
        OpenSwooleG.enable_signalfd = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "wait_signal", ztmp)) {
        OpenSwooleG.wait_signal = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "dns_cache_refresh_time", ztmp)) {
        OpenSwooleG.dns_cache_refresh_time = zval_get_double(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "thread_num", ztmp) ||
        php_openswoole_array_get_value(vht, "min_thread_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = OSW_MAX(1, OSW_MIN(v, UINT32_MAX));
        OpenSwooleG.aio_core_worker_num = v;
    }
    if (php_openswoole_array_get_value(vht, "max_thread_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = OSW_MAX(1, OSW_MIN(v, UINT32_MAX));
        OpenSwooleG.aio_worker_num = v;
    }
    if (php_openswoole_array_get_value(vht, "socket_dontwait", ztmp)) {
        OpenSwooleG.socket_dontwait = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "dns_lookup_random", ztmp)) {
        OpenSwooleG.dns_lookup_random = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "use_async_resolver", ztmp)) {
        OpenSwooleG.use_async_resolver = zval_is_true(ztmp);
    }
    if (php_openswoole_array_get_value(vht, "enable_coroutine", ztmp)) {
        OPENSWOOLE_G(enable_coroutine) = zval_is_true(ztmp);
    }
    RETURN_TRUE;
}
