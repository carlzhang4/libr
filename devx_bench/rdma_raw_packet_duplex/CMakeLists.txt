project(RDMA_RAW_PACKET_DUPLEX)

add_executable(rdma_raw_packet_duplex_bench ./rdma_raw_packet_duplex_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rdma_raw_packet_duplex_bench ${LIBRARIES})