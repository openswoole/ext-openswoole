/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012-2018 The Swoole Group                             |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: Xinyu Zhu  <xyzhu1120@gmail.com>                             |
 |         Tianfeng Han <rango@swoole.com>                              |
 +----------------------------------------------------------------------+
 */

#include "php_openswoole_cxx.h"

#include "openswoole_coroutine_channel.h"

#include "openswoole_channel_coro_arginfo.h"

using openswoole::coroutine::Channel;

static zend_class_entry *openswoole_channel_coro_ce;
static zend_object_handlers openswoole_channel_coro_handlers;

struct ChannelObject {
    Channel *chan;
    zend_object std;
};

OSW_EXTERN_C_BEGIN
static PHP_METHOD(openswoole_channel_coro, __construct);
static PHP_METHOD(openswoole_channel_coro, push);
static PHP_METHOD(openswoole_channel_coro, pop);
static PHP_METHOD(openswoole_channel_coro, close);
static PHP_METHOD(openswoole_channel_coro, stats);
static PHP_METHOD(openswoole_channel_coro, length);
static PHP_METHOD(openswoole_channel_coro, isEmpty);
static PHP_METHOD(openswoole_channel_coro, isFull);
static PHP_METHOD(openswoole_channel_coro, getId);
OSW_EXTERN_C_END

