#include "gpio.h"

int read_gas_gpio() {
    unsigned int gpio;
    int GPIOValue = 0;

    gpio = 15;
    pinMode(gpio, INPUT);
    GPIOValue = digitalRead(gpio);
//    if (GPIOValue < -1) {
//        printf("Can't set gpio:%d .\n", gpio);
        unexportGPIO(gpio);
//        return -1;        
//    }
    return GPIOValue;
}


int getNumberOfGPIOs() {
    int fileDescriptor = open(GPIO_NGPIO_FILE, O_RDONLY);

    if (fileDescriptor > -1) {
        char numberOfGPIOs[4];
        int returnCode = read(fileDescriptor, &numberOfGPIOs, 3);
        close(fileDescriptor);

        if (returnCode < 0) {
            return -1;
        }

        return atoi(numberOfGPIOs);

    }
    else {
        return fileDescriptor;
    }
}


char toggleGPIO(unsigned int gpio, char lastGPIOValue) {
    if (lastGPIOValue == LOW) {
        digitalWrite(gpio, HIGH);
        lastGPIOValue = HIGH;
    }
    else {
        digitalWrite(gpio, LOW);
        lastGPIOValue = LOW;
    }

    return lastGPIOValue;
}










/*****************************************************
* The functions below are neded for the GPIO
* functionality. All the other functions (above)
* are just used to test the correct functionality
* of the functions below.
****************************************************/

void pinMode(int pin, char *mode) {
    int returnCode = exportGPIO(pin);
    if (returnCode < 0) {
        printf("Error exporting GPIO %d. %s\n", pin, strerror(errno));
    }
    else {
        char gpioDirectionFile[34];
        sprintf(gpioDirectionFile, GPIO_DIRECTION_FILE, pin);

        int fileDescriptor = open(gpioDirectionFile, O_WRONLY);

        if (fileDescriptor > -1) {
            returnCode = write(fileDescriptor, mode, strlen(mode));
            close(fileDescriptor);
        }

        if (fileDescriptor < 0 || returnCode < 0) {
            printf("Error setting mode for pin %d. %s\n", pin, strerror(errno));

            if (returnCode < 0) {
                unexportGPIO(pin);
            }
        }
    }
}


int exportGPIO(int gpio) {
    char buf[4];
    sprintf(buf, "%d", gpio);

    int fileDescriptor = open(GPIO_EXPORT_FILE, O_WRONLY);

    if (fileDescriptor > -1) {
        int returnCode = write(fileDescriptor, buf, strlen(buf));
        close(fileDescriptor);

        if (returnCode < 0) {
            return returnCode;
        }
    }

    return fileDescriptor;
}


int unexportGPIO(int gpio) {
    char buf[4];
    sprintf(buf, "%d", gpio);

    int fileDescriptor = open(GPIO_UNEXPORT_FILE, O_WRONLY);

    if (fileDescriptor > -1) {
        int returnCode = write(fileDescriptor, buf, strlen(buf));
        close(fileDescriptor);

        if (returnCode < 0) {
            return returnCode;
        }
    }

    return fileDescriptor;
}


int digitalRead(int pin) {
    char gpioValueFile[34];
    sprintf(gpioValueFile, GPIO_VALUE_FILE, pin);

    int fileDescriptor = open(gpioValueFile, O_RDONLY);

    if (fileDescriptor > -1) {
        char pinValue;
        int returnCode = read(fileDescriptor, &pinValue, 1);
        close(fileDescriptor);

        if (returnCode < 1) {
            return returnCode;
        }

        return pinValue;
    }

    return fileDescriptor;
}


void digitalWrite(int pin, char pinValue) {
    char gpioValueFile[34];
    sprintf(gpioValueFile, GPIO_VALUE_FILE, pin);

    int fileDescriptor = open(gpioValueFile, O_WRONLY);

    if (fileDescriptor > -1) {
        char s_pinValue[2];
        sprintf(s_pinValue, "%c", pinValue);

        write(fileDescriptor, s_pinValue, 1);
        close(fileDescriptor);
    }
}
