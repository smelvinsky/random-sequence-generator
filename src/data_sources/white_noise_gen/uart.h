/**
 * Created by smelvinsky on 13.11.17.
 */

#ifndef UART_H
#define UART_H

typedef struct serial_dev_config_t
{
    /* CONFIGURABLE BY USER: */

    /** Serial device name
     *  use command "dmesg | grep tty"
     *  for more information about detected devices  */
    const char *serial_dev_name;

    /** Access mode */
    const int *serial_dev_access_mode;

    /** Execute permission */
    const int *serial_dev_exe_perm;

    /** Baud rate */
    const speed_t *serial_dev_baud_rate;

} serial_dev_config;

typedef struct serial_dev_t
{
    /* CONFIGURABLE BY USER: */

    /** Serial device file descriptor */
    int serial_dev_fd;

    /** Serial device configuration structure */
    serial_dev_config* serial_dev_conf;
} serial_dev ;

void serial_dev_init(serial_dev *serial_dev, serial_dev_config *serial_dev_conf);
void serial_dev_open(serial_dev *serial_dev);

/** returns the size of data buffer (not allocated memory !)*/
size_t serial_dev_set_parameters(serial_dev *serial_dev, void **dst_buff);

ssize_t serial_dev_read(serial_dev *serial_dev, void *dst_buff);
void serial_dev_close(serial_dev *serial_dev, void **buff);

#endif //UART_H
