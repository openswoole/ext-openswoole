/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c24104b159b061134fc0b11934d9a1c60c8e5dca */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_WebSocket_Server_push, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_ARG_OBJ_TYPE_MASK(0, data, OpenSwoole\\WebSocket\\Frame, MAY_BE_STRING, NULL)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opcode, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_OPCODE_TEXT")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_FLAG_FIN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_WebSocket_Server_disconnect, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, code, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_CLOSE_NORMAL")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, reason, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_WebSocket_Server_isEstablished, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, fd, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_WebSocket_Server_pack, 0, 1, IS_STRING, 0)
ZEND_ARG_OBJ_TYPE_MASK(0, data, OpenSwoole\\WebSocket\\Frame, MAY_BE_STRING, NULL)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opcode, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_OPCODE_TEXT")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_FLAG_FIN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(
    arginfo_class_Swoole_WebSocket_Server_unpack, 0, 1, OpenSwoole\\WebSocket\\Frame, MAY_BE_FALSE)
ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_WebSocket_Frame___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_WebSocket_Frame_pack arginfo_class_Swoole_WebSocket_Server_pack

#define arginfo_class_Swoole_WebSocket_Frame_unpack arginfo_class_Swoole_WebSocket_Server_unpack
