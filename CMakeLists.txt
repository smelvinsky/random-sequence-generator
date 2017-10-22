cmake_minimum_required(VERSION 3.8)
project(RandomSequenceGenerator)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/main.c)
add_executable(RandomSequenceGenerator ${SOURCE_FILES})