project(EXTRA_RDMA_BENCH)

include_directories("${PROJECT_SOURCE_DIR}/minipcm/include")

file(GLOB PCMRSOURCES ${PROJECT_SOURCE_DIR}/minipcm/src/*.cpp)
list(APPEND PCMSOURCES ${PCMRSOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/baseline)

add_subdirectory(${PROJECT_SOURCE_DIR}/freeflow)

add_subdirectory(${PROJECT_SOURCE_DIR}/minipcm)