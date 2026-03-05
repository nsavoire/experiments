
add_library(tls SHARED lib.cc otel_process_ctx.c)

add_library(tlsdesc SHARED lib.cc otel_process_ctx.c)
target_compile_options(tlsdesc PRIVATE -mtls-dialect=gnu2)

add_executable(tls_nopie main.cc)
target_compile_options(tls_nopie PRIVATE -fno-pie)
target_link_options(tls_nopie PRIVATE -no-pie)
target_link_libraries(tls_nopie tls)

add_executable(tls_pie main.cc)
target_compile_options(tls_pie PRIVATE -fpie)
target_link_options(tls_pie PRIVATE -pie)
target_link_libraries(tls_pie tls)

add_executable(tlsdesc_pie main.cc)
target_compile_options(tlsdesc_pie PRIVATE -fpie)
target_link_options(tlsdesc_pie PRIVATE -pie)
target_link_libraries(tlsdesc_pie tlsdesc)

add_executable(tlsdesc_static_pie main.cc lib.cc otel_process_ctx.c)
target_compile_options(tlsdesc_static_pie PRIVATE -mtls-dialect=gnu2 -fpie)
target_link_options(tlsdesc_static_pie PRIVATE -pie)

add_executable(tlsdesc_static_nopie main.cc lib.cc otel_process_ctx.c)
target_compile_options(tlsdesc_static_nopie PRIVATE -mtls-dialect=gnu2 -fno-pie)
target_link_options(tlsdesc_static_nopie PRIVATE -no-pie)

add_executable(tls_dlopen main_dlopen.cc)
target_link_libraries(tls_dlopen ${CMAKE_DL_LIBS})
