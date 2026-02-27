/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012-2015 The Swoole Group                             |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
 +----------------------------------------------------------------------+
 */

#pragma once

#include "php_openswoole_cxx.h"
#include "openswoole_server.h"

#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>

//--------------------------------------------------------
enum php_openswoole_server_callback_type {
    OSW_SERVER_CB_onStart,         // master
    OSW_SERVER_CB_onShutdown,      // master
    OSW_SERVER_CB_onWorkerStart,   // worker(event & task)
    OSW_SERVER_CB_onWorkerStop,    // worker(event & task)
    OSW_SERVER_CB_onBeforeReload,  // manager
    OSW_SERVER_CB_onAfterReload,   // manager
    OSW_SERVER_CB_onTask,          // worker(task)
    OSW_SERVER_CB_onFinish,        // worker(event & task)
    OSW_SERVER_CB_onWorkerExit,    // worker(event)
    OSW_SERVER_CB_onWorkerError,   // manager
    OSW_SERVER_CB_onManagerStart,  // manager
    OSW_SERVER_CB_onManagerStop,   // manager
    OSW_SERVER_CB_onPipeMessage,   // worker(event & task)
};
//--------------------------------------------------------
enum php_openswoole_server_port_callback_type {
    OSW_SERVER_CB_onConnect,      // stream, worker(event)
    OSW_SERVER_CB_onReceive,      // stream, worker(event)
    OSW_SERVER_CB_onClose,        // stream, worker(event)
    OSW_SERVER_CB_onPacket,       // dgram, worker(event)
    OSW_SERVER_CB_onRequest,      // http, worker(event)
    OSW_SERVER_CB_onHandShake,    // websocket, worker(event)
    OSW_SERVER_CB_onOpen,         // websocket, worker(event)
    OSW_SERVER_CB_onMessage,      // websocket, worker(event)
    OSW_SERVER_CB_onDisconnect,   // websocket (non websocket connection), worker(event)
    OSW_SERVER_CB_onBufferFull,   // worker(event)
    OSW_SERVER_CB_onBufferEmpty,  // worker(event)
};

#define PHP_OPENSWOOLE_SERVER_CALLBACK_NUM (OSW_SERVER_CB_onPipeMessage + 1)
#define PHP_OPENSWOOLE_SERVER_PORT_CALLBACK_NUM (OSW_SERVER_CB_onBufferEmpty + 1)

namespace openswoole {
struct TaskCo;

struct ServerPortProperty {
    zval *callbacks[PHP_OPENSWOOLE_SERVER_PORT_CALLBACK_NUM];
    zend_fcall_info_cache *caches[PHP_OPENSWOOLE_SERVER_PORT_CALLBACK_NUM];
    zval _callbacks[PHP_OPENSWOOLE_SERVER_PORT_CALLBACK_NUM];
    Server *serv;
    ListenPort *port;
    zval *zsetting;
};

struct ServerProperty {
    std::vector<zval *> ports;
    std::vector<zval *> user_processes;
    ServerPortProperty *primary_port;
    zend_fcall_info_cache *callbacks[PHP_OPENSWOOLE_SERVER_CALLBACK_NUM];
    std::unordered_map<TaskId, zend_fcall_info_cache> task_callbacks;
    std::unordered_map<TaskId, TaskCo *> task_coroutine_map;
    std::unordered_map<SessionId, std::list<Coroutine *> *> send_coroutine_map;
};

struct ServerObject {
    Server *serv;
    ServerProperty *property;
    zend_object std;

    zend_class_entry *get_ce() {
        return Z_OBJCE_P(get_object());
    }

    zval *get_object() {
        return (zval *) serv->private_data_2;
    }

    bool isset_callback(ListenPort *port, int event_type) {
        ServerPortProperty *port_property = (ServerPortProperty *) port->ptr;
        return (port_property->callbacks[event_type] || property->primary_port->callbacks[event_type]);
    }

    zend_bool is_websocket_server() {
        return instanceof_function(get_ce(), openswoole_websocket_server_ce);
    }

    zend_bool is_http_server() {
        return instanceof_function(get_ce(), openswoole_http_server_ce);
    }

    void register_callback();
    void on_before_start();
};

struct TaskCo {
    Coroutine *co;
    int *list;
    uint32_t count;
    zval *result;
};

}  // namespace openswoole

void php_openswoole_server_register_callbacks(swServer *serv);
zend_fcall_info_cache *php_openswoole_server_get_fci_cache(swServer *serv, int server_fd, int event_type);
int php_openswoole_create_dir(const char *path, size_t length);
void php_openswoole_server_before_start(swServer *serv, zval *zobject);
bool php_openswoole_server_isset_callback(swServer *serv, swListenPort *port, int event_type);
void php_openswoole_http_server_init_global_variant();
void php_openswoole_server_send_yield(swServer *serv, openswoole::SessionId sesion_id, zval *zdata, zval *return_value);
void php_openswoole_get_recv_data(swServer *serv, zval *zdata, swRecvData *req);
void php_openswoole_server_onConnect(swServer *, swDataHead *);
int php_openswoole_server_onReceive(swServer *, swRecvData *);
int php_openswoole_http_server_onReceive(swServer *, swRecvData *);
int php_openswoole_server_onPacket(swServer *, swRecvData *);
void php_openswoole_server_onClose(swServer *, swDataHead *);
void php_openswoole_server_onBufferFull(swServer *, swDataHead *);
void php_openswoole_server_onBufferEmpty(swServer *, swDataHead *);

swServer *php_openswoole_server_get_and_check_server(zval *zobject);
void php_openswoole_server_port_deref(zend_object *object);
openswoole::ServerObject *php_openswoole_server_get_zend_object(openswoole::Server *serv);
zval *php_openswoole_server_get_zval_object(openswoole::Server *serv);
