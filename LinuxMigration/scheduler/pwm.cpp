#include "pwm.h"

const unsigned int pwm_gpioMap[] = { GPIO12, GPIO13, GPIO182, GPIO183 };

int set_gas_pwm() {
    /* Determine how many PWMs are available in the system    */
    int numberOfPWMs = getNumberOfPWMs();
    unsigned int pwm = 0; //PCB is using PWM0

    if (numberOfPWMs < 1) {
        printf("\nNo PWMs found! (%s)\n", PWM_NPWM_FILE);
        return -1; 
    }      
    if ((pwm + 1) < numberOfPWMs) {
        printf("\nPWM channel was not foud! (%s)\n", PWM_NPWM_FILE);
        return -1;
    }

    /* Get the current mode of the GPIO associated with the selected
    ** PWM to re-establish it before ending the program. */
    int originalGPIOMode = getGPIOMode(pwm);
    if (originalGPIOMode < 0) {
        printf("Unable to get the current mode for pwm&d\n", pwm);
        printf("(from %sgpio%d/%s)\n", GPIO_DEBUG_PATH, pwm_gpioMap[pwm], GPIO_CPINMUX_FILE);
        return -1;
    }

    /* Set the GPIO mode to PWM	*/
    int returnCode = setGPIOMode(pwm, 1);
    if (returnCode < 0) {
        printf("Unable to set mode1 (PWM) for pwm&d\n", pwm);
        printf("(in %sgpio%d/%s)\n", GPIO_DEBUG_PATH, pwm_gpioMap[pwm], GPIO_CPINMUX_FILE);
        return -1;
    }

    /* Before using the selected PWM, it needs to be available in the Linux
    * user space. To make the PWM availabe, its number must be written in
    * its associated export file. */
    returnCode = exportPWM(pwm);
    if (returnCode < 0) {
        printf("Unable to export pwm%d (%s)\n", pwm, PWM_EXPORT_FILE);
        setGPIOMode(pwm, originalGPIOMode);
        return -1;
    }

    /* Get the current PWM period to re-establish it before ending the program */
    int originalPWMPeriod = getPWMPeriod(pwm);
    if (originalPWMPeriod < 0) {
        printf("Unable to determine the current period for pwm%d\n", pwm);
        printf("(from %spwm%d/%s)\n", PWM_CHIP_PATH, pwm, PWM_PERIOD_FILE);
        unexportPWM(pwm);
        setGPIOMode(pwm, originalGPIOMode);
        return -1;
    }

    /* Get the PWM period from the user. This period is the duration of each
    * cycle, in nanoseconds. For ejample, to specify a cycle of 1 milisecond
    * the value 1000000 must be used.
    * The program will end if the user provides a number either less than 104
    * or greater than 218453000.
    * If the Enter key is pressed without entering a value, the suggested
    * value in < > will be used.*/
    int period = 5000000;
    
    if ((period > 0 && period < 104) ||
        (period > 218453000)) {    // The user opted to exit
        unexportPWM(pwm);
        setGPIOMode(pwm, originalGPIOMode);
        //exit(EXIT_FAILURE);
        return -1;
    }

    returnCode = setPWMPeriod(pwm, period);
    if (returnCode < 0) {
        printf("Unable to set the period for pwm%d\n", pwm);
        printf("(in %spwm%d/%s)\n", PWM_CHIP_PATH, pwm, PWM_PERIOD_FILE);
        printf("The existing value will be used: %d\n", originalPWMPeriod);
    }

    returnCode = enablePWM(pwm);
    if (returnCode < 0) {
        printf("It was not possible to enable the pwm%d\n", pwm);
        printf("(from %spwm%d/%s)\n", PWM_CHIP_PATH, pwm, PWM_ENABLE_FILE);
        unexportPWM(pwm);
        setGPIOMode(pwm, originalGPIOMode);
        //exit(EXIT_FAILURE);
        return -1;
    }

    /* Get the duty cycle from the user, in the range of 0 - 255.
    * The duty cycle is the amount of time the pulse will be 1.
    * For example, if the pulse goes from 0 to 5 volts, and the duty cycle is
    * set to 50%, the output voltage will reach 2.5 volts; a duty cycle of 20%
    * will yield an output voltage of 1 volt; a duty cycle of 80% will yield
    * an output voltage of 4 #include <fcntl.h>volts, and so on.*/
    unsigned int dutyCycle = 250;
    do {
        if (dutyCycle > 255) {    // The user opted to exit
            disablePWM(pwm);
            unexportPWM(pwm);
            setGPIOMode(pwm, originalGPIOMode);
            return 0;
        }
        analogWrite(pwm, dutyCycle);
    } while (dutyCycle <= 255);
}


