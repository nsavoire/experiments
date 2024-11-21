#include "elfutils.hpp"

#include <unistd.h>
#include <cstdio>

namespace {

SymbolOverrides *symbol_overrides;

decltype(&::write) real_write = nullptr;

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    puts("write intercepted");
    return real_write(fildes, buf, nbyte);
}

__attribute__((constructor)) void init() {
    fprintf(stderr, "symbol_override init\n");
    symbol_overrides = new SymbolOverrides();
    symbol_overrides->register_override("write", reinterpret_cast<uintptr_t>(&write), reinterpret_cast<uintptr_t *>(&real_write));
    symbol_overrides->apply_overrides();
}

}