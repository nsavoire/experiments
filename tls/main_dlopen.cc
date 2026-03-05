#include <dlfcn.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <libgen.h>
#include <limits.h>
#include <thread>
#include <vector>

#include "labels.hpp"

static int (*get_tls_value_general_dynamic)();
static int (*get_tls_value_local_dynamic)();
static int (*get_tls_value_initial_exec_forced)();
static void (*set_custom_labels_current_set_v2)(const custom_labels_v2_tl_record_t *record);
static void (*publish_otel_process_ctx)();
static void (*update_custom_labels)(uint64_t span_id, trace_id_t trace_id, std::vector<attribute_t> attributes);

int load_lib(bool use_tlsdesc = false) {
    // load the shared library in the same directory as the executable
    char exe_path_buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exe_path_buf, sizeof(exe_path_buf) - 1);
    if (len == -1) {
        fprintf(stderr, "readlink failed: %s\n", ::strerror(errno));
        return 1;
    }
    exe_path_buf[len] = '\0';
    
    char* exe_dir = ::dirname(exe_path_buf);
    std::string lib_path = std::string(exe_dir) + (use_tlsdesc ? "/libtlsdesc.so" : "/libtls.so");
    
    auto r = ::dlopen(lib_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!r) {
        fprintf(stderr, "dlopen failed: %s\n", ::dlerror());
        return 1;
    }

    get_tls_value_general_dynamic = reinterpret_cast<int(*)()>(::dlsym(r, "get_tls_value_general_dynamic"));
    if (!get_tls_value_general_dynamic) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    get_tls_value_local_dynamic = reinterpret_cast<int(*)()>(::dlsym(r, "get_tls_value_local_dynamic"));
    if (!get_tls_value_local_dynamic) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    get_tls_value_initial_exec_forced = reinterpret_cast<int(*)()>(::dlsym(r, "get_tls_value_initial_exec_forced"));
    if (!get_tls_value_initial_exec_forced) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    set_custom_labels_current_set_v2 = reinterpret_cast<void(*)(const custom_labels_v2_tl_record_t *)>(::dlsym(r, "set_custom_labels_current_set_v2"));
    if (!set_custom_labels_current_set_v2) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    publish_otel_process_ctx = reinterpret_cast<void(*)()>(::dlsym(r, "publish_otel_process_ctx"));
    if (!publish_otel_process_ctx) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    update_custom_labels = reinterpret_cast<void(*)(uint64_t span_id, trace_id_t trace_id, std::vector<attribute_t> attributes)>(::dlsym(r, "update_custom_labels"));
    if (!update_custom_labels) {
        fprintf(stderr, "dlsym failed: %s\n", ::dlerror());
        return 1;
    }

    return 0;
}

__attribute__((noinline)) int foo() {
    return get_tls_value_general_dynamic() + get_tls_value_local_dynamic() + get_tls_value_initial_exec_forced();
}

void burn_cpu() {
    for(int i=0; i<1000000000; i++) {
        __asm__("nop");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <use_tlsdesc>\nuse_tlsdesc: 0 for tls, 1 for tlsdesc\n", argv[0]);
        return 1;
    }

    bool use_tlsdesc = std::string(argv[1]) == "1";

    if (load_lib(use_tlsdesc) != 0) {
        return 1;
    }

    publish_otel_process_ctx();
    
    printf("foo() + foo() = %d\n", foo() + foo());

    custom_labels_v2_tl_record_t *record = (custom_labels_v2_tl_record_t *)malloc(sizeof(custom_labels_v2_tl_record_t) + 128);
    set_custom_labels_current_set_v2(record);

    uint64_t span_id = 0;
    trace_id_t trace_id = {0x1234567890abcdefULL, 0x1234567890abcdefULL};
    std::vector<attribute_t> attributes = {
        {0, "hello"},
        {1, "fucking"},
        {2, "world"},
    };
    while (true) {
        update_custom_labels(span_id, trace_id, attributes);
        span_id++;
        burn_cpu();
    }

    return 0;
    // std::thread t(&foo);
    // t.join();

    // return 0;
}