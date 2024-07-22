project(DMA_COPY)

add_executable(dma_copy_bench ./dma_copy_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

add_executable(dma_copy_export ./dma_copy_export.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(dma_copy_bench ${LIBRARIES})

target_link_libraries(dma_copy_export ${LIBRARIES})