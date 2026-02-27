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
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#pragma once

#include "openswoole.h"

#ifdef OSW_USE_OPENSSL

#include <unordered_map>
#include <string>
#include <array>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/ossl_typ.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rand.h>
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
#define OSW_SUPPORT_DTLS
#endif

#if defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x3000000fL
#undef OSW_SUPPORT_DTLS
#endif

enum swSSLCreateFlag {
    OSW_SSL_SERVER = 1,
    OSW_SSL_CLIENT = 2,
};

enum swSSLState {
    OSW_SSL_STATE_HANDSHAKE = 0,
    OSW_SSL_STATE_READY = 1,
    OSW_SSL_STATE_WAIT_STREAM = 2,
};

enum swSSLVersion {
    OSW_SSL_SSLv2 = 1u << 1,
    OSW_SSL_SSLv3 = 1u << 2,
    OSW_SSL_TLSv1 = 1u << 3,
    OSW_SSL_TLSv1_1 = 1u << 4,
    OSW_SSL_TLSv1_2 = 1u << 5,
    OSW_SSL_TLSv1_3 = 1u << 6,
    OSW_SSL_DTLS = 1u << 7,
};

#define OSW_SSL_ALL (OSW_SSL_SSLv2 | OSW_SSL_TLSv1 | OSW_SSL_TLSv1_1 | OSW_SSL_TLSv1_2 | OSW_SSL_TLSv1_3)

enum swSSLMethod {
    OSW_SSLv23_METHOD = 0,
    OSW_SSLv3_METHOD,
    OSW_SSLv3_SERVER_METHOD,
    OSW_SSLv3_CLIENT_METHOD,
    OSW_SSLv23_SERVER_METHOD,
    OSW_SSLv23_CLIENT_METHOD,
    OSW_TLSv1_METHOD,
    OSW_TLSv1_SERVER_METHOD,
    OSW_TLSv1_CLIENT_METHOD,
#ifdef TLS1_1_VERSION
    OSW_TLSv1_1_METHOD,
    OSW_TLSv1_1_SERVER_METHOD,
    OSW_TLSv1_1_CLIENT_METHOD,
#endif
#ifdef TLS1_2_VERSION
    OSW_TLSv1_2_METHOD,
    OSW_TLSv1_2_SERVER_METHOD,
    OSW_TLSv1_2_CLIENT_METHOD,
#endif
#ifdef OSW_SUPPORT_DTLS
    OSW_DTLS_CLIENT_METHOD,
    OSW_DTLS_SERVER_METHOD,
#endif
};

namespace openswoole {

struct SSLContext {
    uchar http : 1;
    uchar http_v2 : 1;
    uchar prefer_server_ciphers : 1;
    uchar session_tickets : 1;
    uchar stapling : 1;
    uchar stapling_verify : 1;
    std::string ciphers;
    std::string ecdh_curve;
    std::string session_cache;
    std::string dhparam;
    std::string cert_file;
    std::string key_file;
    std::string passphrase;
    std::string client_cert_file;
#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
    uchar disable_tls_host_name : 1;
    std::string tls_host_name;
#endif
    std::string cafile;
    std::string capath;
    uint8_t verify_depth;
    uchar disable_compress : 1;
    uchar verify_peer : 1;
    uchar allow_self_signed : 1;
    uint32_t protocols;
    uint8_t create_flag;
    SSL_CTX *context;

    SSL_CTX *get_context() {
        return context;
    }

    bool ready() {
        return context != nullptr;
    }

    void set_protocols(uint32_t _protocols) {
        protocols = _protocols;
    }

    bool set_cert_file(const std::string &_cert_file) {
        if (access(_cert_file.c_str(), R_OK) < 0) {
            openswoole_warning("ssl cert file[%s] not found", _cert_file.c_str());
            return false;
        }
        cert_file = _cert_file;
        return true;
    }

    bool set_key_file(const std::string &_key_file) {
        if (access(_key_file.c_str(), R_OK) < 0) {
            openswoole_warning("ssl key file[%s] not found", _key_file.c_str());
            return false;
        }
        key_file = _key_file;
        return true;
    }

    bool create();
    bool set_capath();
    bool set_ciphers();
    bool set_client_certificate();
    bool set_ecdh_curve();
    bool set_dhparam();
    ~SSLContext();
};
}  // namespace openswoole

void openswoole_ssl_init(void);
void openswoole_ssl_init_thread_safety();
bool openswoole_ssl_is_thread_safety();
void openswoole_ssl_server_http_advise(openswoole::SSLContext &);
const char *openswoole_ssl_get_error();
int openswoole_ssl_get_ex_connection_index();
int openswoole_ssl_get_ex_port_index();
std::string openswoole_ssl_get_version_message();

#endif
