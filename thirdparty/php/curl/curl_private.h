/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sterling Hughes <sterling@php.net>                           |
   |         Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
*/

/* Copied from PHP-4f68662f5b61aecf90f6d8005976f5f91d4ce8d3 */

#ifdef OSW_USE_CURL

#ifndef _PHP_CURL_PRIVATE_H
#define _PHP_CURL_PRIVATE_H

#include "php_curl.h"

#define PHP_CURL_DEBUG 0

#include "php_version.h"
#define PHP_CURL_VERSION PHP_VERSION

#include <curl/curl.h>
#include <curl/multi.h>

#define CURLOPT_RETURNTRANSFER 19913
#define CURLOPT_BINARYTRANSFER 19914 /* For Backward compatibility */
#define PHP_CURL_STDOUT 0
#define PHP_CURL_FILE   1
#define PHP_CURL_USER   2
#define PHP_CURL_DIRECT 3
#define PHP_CURL_RETURN 4
#define PHP_CURL_IGNORE 7

#define SAVE_CURL_ERROR(__handle, __err) \
    do { (__handle)->err.no = (int) __err; } while (0)

typedef struct {
#if PHP_VERSION_ID < 80400
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
#else
	zend_fcall_info_cache fcc;
#endif
	FILE                 *fp;
	smart_str             buf;
	int                   method;
	zval					stream;
} php_curl_write;

typedef struct {
#if PHP_VERSION_ID < 80400
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
#else
	zend_fcall_info_cache fcc;
#endif
	FILE                 *fp;
	zend_resource        *res;
	int                   method;
	zval                  stream;
} php_curl_read;

#if PHP_VERSION_ID < 80400
typedef struct {
	zval                  func_name;
	zend_fcall_info_cache fci_cache;
	int                   method;
} php_curl_progress, php_curl_fnmatch, php_curlm_server_push, php_curl_fnxferinfo, php_curl_fnsshhostkey;
#endif

typedef struct {
	php_curl_write    *write;
	php_curl_write    *write_header;
	php_curl_read     *read;
	zval               std_err;
#if PHP_VERSION_ID < 80400
	php_curl_progress *progress;
#if LIBCURL_VERSION_NUM >= 0x072000
	php_curl_fnxferinfo  *xferinfo;
#endif
#if LIBCURL_VERSION_NUM >= 0x071500 /* Available since 7.21.0 */
	php_curl_fnmatch  *fnmatch;
#endif
#if LIBCURL_VERSION_NUM >= 0x075400 && PHP_VERSION_ID >= 80300
	php_curl_fnsshhostkey  *sshhostkey;
#endif
#else
	zend_fcall_info_cache progress;
	zend_fcall_info_cache xferinfo;
	zend_fcall_info_cache fnmatch;
	zend_fcall_info_cache debug;
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
	zend_fcall_info_cache prereq;
#endif
#if LIBCURL_VERSION_NUM >= 0x075400
	zend_fcall_info_cache sshhostkey;
#endif
#endif
} php_curl_handlers;

struct _php_curl_error  {
	char str[CURL_ERROR_SIZE + 1];
	int  no;
};

struct _php_curl_send_headers {
	zend_string *str;
};

struct _php_curl_free {
    zend_llist post;
    zend_llist stream;
#if LIBCURL_VERSION_NUM < 0x073800 /* 7.56.0 */
    zend_llist buffers;
#endif
#if PHP_VERSION_ID >= 80500
    HashTable slist;
#else
    HashTable *slist;
#endif
};

/* In PHP 8.5 slist is embedded, in older versions it's a pointer */
#if PHP_VERSION_ID >= 80500
#define curl_to_free_slist(tf) (&(tf)->slist)
#else
#define curl_to_free_slist(tf) ((tf)->slist)
#endif

typedef struct {
	CURL                         *cp;
    php_curl_handlers handlers;
	struct _php_curl_free        *to_free;
	struct _php_curl_send_headers header;
	struct _php_curl_error        err;
#if PHP_VERSION_ID >= 80500
	bool                          in_callback;
#else
	zend_bool                     in_callback;
#endif
	uint32_t*                     clone;
	zval                          postfields;
	/* For CURLOPT_PRIVATE */
    zval private_data;
	/* CurlShareHandle object set using CURLOPT_SHARE. */
	struct _php_curlsh            *share;
	zend_object                   std;
} php_curl;

#define CURLOPT_SAFE_UPLOAD -1

#if PHP_VERSION_ID < 80400
typedef struct {
	php_curlm_server_push	*server_push;
} php_curlm_handlers;
#else
typedef struct {
	zend_fcall_info_cache server_push;
} php_curlm_handlers;
#endif

namespace openswoole  { namespace curl {
class Multi;
}}

using openswoole::curl::Multi;

typedef struct {
	Multi *multi;
	zend_llist  easyh;
	php_curlm_handlers	handlers;
	struct {
		int no;
	} err;
	zend_object std;
} php_curlm;

typedef struct _php_curlsh {
	CURLSH                   *share;
	struct {
		int no;
	} err;
	zend_object std;
} php_curlsh;

php_curl *openswoole_curl_init_handle_into_zval(zval *curl);
void openswoole_curl_init_handle(php_curl *ch);
void openswoole_curl_cleanup_handle(php_curl *);
void openswoole_curl_multi_cleanup_list(void *data);
void openswoole_curl_verify_handlers(php_curl *ch, int reporterror);
void openswoole_setup_easy_copy_handlers(php_curl *ch, php_curl *source);

static inline php_curl_handlers *curl_handlers(php_curl *ch) {
    return &ch->handlers;
}

static inline php_curl *curl_from_obj(zend_object *obj) {
	return (php_curl *)((char *)(obj) - XtOffsetOf(php_curl, std));
}

#define Z_CURL_P(zv) curl_from_obj(Z_OBJ_P(zv))

static inline php_curlsh *curl_share_from_obj(zend_object *obj) {
	return (php_curlsh *)((char *)(obj) - XtOffsetOf(php_curlsh, std));
}

#define Z_CURL_SHARE_P(zv) curl_share_from_obj(Z_OBJ_P(zv))
void curl_multi_register_class(const zend_function_entry *method_entries);

zend_result openswoole_curl_cast_object(zend_object *obj, zval *result, int type);

php_curl *openswoole_curl_get_handle(zval *zid, bool exclusive = true, bool required = true);

OSW_EXTERN_C_BEGIN
OSW_EXTERN_C_END

#endif  /* _PHP_CURL_PRIVATE_H */
#endif
