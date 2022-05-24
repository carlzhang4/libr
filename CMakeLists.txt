cmake_minimum_required(VERSION 3.0)

project(libr)

aux_source_directory(. ROOT_SRCS)

aux_source_directory(src SRCS)

add_executable(send ${SRCS} ${ROOT_SRCS}) 

target_link_libraries(send ibverbs)