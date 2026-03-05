#include "lib.hpp"

#include <thread>
#include <cstring>
#include <cstdlib>
#include <cstdio>

thread_local int local_tls_value = 10;

__attribute__((noinline)) int get_tls_value_local_exec() {
    return local_tls_value;
}

__attribute__((noinline)) int get_tls_value_initial_exec() {
    return tls_value;
}

__attribute__((noinline)) int foo() {
    return get_tls_value_general_dynamic() 
    + get_tls_value_initial_exec() 
    + get_tls_value_local_dynamic() 
    + get_tls_value_initial_exec_forced()
    + get_tls_value_local_exec();
}


void burn_cpu() {
    for(int i=0; i<1000000000; i++) {
        __asm__("nop");
    }
}

int main() {
    publish_otel_process_ctx();
    
    std::thread t(&foo);
    t.join();
    foo();

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
}
