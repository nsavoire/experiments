#include <benchmark/benchmark.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <string>

// ---------------------------------------------------------------------------
// Each executable is compiled with one of:
//   -DMODE_LINKED              (link traditional .so)
//   -DMODE_LINKED_TLSDESC      (link tlsdesc .so)
//   -DMODE_DLOPEN              (dlopen traditional .so)
//   -DMODE_DLOPEN_TLSDESC      (dlopen tlsdesc .so, small TLS)
//   -DMODE_DLOPEN_TLSDESC_LARGE (dlopen tlsdesc .so, large TLS → no static TLS)
// ---------------------------------------------------------------------------

#if defined(MODE_LINKED) || defined(MODE_LINKED_TLSDESC)
#include "tls_bench_lib.h"
#else
#include <dlfcn.h>
#endif

// ---------------------------------------------------------------------------
// Function pointer type for dlopen variants
// ---------------------------------------------------------------------------
using bench_fn = void (*)(benchmark::State&);

#if defined(MODE_DLOPEN) || defined(MODE_DLOPEN_TLSDESC) || defined(MODE_DLOPEN_TLSDESC_LARGE)
static bench_fn lib_gd_read, lib_gd_write, lib_ld_read, lib_ld_write;
#endif

// ---------------------------------------------------------------------------
// Local-exec  (main executable TLS – baseline)
// ---------------------------------------------------------------------------
static thread_local int tls_le_var = 42;

static __attribute__((noinline)) int le_read()  { return tls_le_var; }
static __attribute__((noinline)) void le_write(int v) { tls_le_var = v; }

// ---------------------------------------------------------------------------
// Initial-exec
// ---------------------------------------------------------------------------
__attribute__((tls_model("initial-exec")))
static thread_local int tls_ie_var = 42;

static __attribute__((noinline)) int ie_read()  { return tls_ie_var; }
static __attribute__((noinline)) void ie_write(int v) { tls_ie_var = v; }

// ---------------------------------------------------------------------------
// pthread_getspecific / pthread_setspecific
// ---------------------------------------------------------------------------
static pthread_key_t pkey;

__attribute__((constructor))
static void init_pkey() { pthread_key_create(&pkey, nullptr); }

static __attribute__((noinline)) int pk_read() {
    return (int)(intptr_t)pthread_getspecific(pkey);
}
static __attribute__((noinline)) void pk_write(int v) {
    pthread_setspecific(pkey, (void*)(intptr_t)v);
}

// ===========================================================================
//  Dispatch to the right library function depending on mode
// ===========================================================================

#if defined(MODE_LINKED)
static void BM_GD_Read(benchmark::State& s)  { linked_gd_read(s); }
static void BM_GD_Write(benchmark::State& s) { linked_gd_write(s); }
static void BM_LD_Read(benchmark::State& s)  { linked_ld_read(s); }
static void BM_LD_Write(benchmark::State& s) { linked_ld_write(s); }
#elif defined(MODE_LINKED_TLSDESC)
static void BM_GD_Read(benchmark::State& s)  { linked_tlsdesc_gd_read(s); }
static void BM_GD_Write(benchmark::State& s) { linked_tlsdesc_gd_write(s); }
static void BM_LD_Read(benchmark::State& s)  { linked_tlsdesc_ld_read(s); }
static void BM_LD_Write(benchmark::State& s) { linked_tlsdesc_ld_write(s); }
#elif defined(MODE_DLOPEN) || defined(MODE_DLOPEN_TLSDESC) || defined(MODE_DLOPEN_TLSDESC_LARGE)
static void BM_GD_Read(benchmark::State& s)  { lib_gd_read(s); }
static void BM_GD_Write(benchmark::State& s) { lib_gd_write(s); }
static void BM_LD_Read(benchmark::State& s)  { lib_ld_read(s); }
static void BM_LD_Write(benchmark::State& s) { lib_ld_write(s); }
#endif

// ===========================================================================
//  Benchmarks
// ===========================================================================

