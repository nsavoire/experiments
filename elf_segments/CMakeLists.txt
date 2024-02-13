# Build an exe with a shared library that has segments alignment constraints to 2MB
# This fails on glibc < 2.35, [p_align on PT_LOAD segment in DSO isn't honored](https://sourceware.org/bugzilla/show_bug.cgi?id=28676)
add_library(foo_aligned SHARED foo.c)
target_link_options(foo_aligned PRIVATE LINKER:-z,max-page-size=0x200000)
add_executable(aligned_shared main.c)
target_link_libraries(aligned_shared foo_aligned)

# Build a pie exe has segments alignment constraints to 2MB
add_executable(aligned_pie main.c foo.c)
target_link_options(aligned_pie PRIVATE LINKER:-z,max-page-size=0x200000)

add_executable(aligned_static main.c foo.c)
target_link_options(aligned_static PRIVATE -static)

# Fails because of [Kernel doesn't honor p_align of PT_LOAD segments in static PIE](https://bugzilla.kernel.org/show_bug.cgi?id=215275)
add_executable(aligned_static_pie main.c foo.c)
target_link_options(aligned_static_pie PRIVATE LINKER:-z,max-page-size=0x200000 -static-pie)

# Build a pde exe has segments alignment constraints to 2MB
# max-page-size is not needed because the exe is position dependent and therefore the segments are mapped at their virtual addresses
add_executable(aligned_pde main.c foo.c)
target_compile_options(aligned_pde PRIVATE -fno-pie)
target_link_options(aligned_pde PRIVATE -no-pie)


# baseline
add_executable(test test.c)

# add alignment constraints to sections with linker script
add_executable(test_align_ld test.c)
target_link_options(test_align_ld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_align.ld)

# lld allows parts of LOAD segments to belong to the same page and produces segments with Offset != VirtAddr
add_executable(test_align_lld test.c)
target_link_options(test_align_ld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_align.ld -fuse-ld=lld)

# Add LMA (load memory address) constraints to sections with linker script
add_executable(test_lma_ld test.c)
target_link_options(test_lma_ld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_lma.ld)

add_executable(test_lma_lld test.c)
target_link_options(test_lma_lld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_lma.ld -fuse-ld=lld)

# mix sections
add_executable(test_mix_ld test.c)
target_link_options(test_mix_ld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_mix.ld)

# this builds an exe with 7 LOAD segments that all belong to the same file page but are then mmapped at different addresses
add_executable(test_mix_lld test.c)
target_link_options(test_mix_lld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_mix.ld -fuse-ld=lld)

# mix sections with EXEC segment first
add_executable(test_mix_exec_first_ld test.c)
target_link_options(test_mix_exec_first_ld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_mix_exec_first.ld)

# this does not work with lld (exec segment is second)
add_executable(test_mix_exec_first_lld test.c)
target_link_options(test_mix_exec_first_lld PRIVATE LINKER:-T,${CMAKE_CURRENT_SOURCE_DIR}/test_mix_exec_first.ld -fuse-ld=lld)
