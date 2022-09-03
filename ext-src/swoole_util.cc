/*
  +----------------------------------------------------------------------+
  | Open Swoole                                                          |
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

#include "php_swoole_private.h"
#include "swoole_mime_type.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>

#if defined(__MACH__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <net/if_dl.h>
#endif

#if PHP_VERSION_ID >= 80000
#include "swoole_util_arginfo.h"
#else
#include "swoole_util_arginfo_legacy.h"
#endif

zend_class_entry *swoole_util_ce;
static zend_object_handlers swoole_util_handlers;

SW_EXTERN_C_BEGIN
static PHP_METHOD(swoole_util, getVersion);
static PHP_METHOD(swoole_util, getCPUNum);
static PHP_METHOD(swoole_util, getLocalIp);
static PHP_METHOD(swoole_util, getLocalMac);
static PHP_METHOD(swoole_util, getLastErrorCode);
static PHP_METHOD(swoole_util, getErrorMessage);
static PHP_METHOD(swoole_util, errorCode);
static PHP_METHOD(swoole_util, clearError);
static PHP_METHOD(swoole_util, log);
static PHP_METHOD(swoole_util, hashcode);
static PHP_METHOD(swoole_util, mimeTypeAdd);
static PHP_METHOD(swoole_util, mimeTypeSet);
static PHP_METHOD(swoole_util, mimeTypeDel);
static PHP_METHOD(swoole_util, mimeTypeGet);
static PHP_METHOD(swoole_util, mimeTypeList);
static PHP_METHOD(swoole_util, mimeTypeExists);
static PHP_METHOD(swoole_util, setProcessName);
static PHP_METHOD(swoole_util, setAio);

SW_EXTERN_C_END

static const zend_function_entry swoole_util_methods[] = {
    PHP_ME(swoole_util, getVersion, arginfo_class_Swoole_Util_getVersion, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(
        swoole_util, getCPUNum, arginfo_class_Swoole_Util_getCPUNum, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(swoole_util, getLocalIp, arginfo_class_Swoole_Util_getLocalIp, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(
            swoole_util, getLocalMac, arginfo_class_Swoole_Util_getLocalMac, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
            PHP_ME(swoole_util,
                   getLastErrorCode,
                   arginfo_class_Swoole_Util_getLastErrorCode,
                   ZEND_ACC_PUBLIC |
                       ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                               getErrorMessage,
                                               arginfo_class_Swoole_Util_getErrorMessage,
                                               ZEND_ACC_PUBLIC |
                                                   ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                                                           errorCode,
                                                                           arginfo_class_Swoole_Util_errorCode,
                                                                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_ME(swoole_util, clearError, arginfo_class_Swoole_Util_clearError, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                    PHP_ME(swoole_util, log, arginfo_class_Swoole_Util_log, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(
                        swoole_util, hashcode, arginfo_class_Swoole_Util_hashcode, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                        PHP_ME(swoole_util,
                               mimeTypeAdd,
                               arginfo_class_Swoole_Util_mimeTypeAdd,
                               ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                                                         mimeTypeSet,
                                                                         arginfo_class_Swoole_Util_mimeTypeSet,
                                                                         ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                            PHP_ME(swoole_util,
                                   mimeTypeDel,
                                   arginfo_class_Swoole_Util_mimeTypeDel,
                                   ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                                                             mimeTypeGet,
                                                                             arginfo_class_Swoole_Util_mimeTypeGet,
                                                                             ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                                PHP_ME(
                                    swoole_util,
                                    mimeTypeList,
                                    arginfo_class_Swoole_Util_mimeTypeList,
                                    ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                                                              mimeTypeExists,
                                                                              arginfo_class_Swoole_Util_mimeTypeExists,
                                                                              ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                                    PHP_ME(swoole_util,
                                           setProcessName,
                                           arginfo_class_Swoole_Util_setProcessName,
                                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_ME(swoole_util,
                                                                                     setAio,
                                                                                     arginfo_class_Swoole_Util_setAio,
                                                                                     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

                                        PHP_FE_END};

void php_swoole_util_minit(int module_number) {
    SW_INIT_CLASS_ENTRY(swoole_util, "Swoole\\Util", nullptr, nullptr, swoole_util_methods);
    SW_SET_CLASS_NOT_SERIALIZABLE(swoole_util);
    SW_SET_CLASS_CLONEABLE(swoole_util, sw_zend_class_clone_deny);
    SW_SET_CLASS_UNSET_PROPERTY_HANDLER(swoole_util, sw_zend_class_unset_property_deny);
}

static PHP_METHOD(swoole_util, getVersion) {
    RETURN_STRING(SWOOLE_VERSION);
}

static PHP_METHOD(swoole_util, getCPUNum) {
    RETURN_LONG(SW_CPU_NUM);
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

static PHP_METHOD(swoole_util, hashcode) {
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

static PHP_METHOD(swoole_util, clearError) {
    swoole_set_last_error(0);
}

static PHP_METHOD(swoole_util, errorCode) {
    RETURN_LONG(errno);
}

static PHP_METHOD(swoole_util, getLastErrorCode) {
    RETURN_LONG(swoole_get_last_error());
}

static PHP_METHOD(swoole_util, getErrorMessage) {
    zend_long swoole_errno;
    zend_long error_type = SW_STRERROR_SYSTEM;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_LONG(swoole_errno)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(error_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (error_type == SW_STRERROR_GAI) {
        RETURN_STRING(gai_strerror(swoole_errno));
    } else if (error_type == SW_STRERROR_DNS) {
        RETURN_STRING(hstrerror(swoole_errno));
    } else if (error_type == SW_STRERROR_SWOOLE || (swoole_errno > SW_ERROR_BEGIN && swoole_errno < SW_ERROR_END)) {
        RETURN_STRING(swoole_strerror(swoole_errno));
    } else {
        RETURN_STRING(strerror(swoole_errno));
    }
}

static PHP_METHOD(swoole_util, setProcessName) {
    zend_function *cli_set_process_title =
        (zend_function *) zend_hash_str_find_ptr(EG(function_table), ZEND_STRL("cli_set_process_title"));
    if (!cli_set_process_title) {
        php_swoole_fatal_error(E_WARNING, "swoole_set_process_name only support in CLI mode");
        RETURN_FALSE;
    }
    cli_set_process_title->internal_function.handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_METHOD(swoole_util, log) {
    char *msg;
    size_t l_msg;
    zend_long level = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(level)
    Z_PARAM_STRING(msg, l_msg)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    sw_logger()->put(level, msg, l_msg);
}

static PHP_METHOD(swoole_util, getLocalIp) {
    struct sockaddr_in *s4;
    struct ifaddrs *ipaddrs, *ifa;
    void *in_addr;
    char ip[64];

    if (getifaddrs(&ipaddrs) != 0) {
        php_swoole_sys_error(E_WARNING, "getifaddrs() failed");
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

static PHP_METHOD(swoole_util, getLocalMac) {
    auto add_assoc_address = [](zval *zv, const char *name, const unsigned char *addr) {
        char buf[32];
        sw_snprintf(
            SW_STRS(buf), "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        add_assoc_string(zv, name, buf);
    };
#ifdef SIOCGIFHWADDR
    struct ifconf ifc;
    struct ifreq buf[16];

    int sock;
    int i = 0, num = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        php_swoole_sys_error(E_WARNING, "new socket failed");
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
    php_error_docref(nullptr, E_WARNING, "swoole_get_local_mac is not supported");
    RETURN_FALSE;
#endif
#endif
}

static PHP_METHOD(swoole_util, mimeTypeAdd) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::add(ZSTR_VAL(suffix), ZSTR_VAL(mime_type)));
}

static PHP_METHOD(swoole_util, mimeTypeSet) {
    zend_string *suffix;
    zend_string *mime_type;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR(suffix)
    Z_PARAM_STR(mime_type)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    swoole::mime_type::set(ZSTR_VAL(suffix), ZSTR_VAL(mime_type));
}

static PHP_METHOD(swoole_util, mimeTypeDel) {
    zend_string *suffix;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(suffix)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::del(ZSTR_VAL(suffix)));
}

static PHP_METHOD(swoole_util, mimeTypeGet) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_STRING(swoole::mime_type::get(ZSTR_VAL(filename)).c_str());
}

static PHP_METHOD(swoole_util, mimeTypeExists) {
    zend_string *filename;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(filename)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(swoole::mime_type::exists(ZSTR_VAL(filename)));
}

static PHP_METHOD(swoole_util, mimeTypeList) {
    array_init(return_value);
    for (auto &i : swoole::mime_type::list()) {
        add_next_index_string(return_value, i.second.c_str());
    }
}

void php_swoole_set_aio_option(HashTable *vht) {
    zval *ztmp;
    /* AIO */
    if (php_swoole_array_get_value(vht, "aio_core_worker_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = SW_MAX(1, SW_MIN(v, UINT32_MAX));
        SwooleG.aio_core_worker_num = v;
    }
    if (php_swoole_array_get_value(vht, "aio_worker_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = SW_MAX(1, SW_MIN(v, UINT32_MAX));
        SwooleG.aio_worker_num = v;
    }
    if (php_swoole_array_get_value(vht, "aio_max_wait_time", ztmp)) {
        SwooleG.aio_max_wait_time = zval_get_double(ztmp);
    }
    if (php_swoole_array_get_value(vht, "aio_max_idle_time", ztmp)) {
        SwooleG.aio_max_idle_time = zval_get_double(ztmp);
    }
}

static PHP_METHOD(swoole_util, setAio) {
    if (sw_reactor()) {
        php_swoole_fatal_error(E_ERROR, "eventLoop has already been created. unable to change settings");
        RETURN_FALSE;
    }

    zval *zset = nullptr;
    HashTable *vht;
    zval *ztmp;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ARRAY(zset)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    vht = Z_ARRVAL_P(zset);

    php_swoole_set_global_option(vht);
    php_swoole_set_aio_option(vht);

    if (php_swoole_array_get_value(vht, "enable_signalfd", ztmp)) {
        SwooleG.enable_signalfd = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "wait_signal", ztmp)) {
        SwooleG.wait_signal = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "dns_cache_refresh_time", ztmp)) {
        SwooleG.dns_cache_refresh_time = zval_get_double(ztmp);
    }
    if (php_swoole_array_get_value(vht, "thread_num", ztmp) ||
        php_swoole_array_get_value(vht, "min_thread_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = SW_MAX(1, SW_MIN(v, UINT32_MAX));
        SwooleG.aio_core_worker_num = v;
    }
    if (php_swoole_array_get_value(vht, "max_thread_num", ztmp)) {
        zend_long v = zval_get_long(ztmp);
        v = SW_MAX(1, SW_MIN(v, UINT32_MAX));
        SwooleG.aio_worker_num = v;
    }
    if (php_swoole_array_get_value(vht, "socket_dontwait", ztmp)) {
        SwooleG.socket_dontwait = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "dns_lookup_random", ztmp)) {
        SwooleG.dns_lookup_random = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "use_async_resolver", ztmp)) {
        SwooleG.use_async_resolver = zval_is_true(ztmp);
    }
    if (php_swoole_array_get_value(vht, "enable_coroutine", ztmp)) {
        SWOOLE_G(enable_coroutine) = zval_is_true(ztmp);
    }
    RETURN_TRUE;
}
