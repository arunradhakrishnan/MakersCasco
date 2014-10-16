#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define GPIO_EXPORT_FILE    "/sys/class/gpio/export"
#define GPIO_UNEXPORT_FILE  "/sys/class/gpio/unexport"
#define GPIO_DIRECTION_FILE "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_FILE     "/sys/class/gpio/gpio%d/value"
#define GPIO_NGPIO_FILE     "/sys/class/gpio/gpiochip0/ngpio"
#define INPUT  "in"
#define OUTPUT "out"
#define HIGH '1'
#define LOW  '0'

int read_gas_gpio(void);
int getNumberOfGPIOs(void);
char toggleGPIO(unsigned int, char);
void pinMode(int, char*);
int exportGPIO(int);
int unexportGPIO(int);
int digitalRead(int);
void digitalWrite(int, char);

