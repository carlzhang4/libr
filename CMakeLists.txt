cmake_minimum_required(VERSION 3.0)

project(libr)

add_compile_options(-g)

aux_source_directory(src SRCS)

add_executable(send ${SRCS} send.cpp) 

target_link_libraries(send ibverbs)

add_executable(write ${SRCS} write.cpp) 

target_link_libraries(write ibverbs)