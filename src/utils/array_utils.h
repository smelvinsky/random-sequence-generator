//
// Created by smelvinsky on 22.10.17.
//

#include <bits/types/FILE.h>
#include <stdint.h>

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

void print_uint8_t_array(FILE *stream, const uint8_t *array, int array_length);
void print_uint16_t_array(FILE *stream, const uint16_t *array, int array_length);
void print_uint32_t_array(FILE *stream, const uint32_t *array, int array_length);
void print_uint64_t_array(FILE *stream, const uint64_t *array, int array_length);

#endif //ARRAY_UTILS_H
