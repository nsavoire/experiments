#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_hello.h"

#include "elfutils.hpp"

static zend_function_entry hello_functions[] = {
    PHP_FE(hello_world, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry hello_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_HELLO_WORLD_EXTNAME,
    hello_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_HELLO_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HELLO
ZEND_GET_MODULE(hello)
#endif

namespace {

SymbolOverrides *symbol_overrides;

decltype(&::write) real_write = nullptr;

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    fprintf(stderr, "write intercepted: \"%*s\"\n", nbyte, (const char *)buf);
    return real_write(fildes, buf, nbyte);
}

void init_overrides() {
    fputs("init_overrides\n", stderr);
    symbol_overrides = new SymbolOverrides();
    symbol_overrides->register_override("write", reinterpret_cast<uintptr_t>(&write), reinterpret_cast<uintptr_t *>(&real_write));
    symbol_overrides->apply_overrides();

}

}

PHP_FUNCTION(hello_world)
{
    init_overrides();
    zend_string *str = zend_string_init("Hello", sizeof("Hello")-1, 0);
    RETURN_STR(str);
}
