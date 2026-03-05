#include "lib.hpp"

#include "otel_process_ctx.h"

#include <cstdio>
#include <string>
#include <cstring>

__attribute__((visibility("hidden"))) thread_local int local_tls_val1 = 1, local_tls_val2 = 2;
thread_local int tls_val = 3;

// large array to force allocation outside of static TLS block
thread_local int tls_val_array[4000000] = {5};
thread_local int tls_value = 6;

extern "C" {
    thread_local custom_labels_v2_tl_record_t *custom_labels_current_set_v2 = nullptr;
}

int get_tls_value_general_dynamic() {
    return tls_val + tls_val_array[0];
}

int get_tls_value_local_dynamic() {
    return local_tls_val1 + local_tls_val2;
}

#ifdef FORCE_TLS_INITIAL_EXEC
// dlopen fails if array is too large (eg. 1000)
__attribute__((tls_model("initial-exec"))) thread_local int tls_value_initial_exec[100];
int get_tls_value_initial_exec_forced() {
    return tls_value_initial_exec[0];
}
#else
int get_tls_value_initial_exec_forced() {
    return 0;
}
#endif

void set_custom_labels_current_set_v2(custom_labels_v2_tl_record_t *record) {
    custom_labels_current_set_v2 = record;
}

void update_custom_labels(uint64_t span_id, trace_id_t trace_id, std::vector<attribute_t> attributes) {
    if (!custom_labels_current_set_v2) {
        return;
    }

    custom_labels_current_set_v2->span_id = span_id;
    custom_labels_current_set_v2->trace_id = trace_id;

    size_t attr_pos = 0;
    for (size_t i = 0; i < attributes.size(); i++) {
        custom_labels_current_set_v2->attrs_data[attr_pos++] = attributes[i].key_index;
        custom_labels_current_set_v2->attrs_data[attr_pos++] = attributes[i].value.size();
        memcpy(custom_labels_current_set_v2->attrs_data + attr_pos, attributes[i].value.c_str(), attributes[i].value.size());
        attr_pos += attributes[i].value.size();
    }

    custom_labels_current_set_v2->attrs_data_size = attr_pos;
    custom_labels_current_set_v2->valid = 1;
}

void publish_otel_process_ctx() {
    const char *resource_attributes[] = {
        "resource.key1", "resource.value1",
        "resource.key2", "resource.value2",
        NULL
      };
      
      const char *extra_attributes[] = {
        "example_extra_attribute_foo", "example_extra_attribute_foo_value",
        NULL
      };

    const char *attribute_key_map[] = {"http_route", "http_method", "user_id", NULL};
    otel_thread_ctx_config_data thread_ctx_config = {
      .schema_version = "tlsdesc_v1_dev",
      .attribute_key_map = attribute_key_map,
    };
    
    otel_process_ctx_data data = {
        .deployment_environment_name = "prod",
        .service_instance_id = "123d8444-2c7e-46e3-89f6-6217880f7123",
        .service_name = "my-service",
        .service_version = "4.5.6",
        .telemetry_sdk_language = "c",
        .telemetry_sdk_version = "1.2.3",
        .telemetry_sdk_name = "example_ctx.c",
        .resource_attributes = resource_attributes,
        .extra_attributes = extra_attributes,
        .thread_ctx_config = &thread_ctx_config,
    };

    otel_process_ctx_result result = otel_process_ctx_publish(&data);
    if (!result.success) {
      fprintf(stderr, "Failed to publish: %s\n", result.error_message);
    }
}
