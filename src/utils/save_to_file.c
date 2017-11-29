/**
 *  Created by smelvinsky on 18.11.17.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "save_to_file.h"

static const char *file_name;

void generate_file_name(char **file_name)
{
    time_t time_raw;
    struct tm *time_struct;

    time(&time_raw);
    time_struct = localtime(&time_raw);

    if(sprintf(*file_name, "ran_seq_%04d_%02d_%02d_%02d:%02d:%02d", time_struct->tm_year + 1900, time_struct->tm_mon, time_struct->tm_mday,
                                               time_struct->tm_hour, time_struct->tm_min, time_struct->tm_sec) < 0)
    {
        fprintf(stderr, "Error in sprintf() function (generate_file_name function in save_to_file.c)");
        exit(EXIT_FAILURE);
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

