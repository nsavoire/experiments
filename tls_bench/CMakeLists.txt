include(FetchContent)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.9.1
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(benchmark)

# -- TLS dialect flag (gnu2/desc depending on arch) -------------------------
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(TLSDESC_OPT -mtls-dialect=desc)
else()
    set(TLSDESC_OPT -mtls-dialect=gnu2)
endif()

# -- Shared libraries -------------------------------------------------------
# Linked (traditional)
add_library(tls_bench_lib SHARED tls_bench_lib.cc)
target_compile_definitions(tls_bench_lib PRIVATE BENCH_PREFIX=linked)
target_link_libraries(tls_bench_lib benchmark::benchmark)

# Linked (tlsdesc)
add_library(tls_bench_lib_tlsdesc SHARED tls_bench_lib.cc)
target_compile_definitions(tls_bench_lib_tlsdesc PRIVATE BENCH_PREFIX=linked_tlsdesc)
target_compile_options(tls_bench_lib_tlsdesc PRIVATE ${TLSDESC_OPT})
target_link_libraries(tls_bench_lib_tlsdesc benchmark::benchmark)

# Dlopen (traditional)
add_library(tls_bench_lib_dlopen SHARED tls_bench_lib.cc)
target_compile_definitions(tls_bench_lib_dlopen PRIVATE BENCH_PREFIX=dlopen)
target_link_libraries(tls_bench_lib_dlopen benchmark::benchmark)

# Dlopen (tlsdesc, small TLS → fits in static TLS)
add_library(tls_bench_lib_dlopen_tlsdesc SHARED tls_bench_lib.cc)
target_compile_definitions(tls_bench_lib_dlopen_tlsdesc PRIVATE BENCH_PREFIX=dlopen_tlsdesc)
target_compile_options(tls_bench_lib_dlopen_tlsdesc PRIVATE ${TLSDESC_OPT})
target_link_libraries(tls_bench_lib_dlopen_tlsdesc benchmark::benchmark)

# Dlopen (tlsdesc, large TLS → overflows static TLS, forces slow path)
add_library(tls_bench_lib_dlopen_tlsdesc_large SHARED tls_bench_lib.cc)
target_compile_definitions(tls_bench_lib_dlopen_tlsdesc_large PRIVATE BENCH_PREFIX=dlopen_tlsdesc_large LARGE_TLS)
target_compile_options(tls_bench_lib_dlopen_tlsdesc_large PRIVATE ${TLSDESC_OPT})
target_link_libraries(tls_bench_lib_dlopen_tlsdesc_large benchmark::benchmark)

# -- Benchmark executables (one per mode) ------------------------------------
# 1. Linked, traditional dialect
add_executable(tls_bench_linked tls_bench.cc)
target_compile_definitions(tls_bench_linked PRIVATE MODE_LINKED)
target_link_libraries(tls_bench_linked benchmark::benchmark tls_bench_lib pthread)

# 2. Linked, tlsdesc dialect
add_executable(tls_bench_linked_tlsdesc tls_bench.cc)
target_compile_definitions(tls_bench_linked_tlsdesc PRIVATE MODE_LINKED_TLSDESC)
target_link_libraries(tls_bench_linked_tlsdesc benchmark::benchmark tls_bench_lib_tlsdesc pthread)

# 3. Dlopen, traditional dialect
add_executable(tls_bench_dlopen tls_bench.cc)
target_compile_definitions(tls_bench_dlopen PRIVATE MODE_DLOPEN)
target_link_libraries(tls_bench_dlopen benchmark::benchmark ${CMAKE_DL_LIBS} pthread)
add_dependencies(tls_bench_dlopen tls_bench_lib_dlopen)

# 4. Dlopen, tlsdesc dialect (small TLS)
add_executable(tls_bench_dlopen_tlsdesc tls_bench.cc)
target_compile_definitions(tls_bench_dlopen_tlsdesc PRIVATE MODE_DLOPEN_TLSDESC)
target_link_libraries(tls_bench_dlopen_tlsdesc benchmark::benchmark ${CMAKE_DL_LIBS} pthread)
add_dependencies(tls_bench_dlopen_tlsdesc tls_bench_lib_dlopen_tlsdesc)

# 5. Dlopen, tlsdesc dialect (large TLS → no static TLS)
add_executable(tls_bench_dlopen_tlsdesc_large tls_bench.cc)
target_compile_definitions(tls_bench_dlopen_tlsdesc_large PRIVATE MODE_DLOPEN_TLSDESC_LARGE)
target_link_libraries(tls_bench_dlopen_tlsdesc_large benchmark::benchmark ${CMAKE_DL_LIBS} pthread)
add_dependencies(tls_bench_dlopen_tlsdesc_large tls_bench_lib_dlopen_tlsdesc_large)
