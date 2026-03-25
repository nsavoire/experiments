#pragma once

namespace benchmark { class State; }

// Linked at load time (traditional dialect)
extern "C" void linked_gd_read(benchmark::State& state);
extern "C" void linked_gd_write(benchmark::State& state);
extern "C" void linked_ld_read(benchmark::State& state);
extern "C" void linked_ld_write(benchmark::State& state);

// Linked at load time (tlsdesc dialect)
extern "C" void linked_tlsdesc_gd_read(benchmark::State& state);
extern "C" void linked_tlsdesc_gd_write(benchmark::State& state);
extern "C" void linked_tlsdesc_ld_read(benchmark::State& state);
extern "C" void linked_tlsdesc_ld_write(benchmark::State& state);
