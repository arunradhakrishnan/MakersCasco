#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <Arduino.h>
#include "Wire.h"

#define ACCELEROMETER_ADDRESS   0x1D    // Decimal 29
#define OFSX_REGISTER           0x1E    // X-axis offset. Decimal 30
#define OFSY_REGISTER           0x1F    // Y-axis offset. Decimal 31
#define OFSZ_REGISTER           0x20    // Z-axis offset. Decimal 32
#define THRESH_ACT_REGISTER     0x24    // Activity threshold. Decimal 36
#define ACT_INACT_CTL_REGISTER  0x27    // Axis enable control for activity and inactivity detection. Decimal 39
#define POWER_CTL_REGISTER      0x2D    // Power-saving features control. Decimal 45
#define INT_ENABLE_REGISTER     0x2E    // Interrupt enable control. Decimal 46
#define INT_SOURCE_REGISTER     0x30    // Source of interrupts. Decimal 48
#define DATA_FORMAT_REGISTER    0x31    // Data format control. Decimal 49
#define DATAX0_REGISTER         0x32    // X-Axis Data 0. Decimal 50
#define DATAY0_REGISTER         0x34    // Y-Axis Data 0. Decimal 52
#define DATAZ0_REGISTER         0x36    // Z-Axis Data 0. Decimal 54
#define FIFO_CTL_REGISTER       0x38    // FIFO control. Decimal 56

#define ONE_BYTE             1
#define TWO_BYTES            2
#define RELEASE              true
#define BAUD_RATE            9600
#define IN_G                 0          // 0 to deliver x,y,z acceleration m/s^2,  1 to deliver it in gravitational force (g)
#define OFFSET_FACTOR        1          // mg/LSB. 0.5 for 16g, 1 for 8g, 2 for 4g, 4 for 2g
#define NUM_AXES             3
#define BUFFER_SIZE          64         // Preferrable an even value
#define SCALE_FACTOR         0.03125    // Scale factor at 16g (31.25 mg/LSB), 10-bit resolution
#define STANDARD_GRAVITY     9.80665    // The standard acceleration due to gravity (standard acceleration of free fall)
#define THRESH_ACT_MASK      0x10       // Mask to filter activity interrupt flag
#define THRESH_ACT_VALUE     0x02       // This value goes into the THRESH_ACT register


class Accelerometer {
	public:
		Accelerometer();
		void setup();
		void getData();
		float x, y, z;
	private:
		int buffer[NUM_AXES][BUFFER_SIZE];
		int axes[NUM_AXES];
		const byte coordinates[NUM_AXES] = { DATAX0_REGISTER, DATAY0_REGISTER, DATAZ0_REGISTER };
		int buffer_pos[NUM_AXES] = { 0 };

		int getAccelerometerRegisterData(byte);
		void setAccelerometerRegisterData(byte, byte);
		int readAxes(byte);
		int getX();
		int getY();
		int getZ();
		int get_axis(const byte);
};

#endif