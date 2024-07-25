project(RDMA_SPLIT_SEND)

add_executable(rdma_split_send_bench ./rdma_split_send_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

add_executable(rdma_split_send_export ./rdma_split_send_export.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rdma_split_send_bench ${LIBRARIES})

target_link_libraries(rdma_split_send_export ${LIBRARIES})