#ifndef GPS_H
#define GPS_H

/* This code acquires geographical coordinates using the Smart Design Corp.
   GPS, part number S4F1912, hooked up on Edison Arduino Expansion Board's
   pin 0, which is the serial receiver (Rx) pin.
   
   Reference:
   Smart Design Corp. GPS S4F1912
        http://www.modulestek.com/prodts/GPS/S4F1912/S4F1912.html
   TinyPGS
        http://arduiniana.org/libraries/tinygps/
*/  

//#include <Arduino.h>
#include "TinyGPS.h"
#include "anotherTime.h"

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define BAUD_RATE B9600

#define _GPRMC_TERM "GPRMC"

const unsigned char PRECISION = 4;    // Number of decimals used in messages

int test_function();
char gps_get_data();
void uart_write(char*, int);
void uart_read(char*, int);
void uart_init (int);

#endif

