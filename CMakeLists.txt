cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
project(libr)

# add_compile_options(-g -o3 -fcoroutines -std=c++20 -static)
add_compile_options(-g -o3 -std=c++17 -static)

aux_source_directory(src SRCS)

# add_executable(demo ${SRCS} demo.cpp)

add_executable(libr_test ${SRCS} libr_test.cpp)

add_executable(rdma_bench ${SRCS} rdma_bench.cpp)

set(HEAD_FILES
	src/libr.hpp
	src/util.hpp
	src/connection_manager.hpp
)

# target_link_libraries(demo ibverbs pthread numa)
target_link_libraries(libr_test ibverbs pthread numa)
target_link_libraries(rdma_bench ibverbs pthread numa gflags)

add_library(r SHARED ${SRCS})
target_link_libraries(r ibverbs pthread numa)

install(
	TARGETS r
	DESTINATION /usr/lib64
)

install(
	FILES ${HEAD_FILES}
	DESTINATION /usr/include
)

install(
	PROGRAMS script/run.py
	DESTINATION /usr/local/bin
)