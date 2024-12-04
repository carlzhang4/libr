PROJECT(MINIPCM)

add_executable(spr_pcm_memory ./spr_pcm_memory.cpp  ${SRCS} ${PCMRSOURCES} ${APP_ADDITIONAL_SOURCES})

add_executable(skx_pcm_memory ./skx_pcm_memory.cpp  ${SRCS} ${PCMRSOURCES} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(spr_pcm_memory ${LIBRARIES})

target_link_libraries(skx_pcm_memory ${LIBRARIES})