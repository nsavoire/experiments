#pragma once

#include <cstdint>
#include <string>

struct attribute_t {
    uint8_t key_index;
    std::string value;
};

struct trace_id_t {
    uint64_t lo;
    uint64_t hi;
 };
 
using span_id_t = uint64_t;

struct custom_labels_v2_tl_record_t {
    // W3C trace context fields
    trace_id_t trace_id;
    span_id_t span_id;

    // Readers should ignore this record if valid is 0
    uint8_t valid;
    uint8_t _reserved;

    // Number of attributes in attrs_data
    uint16_t attrs_data_size;

    // Attribute data; each attr is [key_index:1][length:1][value:length]
    // This is stored without padding to a constant length (like the key table)
    // so that we squeeze as much data as we can into each context update.
    uint8_t attrs_data[];
};
