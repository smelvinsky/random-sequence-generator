cmake_minimum_required(VERSION 3.8)
project(RandomSequenceGenerator)

link_libraries("-lasound")

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/main.c src/random.c src/random.h src/utils/array_utils.c src/utils/array_utils.h src/data_sources/soundcard/soundcard_noise.c src/data_sources/soundcard/soundcard_noise.h src/data_sources/generator.c src/data_sources/generator.h src/utils/buffer_utils.c src/utils/buffer_utils.h src/data_sources/white_noise_gen/uart.c src/data_sources/white_noise_gen/uart.h)
add_executable(RandomSequenceGenerator ${SOURCE_FILES})