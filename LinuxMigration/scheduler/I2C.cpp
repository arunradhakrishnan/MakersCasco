#include "I2C.h"

int I2C::beginTransmission() {
  // Open a connection to the I2C user space control file
  char i2cDeviceFile[15] = "";
  sprintf(i2cDeviceFile, "%s%d", I2C_DEVICE_FILE, I2C_6);
  if ((i2cDeviceFileDescriptor = open(i2cDeviceFile, O_RDWR)) < 0) {
    printf("Unable to open i2c device file %s", i2cDeviceFile);
    return i2cDeviceFileDescriptor;
  }

  int returnCode = prepareGPIOMode(I2C_6);
  if (returnCode < 0) {
    close(i2cDeviceFileDescriptor);
    return returnCode;
  }

  return returnCode;
}

void I2C::endTransmission() {
  setGPIOMode(GPIO_I2C_6_SDA, originalGPIOModeSDA);
  setGPIOMode(GPIO_I2C_6_SCL, originalGPIOModeSCL);
  close(i2cDeviceFileDescriptor);
}

int I2C::getI2CRegister16(unsigned char addr,
	                      unsigned char reg,
	                      unsigned char *lsb,
                        unsigned char *msb) {
	unsigned char inbuf[2];
	unsigned char outbuf;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	/* In order to read a register, we first do a "dummy write" by writing
	* 0 bytes to the register we want to read from.  This is similar to
	* the packet in setI2Cegister, except it's 1 byte rather than 2.
	*/
	outbuf = reg;
	messages[0].addr = addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = &outbuf;

	// The data will get returned in this structure. Two consecutive bytes are
  // going to be read, the first one in inbuf[0], and the second one in
  // inbuf[1]. We only need to pass the address of the first byte in the array.
	messages[1].addr = addr;
	messages[1].flags = I2C_M_RD;
	messages[1].len = sizeof(inbuf);
	messages[1].buf = &inbuf[0];

	// Send the request to the kernel and get the result back
	packets.msgs = messages;
	packets.nmsgs = 2;    // Number of bytes are to be read

  int returnCode = ioctl(i2cDeviceFileDescriptor, I2C_RDWR, &packets);
  if (returnCode < 0) {
    printf("Unable to read register %x from I2C device %x\n", reg, addr);
		return returnCode;
	}

  *lsb = inbuf[0];
  *msb = inbuf[1];

	return 0;
}

int I2C::setI2CRegister(unsigned char addr,
                        unsigned char reg,
                        unsigned char value) {
	unsigned char outbuf[2];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];

	// The first byte indicates which register will be written
	outbuf[0] = reg;

	messages[0].addr = addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(outbuf);
	messages[0].buf = outbuf;

  /*
  * The second byte indicates the value to write. Note that for many
  * devices, we can write multiple, sequential registers at once by
  * simply making outbuf bigger.
  */
  outbuf[1] = value;

	// Transfer the i2c packets to the kernel and verify it worked
	packets.msgs = messages;
	packets.nmsgs = 1;
  int returnCode = ioctl(i2cDeviceFileDescriptor, I2C_RDWR, &packets);
	if (returnCode < 0) {
    printf("Unable to write data %x in register %x for I2C device %x\n", value, reg, addr);
		return returnCode;
	}

	return 0;
}

int I2C::prepareGPIOMode(unsigned int i2c) {
  /* Get the current mode of the GPIO associated with the selected I2C to
  * re-establish it before ending the program.
  */
  originalGPIOModeSCL = getGPIOMode(i2c_gpioMap[i2c][I2C_SCL]);
  if (originalGPIOModeSCL < 0) {
    printf("Unable to get the current mode for I2C SCL\n");
    printf("(from %sgpio%d/%s)\n", GPIO_DEBUG_PATH, i2c_gpioMap[i2c][I2C_SCL], GPIO_CPINMUX_FILE);
    return originalGPIOModeSCL;
  }

  /* Set the GPIO mode to I2C
  */
  int returnCode = setGPIOMode(i2c_gpioMap[i2c][I2C_SCL], 1);
  if (returnCode < 0) {
    printf("Unable to set mode1 (I2C) for SCL\n");
    printf("(in %sgpio%d/%s)\n", GPIO_DEBUG_PATH, i2c_gpioMap[i2c][I2C_SCL], GPIO_CPINMUX_FILE);
    return returnCode;
  }

  /* Get the current mode of the GPIO associated with the serial data line
  * (SDA) for the selected I2C to re-establish it before ending the program.
  */
  originalGPIOModeSDA = getGPIOMode(i2c_gpioMap[i2c][I2C_SDA]);
  if (originalGPIOModeSDA < 0) {
    printf("Unable to get the current mode for I2C SDA\n");
    printf("(from %sgpio%d/%s)\n", GPIO_DEBUG_PATH, i2c_gpioMap[i2c][I2C_SDA], GPIO_CPINMUX_FILE);
    setGPIOMode(i2c_gpioMap[i2c][I2C_SCL], originalGPIOModeSCL);
    return originalGPIOModeSDA;
  }

  /* Set the GPIO mode to I2C
  */
  returnCode = setGPIOMode(i2c_gpioMap[i2c][I2C_SDA], 1);
  if (returnCode < 0) {
    printf("Unable to set mode1 (I2C) for SDA\n");
    printf("(in %sgpio%d/%s)\n", GPIO_DEBUG_PATH, i2c_gpioMap[i2c][I2C_SDA], GPIO_CPINMUX_FILE);
    setGPIOMode(i2c_gpioMap[i2c][I2C_SCL], originalGPIOModeSDA);
    return returnCode;
  }

  return 0;
}

int I2C::getGPIOMode(unsigned int i2c) {
  char gpioMode[6] = "";
  int returnCode = readFile(GPIO_DEBUG_PATH, "gpio", i2c,
    GPIO_CPINMUX_FILE, gpioMode, sizeof(gpioMode));

  if (returnCode < 0) {
    return returnCode;
  }

  return atoi(&gpioMode[4]);
}

int I2C::setGPIOMode(unsigned int i2c, unsigned int gpioMode) {
  char value[10] = "";
  sprintf(value, "mode%d", gpioMode);

  int returnCode = writeFile(GPIO_DEBUG_PATH, "gpio", i2c, GPIO_CPINMUX_FILE, value);

  if (returnCode < 0) {
    return returnCode;
  }

  return 0;
}

int I2C::readFile(char path[], char type[], unsigned int num,
                  char file[], char value[], size_t strSize) {
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

int I2C::writeFile(char path[], char type[], unsigned int num,
                   char file[], char value[]) {
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


