cmake_minimum_required(VERSION 3.0)

project(libr)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g)

aux_source_directory(src SRCS)


add_executable(write ${SRCS} write.cpp) 

target_link_libraries(write ibverbs)