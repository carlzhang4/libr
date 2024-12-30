project(EXTRA_RDMA_BENCH)


add_subdirectory(${PROJECT_SOURCE_DIR}/baseline)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

include_directories("${PROJECT_SOURCE_DIR}/minipcm/include")

file(GLOB PCMRSOURCES ${PROJECT_SOURCE_DIR}/minipcm/src/*.cpp)
list(APPEND PCMSOURCES ${PCMRSOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/freeflow)

add_subdirectory(${PROJECT_SOURCE_DIR}/minipcm)

endif()