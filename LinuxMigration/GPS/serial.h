#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define BAUD_RATE B9600

void uart_write(char*, int);
void uart_read(char*, int);
void uart_init (int);