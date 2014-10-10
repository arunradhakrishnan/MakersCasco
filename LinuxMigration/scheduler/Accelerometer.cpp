#include "Accelerometer.h"

Accelerometer::Accelerometer(){
	setAccelerometerRegisterData(POWER_CTL_REGISTER, 0x08);
}

// The code in loop() shows how to use the getData function
// Use getData() in your code, just pass it a char[30] variable
// Usage: 
// char parsedValues[30] = "";
// getData(parsedValues);
// Serial.println(parsedValues);

void Accelerometer::setAccelerometerRegisterData(byte accelerometerRegister, byte data) {
  Wire.beginTransmission(ACCELEROMETER_ADDRESS);
  Wire.write(accelerometerRegister);
  Wire.write(data);
  Wire.endTransmission();
}

int Accelerometer::getAccelerometerRegisterData(byte accelerometerRegister) {
  Wire.beginTransmission(ACCELEROMETER_ADDRESS);
  Wire.write(accelerometerRegister);
  Wire.endTransmission();

  Wire.requestFrom(ACCELEROMETER_ADDRESS, ONE_BYTE, RELEASE);
  return Wire.read();
}

int Accelerometer::getAccelerometerRegisterData16(byte accelerometerRegister) {
  unsigned int LSB, MSB;
  int registerData;
  
  Wire.beginTransmission(ACCELEROMETER_ADDRESS);
  Wire.write(accelerometerRegister);
  Wire.endTransmission();

  Wire.requestFrom(ACCELEROMETER_ADDRESS, TWO_BYTES, RELEASE);
  LSB = Wire.read();
  MSB = Wire.read();
  
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
    x = xValueRead * 0.004;
    y = yValueRead * 0.004;
    z = zValueRead * 0.004;
  } else {
    x = xValueRead * 0.004 * 9.80665;
    y = yValueRead * 0.004 * 9.80665;
    z = zValueRead * 0.004 * 9.80665;
  }
}


