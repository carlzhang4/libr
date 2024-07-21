project(RDMA_VHCA)

add_executable(rdma_vhca_bench ./rdma_vhca_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

add_executable(rdma_vhca_export ./rdma_vhca_export.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rdma_vhca_bench ${LIBRARIES})

target_link_libraries(rdma_vhca_export ${LIBRARIES})