#ifndef I2C_H
#define I2C_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
//#include <sys/stat.h>

//#define I2C_FILE_NAME "/dev/i2c-6"
#define I2C_DEVICE_FILE    "/dev/i2c-"
#define GPIO_DEBUG_PATH    "/sys/kernel/debug/gpio_debug/"
#define GPIO_CPINMUX_FILE  "current_pinmux"
#define GPIO_I2C_6_SCL  27
#define GPIO_I2C_6_SDA  28
#define I2C_6    6
#define I2C_SCL  0
#define I2C_SDA  1
#define GPIO19  19
#define GPIO20  20
#define GPIO27  27
#define GPIO28  28

class I2C {
	public:
//		void beginTransmission();
    int beginTransmission();
		void endTransmission();
		int getI2CRegister(unsigned char, unsigned char, unsigned char*);
		int setI2CRegister(unsigned char, unsigned char, unsigned char);
//		int i2cDeviceFileDescriptor;
	private:
    int i2cDeviceFileDescriptor;
    int originalGPIOModeSCL = 0;
    int originalGPIOModeSDA = 0;
    const unsigned int i2c_gpioMap[7][2] = {
      { 0, 0 },
      { GPIO19, GPIO20 },
      { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
      { GPIO27, GPIO28 }
    };
    int prepareGPIOMode(unsigned int);
    int getGPIOMode(unsigned int i2c);
    int setGPIOMode(unsigned int i2c, unsigned int gpioMode);
    int readFile(char path[], char type[], unsigned int num,
                 char file[], char value[], size_t strSize);
    int writeFile(char path[], char type[], unsigned int num,
                  char file[], char value[]);
};

#endif

