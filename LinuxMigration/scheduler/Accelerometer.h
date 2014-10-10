#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H


#include "Wire.cpp"

#define ACCELEROMETER_ADDRESS   0x1D    // Decimal 29
#define POWER_CTL_REGISTER      0x2D    // Power-saving features control. Decimal 45
#define DATAX0_REGISTER         0x32    // X-Axis Data 0. Decimal 50
#define DATAY0_REGISTER         0x34    // Y-Axis Data 0. Decimal 52
#define DATAZ0_REGISTER         0x36    // Z-Axis Data 0. Decimal 54

#define ONE_BYTE  1
#define TWO_BYTES 2
#define RELEASE   true
#define BAUD_RATE 9600
#define IN_G      0       // 0 to deliver x,y,z acceleration m/s^2,  1 to deliver it in gravitational force (g)


typedef char byte; 
class Accelerometer {
	public:
		Accelerometer();
		void getData();
		float x, y, z;
	private:
		int getAccelerometerRegisterData16(byte);
		int getAccelerometerRegisterData(byte);
		void setAccelerometerRegisterData(byte, byte);
};

#endif
