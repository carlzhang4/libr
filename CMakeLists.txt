cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "gcc-11")
set(CMAKE_CXX_COMPILER "g++-11")
project(libr)

add_compile_options(-g -o3 -fcoroutines -std=c++20 -static)

aux_source_directory(src SRCS)

add_executable(write ${SRCS} write.cpp) 

add_executable(demo ${SRCS} demo.cpp)

add_executable(libr_test ${SRCS} libr_test.cpp)

target_link_libraries(write ibverbs)
target_link_libraries(demo ibverbs pthread)
target_link_libraries(libr_test ibverbs)