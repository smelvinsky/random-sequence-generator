/**
 *  Created by smelvinsky on 21.10.17.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>
#include "utils/buffer_utils.h"
#include "data_sources/generator.h"
#include "utils/save_to_file.h"
#include "utils/array_utils.h"

#define MAX_BUFFER_LENGTH 1048576

/* Uncomment for thread access debug */
//#define THREAD_DEBUG

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

char* file_save_buff;

void *soundcard_thread(void *arg)
{
    thread_arg_struct_t *thread_arg_struct1 = arg;

    int samples_read;

    while (*(thread_arg_struct1->bytes_written) < *(thread_arg_struct1->seq_length))
    {
        #ifdef THREAD_DEBUG
        printf("Reading from soundcard thread...\n");
        #endif //THREAD_DEBUG

        samples_read = gen_read_from_source1(thread_arg_struct1->buff_src->buff);

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

        samples_read = gen_read_from_source2(thread_arg_struct2->buff_src->buff);
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
            print_uint8_t_array(stdout, thread_arg_struct2->buff_src->buff, *(thread_arg_struct2->seq_length) - (int) *(thread_arg_struct2->bytes_written));
            #endif //STD_OUTPUT
            *(thread_arg_struct2->bytes_written) = *(thread_arg_struct2->bytes_written) + (*(thread_arg_struct2->seq_length) - *(thread_arg_struct2->bytes_written));
        }

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    #ifdef RUNTIME_DEBUG
    clock_t begin = clock();
    #endif //RUNTIME_DEBUG

    char *unreadable_program_argument_chars;
    size_t buffer_mem_to_alloc;

    int seq_length;

    //TODO: make getopt() argument handling, some arguments hardcoded for now:
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

    FILE *fd;
    fd = create_and_open_file(generate_file_name());

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

//    pthread_t tid1; //thread ID for soundcard thread
//    pthread_t tid2; //thread ID for uart/arduino thread
//
//    thread_arg_struct_t thread_arg_struct1 = {.buff_src = &buff_src_1, .fd = fd, .seq_length = &seq_length, .bytes_written = &bytes_written};
//    thread_arg_struct_t thread_arg_struct2 = {.buff_src = &buff_src_2, .fd = fd, .seq_length = &seq_length, .bytes_written = &bytes_written};
//
//    pthread_create(&tid1, NULL, soundcard_thread, &thread_arg_struct1);
//    pthread_create(&tid2 ,NULL, uart_thread, &thread_arg_struct2);
//
//    /* Wait until all the threads are done */
//    pthread_join(tid1 ,NULL);
//    pthread_join(tid2, NULL);


    gen_hw_close(&buff_src_1, &buff_src_2);
    free(buff_data.buff);
    close_file(fd);

    #ifdef RUNTIME_DEBUG
    clock_t end = clock();
    printf("Program runtime: %f ms", (double) (end - begin) * 1000 / CLOCKS_PER_SEC);
    #endif //RUNTIME_DEBUG

    return EXIT_SUCCESS;
}