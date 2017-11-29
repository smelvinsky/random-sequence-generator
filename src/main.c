/**
 *  Created by smelvinsky on 21.10.17.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>
#include <getopt.h>
#include "utils/buffer_utils.h"
#include "data_sources/generator.h"
#include "utils/save_to_file.h"
#include "utils/array_utils.h"

#define MAX_BUFFER_LENGTH 1048576

/* Uncomment for thread access debug */
#define THREAD_DEBUG

/* Uncomment to save the output to file */
#define SAVE_TO_FILE

/* Uncomment for stdout output */
#define STD_OUTPUT

/* Uncomment for R/W debug */
//#define RW_DEBUG

/* Uncomment for runtime debug */
#define RUNTIME_DEBUG

typedef struct thread_arg_struct
{
    int *seq_length;
    buffer *buff_src;
    FILE *fd;
    size_t *bytes_written;

} thread_arg_struct_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *soundcard_thread(void *arg)
{
    thread_arg_struct_t *thread_arg_struct1 = arg;

    int samples_read;

    while (*(thread_arg_struct1->bytes_written) < *(thread_arg_struct1->seq_length))
    {
        #ifdef THREAD_DEBUG
        printf("Reading from soundcard thread...\n");
        #endif //THREAD_DEBUG

        samples_read = gen_read_from_source1(thread_arg_struct1->buff_src);

        #ifdef RW_DEBUG
        printf("%d bytes read from soundcard\n", samples_read);
        #endif //RW_DEBUG

        //Start critical section
        pthread_mutex_lock(&mutex);

        if (samples_read < (*(thread_arg_struct1->seq_length) - *(thread_arg_struct1->bytes_written)))
        {
            #ifdef SAVE_TO_FILE
            fwrite(thread_arg_struct1->buff_src->buff, 1, (size_t) samples_read, thread_arg_struct1->fd);
            #endif //SAVE_TO_FILE
            #ifdef STD_OUTPUT
            print_uint8_t_array(stdout, thread_arg_struct1->buff_src->buff, samples_read);
            #endif //STD_OUTPUT
            *(thread_arg_struct1->bytes_written) = *(thread_arg_struct1->bytes_written) + samples_read;
        } else
        {
            #ifdef SAVE_TO_FILE
            fwrite(thread_arg_struct1->buff_src->buff, 1, *(thread_arg_struct1->seq_length) - *(thread_arg_struct1->bytes_written), thread_arg_struct1->fd);
            #endif //SAVE_TO_FILE
            #ifdef STD_OUTPUT
            print_uint8_t_array(stdout, thread_arg_struct1->buff_src->buff, *(thread_arg_struct1->seq_length) - (int) *(thread_arg_struct1->bytes_written));
            #endif //STD_OUTPUT
            *(thread_arg_struct1->bytes_written) = *(thread_arg_struct1->bytes_written) + (*(thread_arg_struct1->seq_length) - *(thread_arg_struct1->bytes_written));
        }

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *uart_thread(void *arg)
{
    thread_arg_struct_t *thread_arg_struct2 = arg;

    int samples_read;

    while (*(thread_arg_struct2->bytes_written) < *(thread_arg_struct2->seq_length))
    {
        #ifdef THREAD_DEBUG
        printf("Reading from uart/arduino thread...\n");
        #endif

        samples_read = gen_read_from_source2(thread_arg_struct2->buff_src);
        if (samples_read >= thread_arg_struct2->buff_src->size)
        {
            fprintf(stderr, "Buffer overflow might have occurred (%zu or more data received).\n"
                    "Enlarge SERIAL_DEV_BUFF_SIZE or slow down incoming transmission speed.", thread_arg_struct2->buff_src->size);
        }

        #ifdef RW_DEBUG
        printf("%d bytes read from uart/arduino\n", samples_read);
        #endif //RW_DEBUG

        //Start critical section
        pthread_mutex_lock(&mutex);

        if (samples_read < (*(thread_arg_struct2->seq_length) - *(thread_arg_struct2->bytes_written)))
        {
            #ifdef SAVE_TO_FILE
            fwrite(thread_arg_struct2->buff_src->buff, 1, (size_t) samples_read, thread_arg_struct2->fd);
            #endif //SAVE_TO_FILE
            #ifdef STD_OUTPUT
            print_uint8_t_array(stdout, thread_arg_struct2->buff_src->buff, samples_read);
            #endif //STD_OUTPUT
            *(thread_arg_struct2->bytes_written) = *(thread_arg_struct2->bytes_written) + samples_read;
        } else
        {
            #ifdef SAVE_TO_FILE
            fwrite(thread_arg_struct2->buff_src->buff, 1, *(thread_arg_struct2->seq_length) - *(thread_arg_struct2->bytes_written), thread_arg_struct2->fd);
            #endif //SAVE_TO_FILE
            #ifdef STD_OUTPUT
            print_uint8_t_array(stdout, thread_arg_struct2->buff_src->buff, *(thread_arg_struct2->seq_length) - *(thread_arg_struct2->bytes_written));
            #endif //STD_OUTPUT
            *(thread_arg_struct2->bytes_written) = *(thread_arg_struct2->bytes_written) + (*(thread_arg_struct2->seq_length) - *(thread_arg_struct2->bytes_written));
        }

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void print_usage()
{
    printf("Usage: ./rand-seq-gen -l <seq_length> | ./rand-seq-gen -h (for help)\n");
}

void print_help()
{
    printf("Help: \n\n"
                   "\tInfo:\n"
                   "\t\tTrue Random Number Generator uses 2 non-deterministic data sources to generate \033[1;37mtrue random\033[0m sequence of given length.\n"
                   "\t\t- 1st source: The application uses \033[1;32mA\033[0mdvanced \033[1;32mL\033[0minux \033[1;32mS\033[0mound \033[1;32mA\033[0mrchitecture \033[1;32m(ALSA)\033[0m driver (which is a part of Linux kernel)\n"
                   "\t\t  to get access to the default soundcard of the system and then collects PCM samples. The more noisy soundcard is the better. \n"
                   "\t\t  Remember to adjust the input volume in system settings to get better results.\n"
                   "\t\t  Warning: Some soundcards tend to be not noisy at all, esp. some fancy USB Audio Interfaces,\n"
                   "\t\t  so in worst case they will return sequences of only 0s and 255s.\n"
                   "\t\t- 2nd source: Generator receives the UART signal from \033[1;36mArduino\033[0m which samples\n"
                   "\t\t  analog output from 2 Avalanche Noise Generators (Schematics and arduino sketch in .../rand_seq_gen/arduino).\n"
                   "\t\t  (any noise generator giving the 0-5V output can be used).\n"
                   "\tUsage:\n"
                   "\t\tTrue Random Number Generator program requires one parameter.\n"
                   "\t\tRun the program with: \033[1;35m./rand-seq-gen -l <seq_len>\033[0m where the \033[1;35m<seq_len>\033[0m is the length of output sequence in bytes.\n\n");
}

int main(int argc, char **argv)
{

    #ifdef RUNTIME_DEBUG
    struct timespec begin, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    #endif //RUNTIME_DEBUG

    if(argc < 2 || argc > 3)
    {
        print_usage();
        exit(2);
    }

    /*  main program argument/variable - length of random sequence  */
    int seq_length = 0;
    size_t buffer_mem_to_alloc;

    /*  getopt() variables  */
    int opt;
    int opt_flag = 0;

    char *unreadable_program_argument_chars;

    while ((opt = getopt(argc, argv, "h::l:")) != -1)
    {
        switch (opt)
        {
            case 'h':
                if (opt_flag == 1 || argc > 2)
                {
                    printf("Program requires 1 argument only.\n");
                    print_usage();
                    exit(2);
                }
                print_help();
                exit(2);

            case 'l':
                if (opt_flag == 1 || argc > 3)
                {
                    printf("Program requires 1 argument only.\n");
                    print_usage();
                    exit(2);
                }
                seq_length = (int) strtol(optarg, &unreadable_program_argument_chars, 10);
                if(seq_length <= 0 || strlen(unreadable_program_argument_chars) > 0)
                {
                    fprintf(stdout, "Invalid -l argument: given argument is not an integer or equals 0\n");
                    print_usage();
                    exit(2);
                }
                opt_flag = 1;
                break;

            default:
                print_usage();
                exit(2);
        }
    }

    if (opt_flag == 0)
    {
        fprintf(stdout, "Invalid argument: given argument is not an integer or equals 0\n");
        print_usage();
        exit(2);
    }

    /* generate name and open file */
    FILE *fd;
    char *file_name;
    file_name = malloc(40);
    generate_file_name(&file_name);
    fd = create_and_open_file(file_name);
    free(file_name);

    buffer buff_data;
    size_t bytes_written = 0;

    if (seq_length < MAX_BUFFER_LENGTH)
    {
        buffer_mem_to_alloc = (size_t) seq_length;
        buff_data.size = buffer_mem_to_alloc;
        buff_data.buff = malloc(buffer_mem_to_alloc);
    } else
    {
        buffer_mem_to_alloc = MAX_BUFFER_LENGTH;
        buff_data.size = MAX_BUFFER_LENGTH;
        buff_data.buff = malloc(MAX_BUFFER_LENGTH);
    }
    if (buff_data.buff == NULL)
    {
        fprintf(stderr, "Could not allocate %d of memory with malloc() (%s)", (int) buffer_mem_to_alloc , strerror(errno));
    }

    buffer buff_src_2;
    buffer buff_src_1;
    gen_hw_init(&buff_src_1, &buff_src_2);

    pthread_t tid1; //thread ID for soundcard thread
    pthread_t tid2; //thread ID for uart/arduino thread

    thread_arg_struct_t thread_arg_struct1 = {.buff_src = &buff_src_1, .fd = fd, .seq_length = &seq_length, .bytes_written = &bytes_written};
    thread_arg_struct_t thread_arg_struct2 = {.buff_src = &buff_src_2, .fd = fd, .seq_length = &seq_length, .bytes_written = &bytes_written};

    pthread_create(&tid1, NULL, soundcard_thread, &thread_arg_struct1);
    pthread_create(&tid2 ,NULL, uart_thread, &thread_arg_struct2);

    /* Wait until all the threads are done */
    pthread_join(tid1 ,NULL);
    pthread_join(tid2, NULL);

    gen_hw_close(&buff_src_1, &buff_src_2);
    free(buff_data.buff);
    close_file(fd);

    #ifdef RUNTIME_DEBUG
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_nsec - begin.tv_nsec) / 1000000000.0);
    printf("Program runtime: %f s\n", elapsed);
    #endif //RUNTIME_DEBUG

    return EXIT_SUCCESS;
}