static void BM_LocalExec_Read(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(le_read());
}
BENCHMARK(BM_LocalExec_Read);

static void BM_InitialExec_Read(benchmark::State& state) {
    for (auto _ : state)
        benchmark::DoNotOptimize(ie_read());
}
BENCHMARK(BM_InitialExec_Read);

static void BM_PthreadGetspecific_Read(benchmark::State& state) {
    pk_write(42);
    for (auto _ : state)
        benchmark::DoNotOptimize(pk_read());
}
BENCHMARK(BM_PthreadGetspecific_Read);

BENCHMARK(BM_GD_Read);
BENCHMARK(BM_LD_Read);

static void BM_LocalExec_Write(benchmark::State& state) {
    int v = 0;
    for (auto _ : state)
        le_write(v++);
}
BENCHMARK(BM_LocalExec_Write);

static void BM_InitialExec_Write(benchmark::State& state) {
    int v = 0;
    for (auto _ : state)
        ie_write(v++);
}
BENCHMARK(BM_InitialExec_Write);

static void BM_PthreadSetspecific_Write(benchmark::State& state) {
    int v = 0;
    for (auto _ : state)
        pk_write(v++);
}
BENCHMARK(BM_PthreadSetspecific_Write);

BENCHMARK(BM_GD_Write);
BENCHMARK(BM_LD_Write);

// ===========================================================================
//  dlopen helpers (only needed for dlopen modes)
// ===========================================================================
#if defined(MODE_DLOPEN) || defined(MODE_DLOPEN_TLSDESC) || defined(MODE_DLOPEN_TLSDESC_LARGE)

static void* load_sym(void* handle, const char* name) {
    void* sym = dlsym(handle, name);
    if (!sym) {
        fprintf(stderr, "dlsym(%s): %s\n", name, dlerror());
        exit(1);
    }
    return sym;
}

static void* must_dlopen(const std::string& path) {
    void* h = dlopen(path.c_str(), RTLD_NOW);
    if (!h) {
        fprintf(stderr, "dlopen(%s): %s\n", path.c_str(), dlerror());
        exit(1);
    }
    return h;
}

#endif

// ===========================================================================
//  main
// ===========================================================================
int main(int argc, char** argv) {
#if defined(MODE_DLOPEN) || defined(MODE_DLOPEN_TLSDESC) || defined(MODE_DLOPEN_TLSDESC_LARGE)
    // Determine library directory from argv[0]
    std::string self = argv[0];
    std::string dir;
    auto pos = self.rfind('/');
    if (pos != std::string::npos)
        dir = self.substr(0, pos + 1);

#if defined(MODE_DLOPEN)
    void* h = must_dlopen(dir + "libtls_bench_lib_dlopen.so");
    const char* prefix = "dlopen";
#elif defined(MODE_DLOPEN_TLSDESC)
    void* h = must_dlopen(dir + "libtls_bench_lib_dlopen_tlsdesc.so");
    const char* prefix = "dlopen_tlsdesc";
#elif defined(MODE_DLOPEN_TLSDESC_LARGE)
    void* h = must_dlopen(dir + "libtls_bench_lib_dlopen_tlsdesc_large.so");
    const char* prefix = "dlopen_tlsdesc_large";
#endif
    auto pfx = std::string(prefix);
    lib_gd_read  = (bench_fn) load_sym(h, (pfx + "_gd_read").c_str());
    lib_gd_write = (bench_fn) load_sym(h, (pfx + "_gd_write").c_str());
    lib_ld_read  = (bench_fn) load_sym(h, (pfx + "_ld_read").c_str());
    lib_ld_write = (bench_fn) load_sym(h, (pfx + "_ld_write").c_str());
#endif

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

#if defined(MODE_DLOPEN) || defined(MODE_DLOPEN_TLSDESC) || defined(MODE_DLOPEN_TLSDESC_LARGE)
    dlclose(h);
#endif
    return 0;
}
