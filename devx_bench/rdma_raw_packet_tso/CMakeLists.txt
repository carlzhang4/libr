project(RDMA_RAW_PACKET_TSO)

add_executable(rdma_raw_packet_tso_bench ./rdma_raw_packet_tso_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rdma_raw_packet_tso_bench ${LIBRARIES})