#pragma once

#include "php_openswoole_cxx.h"

#ifdef OSW_USE_CURL
OSW_EXTERN_C_BEGIN

#include <curl/curl.h>
#include <curl/multi.h>

void openswoole_native_curl_minit(int module_number);
void openswoole_native_curl_mshutdown();

PHP_FUNCTION(openswoole_native_curl_close);
PHP_FUNCTION(openswoole_native_curl_copy_handle);
PHP_FUNCTION(openswoole_native_curl_errno);
PHP_FUNCTION(openswoole_native_curl_error);
PHP_FUNCTION(openswoole_native_curl_exec);
PHP_FUNCTION(openswoole_native_curl_getinfo);
PHP_FUNCTION(openswoole_native_curl_init);
PHP_FUNCTION(openswoole_native_curl_setopt);
PHP_FUNCTION(openswoole_native_curl_setopt_array);

#if LIBCURL_VERSION_NUM >= 0x070c01 /* 7.12.1 */
PHP_FUNCTION(openswoole_native_curl_reset);
#endif

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
PHP_FUNCTION(openswoole_native_curl_escape);
PHP_FUNCTION(openswoole_native_curl_unescape);
#endif

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
PHP_FUNCTION(openswoole_native_curl_pause);
#endif
PHP_FUNCTION(openswoole_native_curl_multi_add_handle);
PHP_FUNCTION(openswoole_native_curl_multi_close);
PHP_FUNCTION(openswoole_native_curl_multi_errno);
PHP_FUNCTION(openswoole_native_curl_multi_exec);
PHP_FUNCTION(openswoole_native_curl_multi_select);
PHP_FUNCTION(openswoole_native_curl_multi_remove_handle);
PHP_FUNCTION(openswoole_native_curl_multi_setopt);
PHP_FUNCTION(openswoole_native_curl_multi_getcontent);
PHP_FUNCTION(openswoole_native_curl_multi_info_read);
PHP_FUNCTION(openswoole_native_curl_multi_init);
OSW_EXTERN_C_END
#endif
