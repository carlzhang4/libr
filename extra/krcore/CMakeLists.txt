cmake_minimum_required(VERSION 3.0)

project(KRCORE_BENCH_DUPLEX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast -Wno-unused-function -Wno-missing-braces")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member -Wno-nested-anon-types -Wno-keyword-macro -Wno-deprecated-declarations")


include_directories("${CMAKE_SOURCE_DIR}/../minipcm/include")

file(GLOB PCMRSOURCES ${CMAKE_SOURCE_DIR}/../minipcm/src/*.cpp)
list(APPEND PCMSOURCES ${PCMRSOURCES})


set(LIBRARIES ${LIBRARIES} pthread numa gflags hdr_histogram_static)

option(HDR_HISTOGRAM_BUILD_PROGRAMS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/../../HdrHistogram_c HdrHistogram_c)

add_executable(krcore_bench_duplex ./krcore_bench_duplex.cpp ${PCMSOURCES})

target_link_libraries(krcore_bench_duplex ${LIBRARIES})