//
// Created by smelvinsky on 21.10.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>   //tmp
#include <string.h>
#include "random.h"
#include "array_utils.h"


int main(int argc, char *argv[])
{
    //tmp:
    srand(time(NULL));

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

    uint8_t random_seq[seq_length];
    generate_uint8_t_random_seq(seq_length, random_seq);
    print_uint8_t_array(stdout, random_seq, seq_length);

    return EXIT_SUCCESS;
}