cmake_minimum_required(VERSION 3.10)
project(clion_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(clion_test main.cpp common_utf8handle.h common_utf8handle.cpp)
