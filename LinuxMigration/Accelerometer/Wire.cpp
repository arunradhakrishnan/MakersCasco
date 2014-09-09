
#include "Wire.h"

int i2c_openadapter(uint8_t i2c_adapter_nr)
{
	int i2c_fd;
	char buf[MAX_BUFFER];
	snprintf(buf, sizeof(buf), "/dev/i2c-%d", i2c_adapter_nr);
	if ((i2c_fd = open(buf, O_RDWR)) < 1) {
		perror("Failed to open adapter");
		return -1;
	}
	return i2c_fd;
}

int i2c_setslave(int i2c_fd, uint8_t addr)
{
	if (ioctl(i2c_fd, I2C_SLAVE_FORCE, addr) < 0) {
		fprintf(stderr, "Failed to set slave address %d:", addr);
		perror("");
		return -1;
	}
	return 0;
}

int i2c_readbyte(int i2c_fd, uint8_t *byte)
{
	if (read(i2c_fd,byte,1) == 1) {
		return 0;
		}
		return -1;
}

int i2c_readbytes(int i2c_fd, uint8_t *buf, uint8_t length)
{
	if (read(i2c_fd, buf, length) == length)
		return length;
	else
		return -1;
}

int beginTransmission(uint8_t address)
{
	int i2c_fd;
	uint8_t adapter = 3;
	i2c_fd = i2c_openadapter(adapter);
	i2c_setslave(i2c_fd, address);
	return i2c_fd;
}

/*
//unused
uint8_t read(uint8_t i2c_fd, uint8_t *address, uint8_t quantity)
{
//	i2c_setslave(i2c_fd, address);
	if (i2c_readbytes(i2c_fd, address, quantity) < 0)
		return 0;
	return 1;
}
*/
void i2c_write(int i2c_fd, uint8_t *data)
{
	if(write(i2c_fd,data,1) !=1)
	{
		printf("Failed to write\n");
	}
}

int main()
{
	return 0;
}
