#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define BAUD_RATE B9600

static void uart_write(char* data, int size);
static void uart_read(char* data, int size);
static void uart_init (int baud);