#include "php_swoole_cxx.h"

//----------------------------------Swoole known string------------------------------------

static const char *sw_known_strings[] = {
#define _SW_ZEND_STR_DSC(id, str) str,
    SW_ZEND_KNOWN_STRINGS(_SW_ZEND_STR_DSC)
#undef _SW_ZEND_STR_DSC
        nullptr};

SW_API zend_string **sw_zend_known_strings = nullptr;

//----------------------------------Swoole known string------------------------------------

typedef zend_string zend_source_string_t;

static zend_op_array *swoole_compile_string(zend_source_string_t *source_string,
                                            ZEND_STR_CONST char *filename,
                                            zend_compile_position position);

static zend_op_array *(*old_compile_string)(zend_source_string_t *source_string,
                                            ZEND_STR_CONST char *filename,
                                            zend_compile_position position);

static zend_op_array *swoole_compile_string(zend_source_string_t *source_string,
                                            ZEND_STR_CONST char *filename,
                                            zend_compile_position position) {
    zend_op_array *opa = old_compile_string(source_string, filename, position);
    opa->type = ZEND_USER_FUNCTION;
    return opa;
}

namespace zend {
bool eval(const std::string &code, std::string const &filename) {
    if (!old_compile_string) {
        old_compile_string = zend_compile_string;
    }
    // overwrite
    zend_compile_string = swoole_compile_string;
    int ret = (zend_eval_stringl((char *) code.c_str(), code.length(), nullptr, (char *) filename.c_str()) == SUCCESS);
    // recover
    zend_compile_string = old_compile_string;
    return ret;
}

void known_strings_init(void) {
    zend_string *str;
    sw_zend_known_strings = nullptr;

    /* known strings */
    sw_zend_known_strings = (zend_string **) pemalloc(
        sizeof(zend_string *) * ((sizeof(sw_known_strings) / sizeof(sw_known_strings[0]) - 1)), 1);
    for (unsigned int i = 0; i < (sizeof(sw_known_strings) / sizeof(sw_known_strings[0])) - 1; i++) {
        str = zend_string_init(sw_known_strings[i], strlen(sw_known_strings[i]), 1);
        sw_zend_known_strings[i] = zend_new_interned_string(str);
    }
}

void known_strings_dtor(void) {
    pefree(sw_zend_known_strings, 1);
    sw_zend_known_strings = nullptr;
}

namespace function {

bool call(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv, zval *retval, const bool enable_coroutine) {
    bool success;
    if (enable_coroutine) {
        if (retval) {
            /* the coroutine has no return value */
            ZVAL_NULL(retval);
        }
        success = swoole::PHPCoroutine::create(fci_cache, argc, argv) >= 0;
    } else {
        success = sw_zend_call_function_ex(nullptr, fci_cache, argc, argv, retval) == SUCCESS;
    }
    /* we have no chance to return to ZendVM to check the exception  */
    if (UNEXPECTED(EG(exception))) {
        zend_exception_error(EG(exception), E_ERROR);
    }
    return success;
}

ReturnValue call(const std::string &func_name, int argc, zval *argv) {
    zval function_name;
    ZVAL_STRINGL(&function_name, func_name.c_str(), func_name.length());
    ReturnValue retval;
    if (call_user_function(EG(function_table), NULL, &function_name, &retval.value, argc, argv) != SUCCESS) {
        ZVAL_NULL(&retval.value);
    }
    zval_dtor(&function_name);
    /* we have no chance to return to ZendVM to check the exception  */
    if (UNEXPECTED(EG(exception))) {
        zend_exception_error(EG(exception), E_ERROR);
    }
    return retval;
}

}  // namespace function
}  // namespace zend
