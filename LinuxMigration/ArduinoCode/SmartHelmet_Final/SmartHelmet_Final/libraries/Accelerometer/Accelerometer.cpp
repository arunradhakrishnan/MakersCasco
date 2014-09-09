#include <Accelerometer.h>

Accelerometer::Accelerometer(){

}

void Accelerometer::setup(){
    //Wire.begin();

	setAccelerometerRegisterData(POWER_CTL_REGISTER, 0x00);                 // Go into standby mode to configure the device
	setAccelerometerRegisterData(DATA_FORMAT_REGISTER, 0x03);               // ±16 g, 10-bit resolution
	setAccelerometerRegisterData(THRESH_ACT_REGISTER, THRESH_ACT_VALUE);    // 62.5 mg per unit (62.5mg/LSB)
	setAccelerometerRegisterData(FIFO_CTL_REGISTER, 0x80);                  // Hold the last 32 data values and overwrite the oldest data when full
	setAccelerometerRegisterData(ACT_INACT_CTL_REGISTER, 0xF0);             // Enable activity detection on x, y, z using ac
	setAccelerometerRegisterData(INT_ENABLE_REGISTER, 0x10);                // Enable interrupt for activity
	setAccelerometerRegisterData(POWER_CTL_REGISTER, 0x08);                 // Start measurement

//	// Set offset value for x, y, and z
//	setAccelerometerRegisterData(OFSX_REGISTER, 0x00);
//	setAccelerometerRegisterData(OFSY_REGISTER, 0x00);
//	setAccelerometerRegisterData(OFSZ_REGISTER, 0x00);
//	for (byte i = 0; i < BUFFER_SIZE; i++) {    // Fill in each axis buffer
//		readAxes(axes);
//		buffer[0][i] = axes[0];    // X axis buffer
//		buffer[1][i] = axes[1];    // Y axis buffer
//		buffer[2][i] = axes[2];    // Z axis buffer
//	}

	// float X0g, Y0g, Z0g = 0;
	// byte X0g_Calib, Y0g_Calib, Z0g_Calib;
	//
	// Get the average value for X, as well its offset value
	//  for (byte i = 0; i < BUFFER_SIZE; i++) X0g += buffer[0][i];
	////Serial.println(X0g);
	//  X0g /= BUFFER_SIZE;
	//  X0g_Calib = round((X0g/OFFSET_FACTOR)) * (-1);
	//  
	//  // Get the average value for Y, as well its offset value
	//  for (byte i = 0; i < BUFFER_SIZE; i++) Y0g += buffer[1][i];
	////Serial.println(Y0g);
	//  Y0g /= BUFFER_SIZE;
	//  Y0g_Calib = round((Y0g/OFFSET_FACTOR)) * (-1);
	// 
	//  // Get the average value for Z, as well its offset value
	//  for (byte i = 0; i < BUFFER_SIZE; i++) Z0g += buffer[2][i];
	////Serial.println(Z0g);
	//  Z0g /= BUFFER_SIZE;
	//  Z0g_Calib = round((Z0g/OFFSET_FACTOR)) * (-1); 
	//
	//  // Set the offset value
	//  setAccelerometerRegisterData(OFSX_REGISTER, X0g_Calib);
	//  setAccelerometerRegisterData(OFSY_REGISTER, Y0g_Calib);
	//  setAccelerometerRegisterData(OFSZ_REGISTER, Z0g_Calib);

}

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

void Accelerometer::getData() {
	int xValueRead, yValueRead, zValueRead;
//	int activity;
//
//	activity = getAccelerometerRegisterData(INT_SOURCE_REGISTER);
//	activity = activity & THRESH_ACT_MASK;

//	for (byte i = 0; i < BUFFER_SIZE / 2; i++)
//		readAxes(axes);

	xValueRead = getX();
	yValueRead = getY();
	zValueRead = getZ();

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

//void Accelerometer::readAxes(int axesContainer[]) {
//	Wire.beginTransmission(ACCELEROMETER_ADDRESS);                     // Start transimission to device
//	Wire.write(DATAX0_REGISTER);                                       // Specify address to start reading from
//	Wire.requestFrom(ACCELEROMETER_ADDRESS, NUM_AXES * 2, RELEASE);    // Prepare data to be read, two bytes per axis
//
//	// Read register data
//	unsigned int tempContainer[NUM_AXES * 2];
//	byte i = 0;
//	while (Wire.available()) {				// Device may send less bytes than requested
//		tempContainer[i] = Wire.read();		// Read a byte from the device, starting at register address
//		i++;								// specified by registerAddress, then increment the array pointer
//	}										// to store the next byte in the next location of the array
//	Wire.endTransmission();
//
//	for (i = 0; i < NUM_AXES * 2; i += 2) {
//		if (tempContainer[i + 1] >> 7) axesContainer[i / 2] = 0xFFFF0000;
//		else                         axesContainer[i / 2] = 0x0;
//		axesContainer[i / 2] |= tempContainer[i];
//		axesContainer[i / 2] |= tempContainer[i + 1] << 8;
//	}
//}

int Accelerometer::readAxes(byte accelerometerRegister) {
  unsigned int LSB, MSB;
  int registerData=0x0;
  
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


//int Accelerometer::getX() { return get_axis(0); }
//int Accelerometer::getY() { return get_axis(1); }
//int Accelerometer::getZ() { return get_axis(2); }

int Accelerometer::getX() { return readAxes(DATAX0_REGISTER); }
int Accelerometer::getY() { return readAxes(DATAY0_REGISTER); }
int Accelerometer::getZ() { return readAxes(DATAZ0_REGISTER); }

//int Accelerometer::get_axis(const byte axis) {
//	buffer[axis][buffer_pos[axis]] = axes[axis];
//	buffer_pos[axis] = (buffer_pos[axis] + 1) % BUFFER_SIZE;
//	long sum = 0;
//	for (unsigned int i = 0; i < BUFFER_SIZE; i++)
//		sum += buffer[axis][i];
//	return round(sum / BUFFER_SIZE);
//}
