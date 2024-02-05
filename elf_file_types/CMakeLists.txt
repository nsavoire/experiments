
# dynamic pie executble (produced by default by gcc/clang)
add_executable(main_dynamic_pie main.c)
target_compile_options(main_dynamic_pie PRIVATE -fPIE)
target_link_options(main_dynamic_pie PRIVATE -pie)
target_link_libraries(main_dynamic_pie dl)

# dynamic pde executable
add_executable(main_dynamic_pde main.c)
target_compile_options(main_dynamic_pde PRIVATE -fno-pie)
target_link_options(main_dynamic_pde PRIVATE -no-pie)
target_link_libraries(main_dynamic_pde dl)

# static pie executable
add_executable(main_static_pie main.c)
target_compile_options(main_static_pie PRIVATE -fPIE)
target_link_options(main_static_pie PRIVATE -static-pie)
target_link_libraries(main_static_pie dl)

# static pde executable
add_executable(main_static_pde main.c)
target_compile_options(main_static_pde PRIVATE -fno-pie)
target_link_options(main_static_pde PRIVATE -no-pie -static)
target_link_libraries(main_static_pde dl)

#shared library
add_library(foo SHARED libfoo.c)

# executable shared library
add_library(foo_exec SHARED libfoo.c)
target_compile_definitions(foo_exec PRIVATE PT_INTERP="/lib64/ld-linux-x86-64.so.2")
target_link_options(foo_exec PRIVATE LINKER:-e,main)
