/**
 *  Created by smelvinsky on 13.11.17.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include "termios.h"

#include "uart.h"

#define SERIAL_DEBUG
//#define SERIAL_DEBUG_RW

#define SERIAL_DEV_BUFF_SIZE 2048

void serial_dev_init(serial_dev *serial_dev, serial_dev_config *serial_dev_conf)
{
    #ifdef SERIAL_DEBUG
    printf("==> Serial device initialization...\n");
    fflush(stdout);
    #endif

    if(!serial_dev)
    {
        fprintf(stderr, "serial_dev pointer cannot be NULL (in \"serial_dev_init\" function, uart.c)\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if(!serial_dev_conf)
    {
        fprintf(stderr, "serial_dev_config pointer cannot be NULL (in \"serial_dev_init\" function, uart.c)\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    serial_dev->serial_dev_conf = serial_dev_conf;

    #ifdef SERIAL_DEBUG
    printf("==> Serial device initialized:\n\tDevice name: %s\n", serial_dev->serial_dev_conf->serial_dev_name);
    fflush(stdout);
    #endif
}

void serial_dev_open(serial_dev *serial_dev)
{
    #ifdef SERIAL_DEBUG
    printf("==> Opening Serial device...\n");
    fflush(stdout);
    #endif

    if(!serial_dev)
    {
        fprintf(stderr, "serial_dev pointer cannot be NULL (in \"serial_dev_open\" function, uart.c)\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    /* The argument flags must include one of the following access modes: */
    /* O_RDONLY, O_WRONLY, or O_RDWR. These request opening               */
    /* the file read-only, write-only, or read/write, respectively.       */
    serial_dev->serial_dev_fd = open(serial_dev->serial_dev_conf->serial_dev_name, *(serial_dev->serial_dev_conf->serial_dev_access_mode)
                                                                                   | *(serial_dev->serial_dev_conf->serial_dev_exe_perm));

    if (serial_dev->serial_dev_fd == -1)
    {
        fprintf(stderr, "Cannot open \"%s\" device (%s)\nPlease check if your device is properly detected or if is being used by other software.\n"
                "\"Useful commands: \"dmesg | grep tty\"\n(\"Permission denied\" might indicate that r/w permit of the device have not been given for your user:\n"
                "To fix it, enter the commands as root:\n"
                "$ sudo usermod -a -G dialout <username>\n"
                "$ sudo chmod a+rw /dev/ttyACM0)\n", serial_dev->serial_dev_conf->serial_dev_name, strerror(errno));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }


    #ifdef SERIAL_DEBUG
    printf("==> Serial device opened:\n\t\tGiven file descriptor: %d\n", serial_dev->serial_dev_fd);
    #endif
}

size_t serial_dev_set_parameters(serial_dev *serial_dev, void **dst_buff)
{
    #ifdef SERIAL_DEBUG
    printf("==> Setting serial port parameters...\n");
    #endif

    if(!serial_dev)
    {
        fprintf(stderr, "serial_dev pointer cannot be NULL (in \"serial_dev_set_parameters\" function, uart.c)\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if(!(*dst_buff))
    {
        fprintf(stderr, "(dst_buffer pointer) pointer cannot be NULL (in \"serial_dev_set_parameters\" function, uart.c)\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    *dst_buff = malloc(SERIAL_DEV_BUFF_SIZE);
    memset(*dst_buff, 0, malloc_usable_size(*dst_buff));

    #ifdef SERIAL_DEBUG
    printf("\t==> Buffer allocated with size of %d\n", (int) malloc_usable_size(*dst_buff));
    #endif

    /* Set options configure structure */
    struct termios t_options;
    memset(&t_options, 0, sizeof(t_options));

    /* Set the baud rate */
    cfsetispeed(&t_options, *(serial_dev->serial_dev_conf->serial_dev_baud_rate));

    /* c_cflag - control mode flags     */
    /* 8 bits, no parity, no stop bits  */
    t_options.c_cflag |= CS8;
    t_options.c_cflag &= ~PARENB;
    t_options.c_cflag &= ~CSTOPB;

    /* no hardware flow control */
    t_options.c_cflag &= ~CRTSCTS;

    /* enable receiver */
    t_options.c_cflag |= CREAD;

    /* ignore modem status lines */
    t_options.c_cflag |= CLOCAL;

    /* c_iflag - input mode flags           */
    /* disable input/output flow control    */
    t_options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* c_lflag - local mode flags               */
    /* disable canonical input, disable echo,   */
    /* disable visually erase chars,            */
    /* disable terminal-generated signals       */
    t_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* wait for 12 characters to come in before read returns    */
    /* WARNING! THIS CAUSES THE read() TO BLOCK UNTIL ALL       */
    /* CHARACTERS HAVE COME IN!                                 */
    t_options.c_cc[VMIN] = 8;
    /* no minimum time to wait before read returns */
    t_options.c_cc[VTIME] = 0;

    /* commit options */
    if (tcsetattr(serial_dev->serial_dev_fd, TCSANOW, &t_options) < 0)
    {
        fprintf(stderr, "cannot set terminal options (%s) (in \"serial_dev_set_parameters\" function, uart.c)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Flush anything already in the serial buffer */
    if (tcflush(serial_dev->serial_dev_fd, TCIFLUSH) != 0)
    {
        fprintf(stderr, "cannot flush serial buffer (%s) (in \"serial_dev_set_parameters\" function, uart.c)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    #ifdef SERIAL_DEBUG
    printf("==> Serial buffer flushed.\n");
    #endif

    #ifdef SERIAL_DEBUG
    printf("==> Serial port hardware parameters successfully prepared.\n");
    #endif

    return SERIAL_DEV_BUFF_SIZE;
}

ssize_t serial_dev_read(serial_dev *serial_dev, void *dst_buff)
{
    #ifdef SERIAL_DEBUG_RW
    printf("==> Reading from Serial device...\n");
    #endif

    ssize_t bytes_read;

    bytes_read = read(serial_dev->serial_dev_fd, dst_buff, SERIAL_DEV_BUFF_SIZE);

    if (bytes_read == -1)
    {
        fprintf(stderr, "error reading from %s (%s)", serial_dev->serial_dev_conf->serial_dev_name, strerror(errno));
    }

    return bytes_read;
}

void serial_dev_close(serial_dev *serial_dev, void **buff)
{
    #ifdef SERIAL_DEBUG
    printf("==> Closing Serial device...\n");
    #endif

    if(!serial_dev)
    {
        fprintf(stderr, "serial_dev pointer cannot be NULL (in \"serial_dev_close\" function, uart.c)\n");
        exit(EXIT_FAILURE);
    }

    /* Closes Serial device by given file descriptor */
    if (close(serial_dev->serial_dev_fd) == -1)
    {
        fprintf(stderr, "cannot close %s device (%s) (in \"serial_dev_close\" function, uart.c)\n", serial_dev->serial_dev_conf->serial_dev_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    #ifdef SERIAL_DEBUG
    printf("==> Serial device closed\n");
    #endif

    free(*buff);

    #ifdef SERIAL_DEBUG
    printf("==> Data buffer freed\n");
    #endif
}