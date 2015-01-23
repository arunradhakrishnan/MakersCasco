#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

int set_gas_pwm();
int getNumberOfPWMs();
int getGPIOMode(unsigned int);
int setGPIOMode(unsigned int, unsigned int);
int exportPWM(unsigned int);
int unexportPWM(unsigned int);
int getPWMPeriod(unsigned int);
int setPWMPeriod(unsigned int, int);
int enablePWM(unsigned int);
int disablePWM(unsigned int);
void analogWrite(unsigned int, unsigned int);
int readFile(char path[], char type[], unsigned int num, char file[], char value[], size_t strSize);
int writeFile(char path[], char type[], unsigned int num, char file[], char value[]);
