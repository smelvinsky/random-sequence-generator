/**
 * Created by smelvinsky on 18.11.17.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "save_to_file.h"

static const char *file_name;

char *generate_file_name()
{
    time_t time_raw;
    struct tm *time_struct;
    char *name;

    time(&time_raw);
    time_struct = localtime(&time_raw);

    if(sprintf(name, "ran_seq_%04d_%02d_%02d_%02d:%02d:%02d", time_struct->tm_year + 1900, time_struct->tm_mon, time_struct->tm_mday,
                                               time_struct->tm_hour, time_struct->tm_min, time_struct->tm_sec) < 0)
    {
        fprintf(stderr, "Error in sprintf() function");
        exit(EXIT_FAILURE);
    }
    else
    {
        return name;
    }
}

FILE *create_and_open_file(char *filename)
{
    if(!filename)
    {
        fprintf(stderr, "filename pointer cannot be NULL\n");
        exit(EXIT_FAILURE);
    }

    file_name = filename;
    return fopen(filename, "a+");
};

void write_to_file(const void *data_ptr, FILE *stream)
{
    if(!data_ptr)
    {
        fprintf(stderr, "data_ptr pointer cannot be NULL\n");
        exit(EXIT_FAILURE);
    }

    if(!stream)
    {
        fprintf(stderr, "stream pointer cannot be NULL\n");
        exit(EXIT_FAILURE);
    }

    if (fwrite(data_ptr, sizeof(char), sizeof(data_ptr), stream) != sizeof(data_ptr))
    {
        fprintf(stderr, "Problem while writing to file \"%s\" (size of data written is not equal to requested size)", file_name);
    }
}

void close_file(FILE *stream)
{
    if(!stream)
    {
        fprintf(stderr, "stream pointer cannot be NULL\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(stream) != 0)
    {
        fprintf(stderr, "Problem while closing file %s occurred\n", file_name);
        exit(EXIT_FAILURE);
    }
}

