project(RDMA_RAW_PACKET)

add_executable(rdma_raw_packet_bench ./rdma_raw_packet_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rdma_raw_packet_bench ${LIBRARIES})