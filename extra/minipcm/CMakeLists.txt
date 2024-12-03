PROJECT(MINIPCM)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB PCMRSOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(APPEND PCMSOURCES ${PCMRSOURCES})

add_executable(spr_pcm_memory ./spr_pcm_memory.cpp  ${SRCS} ${PCMRSOURCES} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(spr_pcm_memory ${LIBRARIES})