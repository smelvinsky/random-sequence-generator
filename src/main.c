/**
 *  Created by smelvinsky on 21.10.17.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include <pthread.h>
#include "utils/array_utils.h"
#include "utils/buffer_utils.h"
#include "data_sources/generator.h"
#include "utils/save_to_file.h"

#define MEGABYTE_LENGTH 1048576

FILE *fd;
size_t bytes_written;
int seq_length;
buffer buff_data;
buffer buff_src_2;
buffer buff_src_1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *soundcard_thread()
{
    int samples_read;

    while (bytes_written < seq_length)
    {
        samples_read = gen_read_from_source1(buff_src_1.buff);

        //Start critical section
        pthread_mutex_lock(&mutex);

        if (samples_read < (seq_length - bytes_written)) {
            fwrite(buff_src_1.buff, 1, (size_t) samples_read, fd);
            bytes_written = bytes_written + samples_read;
        } else {
            fwrite(buff_src_1.buff, 1, seq_length - bytes_written, fd);
            bytes_written = bytes_written + (seq_length - bytes_written);
        }

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *uart_thread()
{
    int samples_read;

    while (bytes_written < seq_length)
    {
        samples_read = gen_read_from_source2(buff_src_2.buff);
        if (samples_read >= buff_src_2.size) {
            fprintf(stderr, "Buffer overflow might have occurred (%zu or more data received).\n"
                    "Enlarge SERIAL_DEV_BUFF_SIZE or slow down incoming transmission speed.", buff_src_2.size);
        }

        //Start critical section
        pthread_mutex_lock(&mutex);

        if (samples_read < (seq_length - bytes_written)) {
            fwrite(buff_src_2.buff, 1, (size_t) samples_read, fd);
            bytes_written = bytes_written + samples_read;
        } else {
            fwrite(buff_src_2.buff, 1, seq_length - bytes_written, fd);
            bytes_written = bytes_written + (seq_length - bytes_written);
        }

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    char *unreadable_program_argument_chars;
    size_t buffer_mem_to_alloc;

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

    fd = create_and_open_file(generate_file_name());

    if (seq_length < MEGABYTE_LENGTH)
    {
        buffer_mem_to_alloc = (size_t) seq_length;
        buff_data.size = buffer_mem_to_alloc;
        buff_data.buff = malloc(buffer_mem_to_alloc);
    } else
    {
        buffer_mem_to_alloc = MEGABYTE_LENGTH;
        buff_data.size = MEGABYTE_LENGTH;
        buff_data.buff = malloc(MEGABYTE_LENGTH);
    }
    if (buff_data.buff == NULL)
    {
        fprintf(stderr, "Could not allocate %d of memory with malloc() (%s)", (int) buffer_mem_to_alloc , strerror(errno));
    }

    gen_hw_init(&buff_src_1, &buff_src_2);

    pthread_t tid1; //thread ID for soundcard thread
    pthread_t tid2; //thread ID for uart/arduino thread

    pthread_create(&tid1, NULL, soundcard_thread, NULL);
    pthread_create(&tid2 ,NULL, uart_thread, NULL);

    /* Wait until all the threads are done */
    pthread_join(tid1 ,NULL);
    pthread_join(tid2, NULL);

    gen_hw_close(&buff_src_1, &buff_src_2);
    free(buff_data.buff);
    close_file(fd);

    return EXIT_SUCCESS;
}