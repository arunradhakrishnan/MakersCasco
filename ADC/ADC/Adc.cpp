#include "Adc.h"

I2C i2c;

Adc::Adc() {

}

void Adc::setup() {
	setAdcRegisterData(POWER_CTL_REGISTER, 0x00);                 // Go into standby mode to configure the device
	setAdcRegisterData(DATA_FORMAT_REGISTER, 0x03);               // ±16 g, 10-bit resolution
	setAdcRegisterData(THRESH_ACT_REGISTER, THRESH_ACT_VALUE);    // 62.5 mg per unit (62.5mg/LSB)
	setAdcRegisterData(FIFO_CTL_REGISTER, 0x80);                  // Hold the last 32 data values and overwrite the oldest data when full
	setAdcRegisterData(POWER_CTL_REGISTER, 0x08);                 // Start measurement
}

void Adc::setAdcRegisterData(byte adcRegister, byte data) {
  i2c.beginTransmission();
  i2c.setI2CRegister(ADC_ADDRESS, adcRegister, data);
  i2c.endTransmission();
}

int Adc::getAdcRegisterData16(byte adcRegister) {
  unsigned int LSB = 0;
  unsigned int MSB = 0;
  int registerData = 0;
  unsigned char lsb = 0;
  unsigned char msb = 0;

  i2c.beginTransmission();
  i2c.getI2CRegister16(ADC_ADDRESS, adcRegister, &lsb, &msb);
  i2c.endTransmission();

  LSB = lsb;
  MSB = msb;

  if (MSB >> 7) registerData = 0xFFFF0000;
  else          registerData = 0x0;

  registerData |= LSB;
  registerData |= MSB << 8;

  return registerData;
}

void Adc::getData() {
  int xValueRead = 0;
  int yValueRead = 0;
  int zValueRead = 11;

  xValueRead = getAdcRegisterData16(DATAX0_REGISTER);
  yValueRead = getAdcRegisterData16(DATAY0_REGISTER);
  zValueRead = getAdcRegisterData16(DATAZ0_REGISTER);

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

