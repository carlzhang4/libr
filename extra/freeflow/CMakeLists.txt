project(FREEFLOW_BENCH_DUPLEX)

add_executable(freeflow_bench_duplex ./freeflow_bench_duplex.cpp  ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(freeflow_bench_duplex ${LIBRARIES})