cmake_minimum_required(VERSION 3.23)
project(sys)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)

add_executable(sys1 sys1.c)
add_executable(sys2 sys2.c)
add_executable(sys3 sys3.c)
add_executable(sys4 sys4.c)
add_executable(btree btree.c)