int getNumberOfPWMs() {
    int fileDescriptor;
    char numberOfPWMs[2] = "";
    int returnCode = 0; 

    fileDescriptor = open(PWM_NPWM_FILE, O_RDONLY);
    if (fileDescriptor > -1) {
        returnCode = read(fileDescriptor, &numberOfPWMs, 1);
        close(fileDescriptor);
        if (returnCode < 0) {
            return -1;
        }
        return atoi(numberOfPWMs);
    }
    else {
        return fileDescriptor;
    }
}


/*****************************************************
* The functions below are neded for the PWM
* functionality. All the other functions (above)
* are just used to test the correct functionality
* of the functions below.
****************************************************/

int getGPIOMode(unsigned int pwm) {
    char gpioMode[6] = "";
    int returnCode = readFile(GPIO_DEBUG_PATH, "gpio", pwm_gpioMap[pwm],
        GPIO_CPINMUX_FILE, gpioMode, sizeof(gpioMode));
    if (returnCode < 0) {
        return returnCode;
    }

    return atoi(&gpioMode[4]);
}


int setGPIOMode(unsigned int pwm, unsigned int gpioMode) {
    char value[10] = "";
    sprintf(value, "mode%d", gpioMode);

    int returnCode = writeFile(GPIO_DEBUG_PATH, "gpio", pwm_gpioMap[pwm], GPIO_CPINMUX_FILE, value);

    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


int exportPWM(unsigned int pwm) {
    char value[2] = "";
    sprintf(value, "%u", pwm);

    int fileDescriptor = open(PWM_EXPORT_FILE, O_WRONLY);
    if (fileDescriptor < 0) {
        return fileDescriptor;
    }

    int returnCode = write(fileDescriptor, value, strlen(value));
    close(fileDescriptor);

    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


int unexportPWM(unsigned int pwm) {
    char value[2] = "";
    sprintf(value, "%d", pwm);

    int fileDescriptor = open(PWM_UNEXPORT_FILE, O_WRONLY);

    if (fileDescriptor < 0) {
        return fileDescriptor;
    }

    int returnCode = write(fileDescriptor, value, strlen(value));
    close(fileDescriptor);

    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}



int getPWMPeriod(unsigned int pwm) {
    char pwmPeriod[10] = "";
    int returnCode = readFile(PWM_CHIP_PATH, "pwm", pwm, PWM_PERIOD_FILE,
        pwmPeriod, sizeof(pwmPeriod));

    if (returnCode < 0) {
        return returnCode;
    }

    return atoi(pwmPeriod);
}


int setPWMPeriod(unsigned int pwm, int pwmPeriod) {
    char value[15] = "";
    sprintf(value, "%d", pwmPeriod);

    int returnCode = writeFile(PWM_CHIP_PATH, "pwm", pwm, PWM_PERIOD_FILE, value);
    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


int enablePWM(unsigned int pwm) {
    char value[2] = "";
    sprintf(value, "%d", 1);

    int returnCode = writeFile(PWM_CHIP_PATH, "pwm", pwm, PWM_ENABLE_FILE, value);

    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


int disablePWM(unsigned int pwm) {
    char value[2] = "";
    sprintf(value, "%d", 0);

    int returnCode = writeFile(PWM_CHIP_PATH, "pwm", pwm, PWM_ENABLE_FILE, value);
    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


void analogWrite(unsigned int pwm, unsigned int pwmDutyCycle) {
    if ((pwmDutyCycle >= 0) &&
        (pwmDutyCycle <= 255)) {
        char value[15] = "";
        float pwmdc = (pwmDutyCycle / 255.0) * getPWMPeriod(pwm);
        sprintf(value, "%d", (int)pwmdc);
        writeFile(PWM_CHIP_PATH, "pwm", pwm, PWM_DUTY_CYCLE_FILE, value);
    }
}


int readFile(char path[], char type[], unsigned int num, char file[], char value[], size_t strSize){
    char buf[55] = "";
    sprintf(buf, "%s%s%d/%s", path, type, num, file);
    int fileDescriptor = open(buf, O_RDONLY);

    if (fileDescriptor < 0) {
        return fileDescriptor;
    }

    int returnCode = read(fileDescriptor, value, strSize);
    close(fileDescriptor);
    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}


int writeFile(char path[], char type[], unsigned int num, char file[], char value[]) {
    char buf[55] = "";
    sprintf(buf, "%s%s%d/%s", path, type, num, file);

    int fileDescriptor = open(buf, O_WRONLY);
    if (fileDescriptor < 0) {
        return fileDescriptor;
    }

    sprintf(buf, "%s", value);
    int returnCode = write(fileDescriptor, buf, strlen(buf));
    close(fileDescriptor);

    if (returnCode < 0) {
        return returnCode;
    }

    return 0;
}
