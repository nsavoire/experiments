set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(symbol_override SHARED elfutils.cc init.cc)
add_executable(test_symbol_override test_symbol_override.cc)
