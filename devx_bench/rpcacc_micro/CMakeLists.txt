project(RPCACC_MICRO)

add_executable(rpcacc_micro_bench ./rpcacc_micro_bench.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

add_executable(rpcacc_micro_export ./rpcacc_micro_export.cpp ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(rpcacc_micro_bench ${LIBRARIES})

target_link_libraries(rpcacc_micro_export ${LIBRARIES})