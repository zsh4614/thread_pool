cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0048 NEW)

set(COMPILER "UNIX GNU Compiler")
project(thread_pool)
set(CMAKE_CXX_STANDARD 11)

add_executable(demo demo.cpp)
target_link_libraries(demo pthread)