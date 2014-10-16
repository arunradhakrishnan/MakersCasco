#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define PWM_EXPORT_FILE     "/sys/class/pwm/pwmchip0/export"
#define PWM_UNEXPORT_FILE   "/sys/class/pwm/pwmchip0/unexport"
#define PWM_NPWM_FILE       "/sys/class/pwm/pwmchip0/npwm"
#define GPIO_DEBUG_PATH     "/sys/kernel/debug/gpio_debug/"
#define GPIO_CPINMUX_FILE   "current_pinmux"
#define PWM_PERIOD_FILE     "period"
#define PWM_DUTY_CYCLE_FILE "duty_cycle"
#define PWM_ENABLE_FILE     "enable"
#define PWM_CHIP_PATH       "/sys/class/pwm/pwmchip0/"
#define GPIO12  12
#define GPIO13  13
#define GPIO182 182
#define GPIO183 183

void analogWrite(unsigned int, unsigned int);

/*int getNumberOfPWMs();
int getGPIOMode(unsigned int);
int setGPIOMode(unsigned int, unsigned int);
int exportPWM(unsigned int);
int unexportPWM(unsigned int);
int getPWMPeriod(unsigned int);
int setPWMPeriod(unsigned int, int);
int enablePWM(unsigned int);
int disablePWM(unsigned int);
void analogWrite(unsigned int, unsigned int);
*/
const unsigned int pwm_gpioMap[] = { GPIO12, GPIO13, GPIO182, GPIO183 };

void main() {
    /* Determine how many PWMs are available in the system    */
    int numberOfPWMs = getNumberOfPWMs();
    if (numberOfPWMs < 1) {
        printf("\nNo PWMs found! (%s)\n", PWM_NPWM_FILE);
        exit(EXIT_FAILURE);
    }
    printf("\nNumber of PWMs found: %d\n", numberOfPWMs);

    /* Get the PWM number from the user. The program will end if
    ** the user provides a number greater than the available PWMs */
    unsigned int pwm;
    printf("\nEnter a PWM number from 0 to %d (or greater than %d to exit): ",
        numberOfPWMs - 1, numberOfPWMs - 1);
    scanf("%u", &pwm);
    if (pwm >= numberOfPWMs) {
        exit(EXIT_SUCCESS);
    }

    /* Get the current mode of the GPIO associated with the selected
    ** PWM to re-establish it before ending the program. */
    int originalGPIOMode = getGPIOMode(pwm);
    if (originalGPIOMode < 0) {
        printf("Unable to get the current mode for pwm&d\n", pwm);
        printf("(from %sgpio%d/%s)\n", GPIO_DEBUG_PATH, pwm_gpioMap[pwm], GPIO_CPINMUX_FILE);
        exit(EXIT_FAILURE);
    }

    /* Set the GPIO mode to PWM	*/
    int returnCode = setGPIOMode(pwm, 1);
    if (returnCode < 0) {
        printf("Unable to set mode1 (PWM) for pwm&d\n", pwm);
        printf("(in %sgpio%d/%s)\n", GPIO_DEBUG_PATH, pwm_gpioMap[pwm], GPIO_CPINMUX_FILE);
        exit(EXIT_FAILURE);
    }


    /* Before using the selected PWM, it needs to be available in the Linux
    * user space. To make the PWM availabe, its number must be written in
    * its associated export file. */
    returnCode = exportPWM(pwm);
    if (returnCode < 0) {
        printf("Unable to export pwm%d (%s)\n", pwm, PWM_EXPORT_FILE);
        setGPIOMode(pwm, originalGPIOMode);
        exit(EXIT_FAILURE);
    }

    /* Get the current PWM period to re-establish it before ending the program */
    int originalPWMPeriod = getPWMPeriod(pwm);
    if (originalPWMPeriod < 0) {
        printf("Unable to determine the current period for pwm%d\n", pwm);
        printf("(from %spwm%d/%s)\n", PWM_CHIP_PATH, pwm, PWM_PERIOD_FILE);
        unexportPWM(pwm);
        setGPIOMode(pwm, originalGPIOMode);
        exit(EXIT_FAILURE);
    }

    /* Get the PWM period from the user. This period is the duration of each
    * cycle, in nanoseconds. For ejample, to specify a cycle of 1 milisecond
    * the value 1000000 must be used.
    * The program will end if the user provides a number either less than 104
    * or greater than 218453000.
    * If the Enter key is pressed without entering a value, the suggested
    * value in < > will be used.*/
    int period = -1;
    printf("\nEnter the signal period in nanoseconds, in the range of 104 - 218453000\n"
        "(0 to accept default, any other value to exit) <%d>: ", originalPWMPeriod);
    scanf("%d", &period);

    if (period == 0) {    // The suggested value was selected
        period = originalPWMPeriod;
    }

    if ((period > 0 && period < 104) ||
        (period > 218453000)) {    // The user opted to exit
        unexportPWM(pwm);
        setGPIOMode(pwm, originalGPIOMode);
        exit(EXIT_SUCCESS);
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
        exit(EXIT_FAILURE);
    }

    /* Get the duty cycle from the user, in the range of 0 - 255.
    * The duty cycle is the amount of time the pulse will be 1.
    * For example, if the pulse goes from 0 to 5 volts, and the duty cycle is
    * set to 50%, the output voltage will reach 2.5 volts; a duty cycle of 20%
    * will yield an output voltage of 1 volt; a duty cycle of 80% will yield
    * an output voltage of 4 volts, and so on.*/
    unsigned int dutyCycle = 0;
    do {
        printf("\nEnter a duty cycle in the range of 0 - 255\n"
        "(or greater than 255 to exit): ");
        scanf("%u", &dutyCycle);

        if (dutyCycle > 255) {    // The user opted to exit
            disablePWM(pwm);
            unexportPWM(pwm);
            setGPIOMode(pwm, originalGPIOMode);
            exit(EXIT_SUCCESS);
        }

        analogWrite(pwm, dutyCycle);
    } while (dutyCycle <= 255);
}


int getNumberOfPWMs() {
    int fileDescriptor = open(PWM_NPWM_FILE, O_RDONLY);

    if (fileDescriptor > -1) {
        char numberOfPWMs[2] = "";
        int returnCode = read(fileDescriptor, &numberOfPWMs, 1);
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
