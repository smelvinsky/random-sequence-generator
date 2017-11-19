//
// Created by smelvinsky on 22.10.17.
//

#include <stdio.h>
#include "array_utils.h"

void print_uint8_t_array(FILE *stream, const uint8_t *array, int array_length)
{
    for (int i = 0; i < array_length; i++)
    {
        fprintf(stream, "%d ", *(array + i));
    }
}

void print_uint16_t_array(FILE *stream, const uint16_t *array, int array_length)
{
    for (int i = 0; i < array_length; i++)
    {
        fprintf(stream, "%d ", *(array + i));
    }
}

void print_uint32_t_array(FILE *stream, const uint32_t *array, int array_length)
{
    for (int i = 0; i < array_length; i++)
    {
        fprintf(stream, "%d ", *(array + i));
    }
}

void print_uint64_t_array(FILE *stream, const uint64_t *array, int array_length)
{
    for (int i = 0; i < array_length; i++)
    {
        fprintf(stream, "%ld ", *(array + i));
    }
}