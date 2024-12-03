project(EXTRA_RDMA_BENCH)

include_directories("${PROJECT_SOURCE_DIR}/minipcm")

file(GLOB PCMRSOURCES ${PROJECT_SOURCE_DIR}/minipcm/*.cpp)
list(APPEND PCMSOURCES ${PCMRSOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/baseline)

add_subdirectory(${PROJECT_SOURCE_DIR}/freeflow)