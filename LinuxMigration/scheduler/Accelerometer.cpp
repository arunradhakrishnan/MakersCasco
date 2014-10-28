#include "Accelerometer.h"

I2C i2c;

Accelerometer::Accelerometer() {

}

void Accelerometer::setup() {
	setAccelerometerRegisterData(POWER_CTL_REGISTER, 0x00);                 // Go into standby mode to configure the device
	setAccelerometerRegisterData(DATA_FORMAT_REGISTER, 0x03);               // ±16 g, 10-bit resolution
	setAccelerometerRegisterData(THRESH_ACT_REGISTER, THRESH_ACT_VALUE);    // 62.5 mg per unit (62.5mg/LSB)
	setAccelerometerRegisterData(FIFO_CTL_REGISTER, 0x80);                  // Hold the last 32 data values and overwrite the oldest data when full
	setAccelerometerRegisterData(POWER_CTL_REGISTER, 0x08);                 // Start measurement
}

void Accelerometer::setAccelerometerRegisterData(byte accelerometerRegister, byte data) {
  i2c.beginTransmission();
  i2c.setI2CRegister(ACCELEROMETER_ADDRESS, accelerometerRegister, data);
  i2c.endTransmission();
}

int Accelerometer::getAccelerometerRegisterData16(byte accelerometerRegister) {
  unsigned int LSB = 0;
  unsigned int MSB = 0;
  int registerData = 0;
  unsigned char value;

  i2c.beginTransmission();
  i2c.getI2CRegister(ACCELEROMETER_ADDRESS, accelerometerRegister, &value);
  LSB = value;
  i2c.getI2CRegister(ACCELEROMETER_ADDRESS, accelerometerRegister + 1, &value);
  MSB = value;
  i2c.endTransmission();

  if (MSB >> 7) registerData = 0xFFFF0000;
  else          registerData = 0x0;

  registerData |= LSB;
  registerData |= MSB << 8;

  return registerData;
}

void Accelerometer::getData() {
  int xValueRead, yValueRead, zValueRead;

  xValueRead = getAccelerometerRegisterData16(DATAX0_REGISTER);
  yValueRead = getAccelerometerRegisterData16(DATAY0_REGISTER);
  zValueRead = getAccelerometerRegisterData16(DATAZ0_REGISTER);

  if (IN_G) {
    x = xValueRead * SCALE_FACTOR;
    y = yValueRead * SCALE_FACTOR;
    z = zValueRead * SCALE_FACTOR;
  } else {
    x = xValueRead * SCALE_FACTOR * STANDARD_GRAVITY;
    y = yValueRead * SCALE_FACTOR * STANDARD_GRAVITY;
    z = zValueRead * SCALE_FACTOR * STANDARD_GRAVITY;
  }
}

