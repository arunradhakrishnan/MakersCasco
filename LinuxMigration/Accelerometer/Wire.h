#define uint8_t unsigned char
#define MAX_BUFFER 20
#define I2C_SLAVE_FORCE 0x0706

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int i2c_openadapter(uint8_t i2c_adapter_nr);
int i2c_setslave(int i2c_fd, uint8_t addr);
int i2c_readbyte(int i2c_fd, uint8_t *byte);
int i2c_readbytes(int i2c_fd, uint8_t *buf, uint8_t length);
int beginTransmission(uint8_t address);
void i2c_write(int i2c_fd, uint8_t *data);
