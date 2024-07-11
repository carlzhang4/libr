cmake_minimum_required(VERSION 3.0)

project(libr)

# add_compile_options(-g -o3 -fcoroutines -std=c++20 -static)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17 -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wno-unused-function -Wno-missing-braces")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member -Wno-nested-anon-types -Wno-keyword-macro -Wno-deprecated-declarations")

set(SRCS
        ${CMAKE_SOURCE_DIR}/src/connection_manager.cpp
        ${CMAKE_SOURCE_DIR}/src/libr.cpp
        ${CMAKE_SOURCE_DIR}/src/util.cpp
)

set(HEAD_FILES
	${CMAKE_SOURCE_DIR}/include/libr.hpp
	${CMAKE_SOURCE_DIR}/include/util.hpp
	${CMAKE_SOURCE_DIR}/include/connection_manager.hpp
)

include_directories("${CMAKE_SOURCE_DIR}/include")

#sub-project: HdrHistogram
include_directories(${CMAKE_SOURCE_DIR}/HdrHistogram/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/HdrHistogram)


# add_executable(demo ${SRCS} demo.cpp)

add_executable(libr_test ${SRCS} libr_test.cpp)

add_executable(rdma_bench ${SRCS} rdma_bench.cpp)

add_executable(rdma_bench_duplex ${SRCS} rdma_bench_duplex.cpp)

set(LIBRARIES ${LIBRARIES} ibverbs pthread numa gflags hdr_histogram_static)

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