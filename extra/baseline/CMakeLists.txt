project(BASELINE_BENCH_DUPLEX)

add_executable(baseline_bench_duplex ./baseline_bench_duplex.cpp  ${SRCS} ${APP_ADDITIONAL_SOURCES})

target_link_libraries(baseline_bench_duplex ${LIBRARIES})