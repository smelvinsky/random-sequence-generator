cmake_minimum_required(VERSION 3.8)
project(RandomSequenceGenerator)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/main.c src/random.c src/random.h src/array_utils.c src/array_utils.h)
add_executable(RandomSequenceGenerator ${SOURCE_FILES})