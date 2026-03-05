#pragma once

#include "labels.hpp"
#include <vector>

extern "C" {
extern thread_local int tls_value;

int get_tls_value_general_dynamic();
int get_tls_value_local_dynamic();
int get_tls_value_initial_exec_forced();


void set_custom_labels_current_set_v2(custom_labels_v2_tl_record_t *record);
void publish_otel_process_ctx();
void update_custom_labels(uint64_t span_id, trace_id_t trace_id, std::vector<attribute_t> attributes);
}
