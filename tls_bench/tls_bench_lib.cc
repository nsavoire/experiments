// Compiled into a shared library.
//
// General-Dynamic: default visibility TLS variable → linker must use GD
// relocations (or TLSDESC equivalent) since the symbol is exported.
//
// Local-Dynamic: hidden visibility TLS variables → linker can use LD
// relocations (two variables share one __tls_get_addr call for the module
// base, then each adds its own offset).

#include <benchmark/benchmark.h>

// BENCH_PREFIX is set via -D at compile time to give each library variant
// unique symbol names, preventing the dynamic linker from interposing
// variables/functions across libraries.
#ifndef BENCH_PREFIX
#define BENCH_PREFIX bench
#endif

#define _CONCAT(a, b) a ## _ ## b
#define CONCAT(a, b) _CONCAT(a, b)
#define PREFIXED(name) CONCAT(BENCH_PREFIX, name)

// When LARGE_TLS is defined, allocate a large TLS block to exhaust static TLS
// space so the dynamic loader cannot place this library's TLS into the static
// TLS region. This forces TLSDESC to take the slow path via __tls_get_addr.
#ifdef LARGE_TLS
__attribute__((visibility("hidden"))) thread_local char tls_padding[1 << 20] = {1};
#endif

// General-Dynamic (default visibility → exported → GD relocations)
thread_local int PREFIXED(gd_var) = 42;

// Local-Dynamic (hidden → not exported, stays within this module)
__attribute__((visibility("hidden"))) thread_local int PREFIXED(ld_var1) = 42;
__attribute__((visibility("hidden"))) thread_local int PREFIXED(ld_var2) = 99;

// noinline to prevent the compiler from hoisting the TLS access out of the loop
static __attribute__((noinline)) int gd_read()  { return PREFIXED(gd_var); }
static __attribute__((noinline)) void gd_write(int v) { PREFIXED(gd_var) = v; }
static __attribute__((noinline)) int ld_read()  { return PREFIXED(ld_var1) + PREFIXED(ld_var2); }
static __attribute__((noinline)) void ld_write(int v) { PREFIXED(ld_var1) = v; PREFIXED(ld_var2) = v; }

extern "C" {

void PREFIXED(gd_read)(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(gd_read());
}

void PREFIXED(gd_write)(benchmark::State& state) {
    int v = 0;
    for (auto _ : state)
        gd_write(v++);
}

void PREFIXED(ld_read)(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(ld_read());
}

void PREFIXED(ld_write)(benchmark::State& state) {
    int v = 0;
    for (auto _ : state)
        ld_write(v++);
}

}
