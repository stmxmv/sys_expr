cmake_minimum_required(VERSION 3.23)
project(sys)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)

add_executable(producer producer.cpp)
add_executable(consumer consumer.cpp)