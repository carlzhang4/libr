cmake_minimum_required(VERSION 3.0)

project(libr)

# add_compile_options(-g -o3 -fcoroutines -std=c++20 -static)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message(STATUS "Compile on DPU")
    message(STATUS "Output Dir is /build_dpu")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_dpu)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_dpu)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_dpu)

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    message(STATUS "Compile on HOST")
    message(STATUS "Output Dir is /build_host")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_host)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_host)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build_host)
else()
    # 在其他架构下的逻辑代码
    message("Unknown architecture detected")
    return()
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast -Wno-unused-function -Wno-missing-braces")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member -Wno-nested-anon-types -Wno-keyword-macro -Wno-deprecated-declarations")

set(SRCS
        ${CMAKE_SOURCE_DIR}/src/connection_manager.cpp
        ${CMAKE_SOURCE_DIR}/src/libr.cpp
        ${CMAKE_SOURCE_DIR}/src/util.cpp
		${CMAKE_SOURCE_DIR}/src/devx_device.cpp
		${CMAKE_SOURCE_DIR}/src/devx_mr.cpp
)

set(HEAD_FILES
	${CMAKE_SOURCE_DIR}/include/libr.hpp
	${CMAKE_SOURCE_DIR}/include/util.hpp
	${CMAKE_SOURCE_DIR}/include/connection_manager.hpp
)

include_directories("${CMAKE_SOURCE_DIR}/include")

include_directories("${CMAKE_SOURCE_DIR}/include/bench")

include_directories("${CMAKE_SOURCE_DIR}/include/devx")

include_directories("${CMAKE_SOURCE_DIR}/include/fw")

#sub-project: HdrHistogram
include_directories(${CMAKE_SOURCE_DIR}/HdrHistogram_c/include)

set(LIBRARIES ${LIBRARIES} ibverbs pthread numa gflags hdr_histogram_static mlx5)

option(HDR_HISTOGRAM_BUILD_PROGRAMS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/HdrHistogram_c)

add_subdirectory(${CMAKE_SOURCE_DIR}/devx_bench/rdma_vhca)

add_subdirectory(${CMAKE_SOURCE_DIR}/devx_bench/dma_copy)

add_subdirectory(${CMAKE_SOURCE_DIR}/devx_bench/rdma_split_send)

add_subdirectory(${CMAKE_SOURCE_DIR}/devx_bench/rdma_raw_packet)

add_subdirectory(${CMAKE_SOURCE_DIR}/devx_bench/rdma_raw_packet_tso)
# add_executable(demo ${SRCS} demo.cpp)

add_executable(libr_test ${SRCS} libr_test.cpp)

add_executable(rdma_bench ${SRCS} rdma_bench.cpp)

add_executable(rdma_bench_duplex ${SRCS} rdma_bench_duplex.cpp)

# target_link_libraries(demo ibverbs pthread numa)
target_link_libraries(libr_test ${LIBRARIES})
target_link_libraries(rdma_bench ${LIBRARIES})
target_link_libraries(rdma_bench_duplex ${LIBRARIES})

add_library(r SHARED ${SRCS})
target_link_libraries(r ibverbs pthread numa hdr_histogram_static)

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