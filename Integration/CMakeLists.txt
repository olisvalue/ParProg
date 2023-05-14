cmake_minimum_required(VERSION 3.16)
project(Integration)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -pthread)
add_executable(main main.c)

find_package(Threads REQUIRED)
target_link_libraries(main PRIVATE Threads::Threads m)