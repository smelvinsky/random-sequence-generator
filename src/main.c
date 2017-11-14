/**
 *  Created by smelvinsky on 21.10.17.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils/array_utils.h"
#include "data_sources/soundcard/soundcard_noise.h"
#include "utils/buffer_utils.h"
#include "data_sources/generator.h"

//TODO: make getopt() argument handling, some arguments hardcoded for now:


int main(int argc, char *argv[])
{
    char *unreadable_program_argument_chars;
    int seq_length;

    if(argc != 2)
    {
        //TODO : application name
        fprintf(stderr, "1 program argument expected! Usage: \"./applicationName\" <seq_length>\n");
        return EXIT_FAILURE;
    }

    seq_length = (int) strtol(argv[1], &unreadable_program_argument_chars, 10);

    if(seq_length <= 0 || strlen(unreadable_program_argument_chars) > 0)
    {
        fprintf(stderr, "Invalid program argument: given argument is not an integer or equals 0\n");
        return EXIT_FAILURE;
    }

    buffer buff_1;
    buffer buff_2;

    gen_hw_init(&buff_1, &buff_2, NULL, NULL);


    int samples_read;

    while(1)
    {
        samples_read = gen_read_from_source1(&buff_1);
        printf("Samples read: %d\n", samples_read);
        print_uint8_t_array(stdout, buff_1.buff, samples_read);
    }

    while(1)
    {
        samples_read=gen_read_from_source2(&buff_2);
        if (samples_read >= buff_2.size)
        {
            fprintf(stderr, "Buffer overflow might have occurred (%zu or more data received).\n"
                    "Enlarge SERIAL_DEV_BUFF_SIZE or slow down incoming transmission speed.", buff_2.size);
        }
        //printf("\nREAD: %d\n", samples_read);
        write(1, buff_2.buff, samples_read);
    }

    gen_hw_close(&buff_1, &buff_2, NULL, NULL);

    return EXIT_SUCCESS;
}