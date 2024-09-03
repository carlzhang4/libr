project(RDMA_RAW_PACKET)

# add_executable(rdma_raw_packet_send ./rdma_raw_packet_send.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

add_executable(rdma_raw_packet_recv ./rdma_raw_packet_recv.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

# target_link_libraries(rdma_raw_packet_send ${LIBRARIES})

target_link_libraries(rdma_raw_packet_recv ${LIBRARIES})