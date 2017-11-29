/**
 *  Created by smelvinsky on 22.10.17.
 */

#include <stdio.h>
#include "array_utils.h"

void print_uint8_t_array(FILE *stream, const uint8_t *array, int array_length)
{
    for (int i = 0; i < array_length; i++)
    {
        fprintf(stream, "%d ", *(array + i));
    }
    //fprintf(stream, "\n");
}