// clang-format off
static const zend_function_entry openswoole_channel_coro_methods[] =
{
    PHP_ME(openswoole_channel_coro, __construct, arginfo_class_OpenSwoole_Coroutine_Channel___construct, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, push, arginfo_class_OpenSwoole_Coroutine_Channel_push, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, pop,  arginfo_class_OpenSwoole_Coroutine_Channel_pop,  ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, isEmpty, arginfo_class_OpenSwoole_Coroutine_Channel_isEmpty, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, isFull, arginfo_class_OpenSwoole_Coroutine_Channel_isFull, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, close, arginfo_class_OpenSwoole_Coroutine_Channel_close, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, stats, arginfo_class_OpenSwoole_Coroutine_Channel_stats, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, getId, arginfo_class_OpenSwoole_Coroutine_Channel_getId, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_channel_coro, length, arginfo_class_OpenSwoole_Coroutine_Channel_length, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
// clang-format on

static osw_inline ChannelObject *php_openswoole_channel_coro_fetch_object(zend_object *obj) {
    return (ChannelObject *) ((char *) obj - openswoole_channel_coro_handlers.offset);
}

static osw_inline Channel *php_openswoole_get_channel(zval *zobject) {
    Channel *chan = php_openswoole_channel_coro_fetch_object(Z_OBJ_P(zobject))->chan;
    if (UNEXPECTED(!chan)) {
        php_openswoole_fatal_error(E_ERROR, "you must call Channel constructor first");
    }
    return chan;
}

static void php_openswoole_channel_coro_dtor_object(zend_object *object) {
    zend_objects_destroy_object(object);

    ChannelObject *chan_object = php_openswoole_channel_coro_fetch_object(object);
    Channel *chan = chan_object->chan;
    if (chan) {
        zval *data;
        while ((data = (zval *) chan->pop_data())) {
            osw_zval_free(data);
        }
        delete chan;
        chan_object->chan = nullptr;
    }
}

static void php_openswoole_channel_coro_free_object(zend_object *object) {
    ChannelObject *chan_object = php_openswoole_channel_coro_fetch_object(object);
    Channel *chan = chan_object->chan;
    if (chan) {
        delete chan;
    }
    zend_object_std_dtor(object);
}

static zend_object *php_openswoole_channel_coro_create_object(zend_class_entry *ce) {
    ChannelObject *chan_object = (ChannelObject *) zend_object_alloc(sizeof(ChannelObject), ce);
    zend_object_std_init(&chan_object->std, ce);
    object_properties_init(&chan_object->std, ce);
    chan_object->std.handlers = &openswoole_channel_coro_handlers;
    return &chan_object->std;
}

void php_openswoole_channel_coro_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY(
        openswoole_channel_coro, "OpenSwoole\\Coroutine\\Channel", nullptr, nullptr, openswoole_channel_coro_methods);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_channel_coro);
    OSW_SET_CLASS_CLONEABLE(openswoole_channel_coro, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_channel_coro, osw_zend_class_unset_property_deny);
    OSW_SET_CLASS_CUSTOM_OBJECT(openswoole_channel_coro,
                               php_openswoole_channel_coro_create_object,
                               php_openswoole_channel_coro_free_object,
                               ChannelObject,
                               std);
    OSW_SET_CLASS_DTOR(openswoole_channel_coro, php_openswoole_channel_coro_dtor_object);
    OSW_CLASS_ALIAS("chan", openswoole_channel_coro);

    zend_declare_property_long(openswoole_channel_coro_ce, ZEND_STRL("id"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_long(openswoole_channel_coro_ce, ZEND_STRL("capacity"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_long(openswoole_channel_coro_ce, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);

    zend_declare_class_constant_long(openswoole_channel_coro_ce, ZEND_STRL("CHANNEL_OK"), Channel::ERROR_OK);
    zend_declare_class_constant_long(openswoole_channel_coro_ce, ZEND_STRL("CHANNEL_TIMEOUT"), Channel::ERROR_TIMEOUT);
    zend_declare_class_constant_long(openswoole_channel_coro_ce, ZEND_STRL("CHANNEL_CLOSED"), Channel::ERROR_CLOSED);
    zend_declare_class_constant_long(openswoole_channel_coro_ce, ZEND_STRL("CHANNEL_CANCELED"), Channel::ERROR_CANCELED);

    // backward compatibility
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CHANNEL_OK", Channel::ERROR_OK);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CHANNEL_TIMEOUT", Channel::ERROR_TIMEOUT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CHANNEL_CLOSED", Channel::ERROR_CLOSED);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CHANNEL_CANCELED", Channel::ERROR_CANCELED);
}

static PHP_METHOD(openswoole_channel_coro, __construct) {
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0) {
        php_openswoole_fatal_error(E_ERROR, "capacity is invalid");
        RETURN_FALSE;
    }

    ChannelObject *chan_t = php_openswoole_channel_coro_fetch_object(Z_OBJ_P(ZEND_THIS));
    chan_t->chan = new Channel(capacity);
    zend_update_property_long(openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("id"), chan_t->chan->get_id());
    zend_update_property_long(openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("capacity"), capacity);
}

static PHP_METHOD(openswoole_channel_coro, push) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    zval *zdata;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
    Z_PARAM_ZVAL(zdata)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Z_TRY_ADDREF_P(zdata);
    zdata = osw_zval_dup(zdata);
    if (chan->push(zdata, timeout)) {
        zend_update_property_long(
            openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("errCode"), Channel::ERROR_OK);
        RETURN_TRUE;
    } else {
        zend_update_property_long(
            openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("errCode"), chan->get_error());
        Z_TRY_DELREF_P(zdata);
        efree(zdata);
        RETURN_FALSE;
    }
}

static PHP_METHOD(openswoole_channel_coro, pop) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval *zdata = (zval *) chan->pop(timeout);
    if (zdata) {
        RETVAL_ZVAL(zdata, 0, 0);
        efree(zdata);
        zend_update_property_long(
            openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("errCode"), Channel::ERROR_OK);
    } else {
        zend_update_property_long(
            openswoole_channel_coro_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("errCode"), chan->get_error());
        RETURN_FALSE;
    }
}

static PHP_METHOD(openswoole_channel_coro, close) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    RETURN_BOOL(chan->close());
}

static PHP_METHOD(openswoole_channel_coro, length) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    RETURN_LONG(chan->length());
}

static PHP_METHOD(openswoole_channel_coro, isEmpty) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    RETURN_BOOL(chan->is_empty());
}

static PHP_METHOD(openswoole_channel_coro, isFull) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    RETURN_BOOL(chan->is_full());
}

static PHP_METHOD(openswoole_channel_coro, getId) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    RETURN_LONG(chan->get_id());
}

static PHP_METHOD(openswoole_channel_coro, stats) {
    Channel *chan = php_openswoole_get_channel(ZEND_THIS);
    array_init(return_value);
    add_assoc_long_ex(return_value, ZEND_STRL("consumer_num"), chan->consumer_num());
    add_assoc_long_ex(return_value, ZEND_STRL("producer_num"), chan->producer_num());
    add_assoc_long_ex(return_value, ZEND_STRL("queue_num"), chan->length());
}
