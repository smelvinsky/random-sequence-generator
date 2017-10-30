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
    gen_hw_init(&buff_1, NULL, NULL, NULL);


    ////////////////////////////////////////
    int i = 10;
    while (i > 0)

    {
        i--;
        gen_read_from_source1(&buff_1);
        print_uint8_t_array(stdout, buff_1.buff, (int) buff_1.size);
        printf("\n");
    }
    ///////////////////////////////////////

    gen_hw_close(&buff_1, NULL, NULL, NULL);

    return EXIT_SUCCESS;
}