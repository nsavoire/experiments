#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>

int main(int argc, char* argv[]) {
    ::write(1, "Hello1\n", 7);
    auto r = ::dlopen("./libsymbol_override.so", RTLD_NOW | RTLD_GLOBAL);
    if (!r) {
        fprintf(stderr, "dlopen failed: %s\n", ::dlerror());
        return 1;
    }

    ::write(1, "Hello2\n", 7);
    ::write(1, "Hello3\n", 7);
    return 0;
}