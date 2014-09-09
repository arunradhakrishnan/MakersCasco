/*
  GasSensor.h - Library for getting parts per million (PPM) from a gas sensor
  Created by Roberto Burgos, August 15, 2014.
  Released into the public domain.
*/

#ifndef GasSensor_h
#define GasSensor_h

#include "Arduino.h"

/************************Hardware Related Macros************************************/
#define RL_VALUE            10      // Define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR 9.83    // Sensor resistance in clean air divided by RO,
                                    // which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define CALIBARAION_SAMPLE_TIMES     50     // Define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL  500    // Define the time interal(in milisecond) between each samples in the
                                            // cablibration phase.
#define READ_SAMPLE_INTERVAL         50     // Define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES            5      // Define the time interal(in milisecond) between each samples in 
                                            // normal operation.
 
/**********************Application Related Macros**********************************/
#define GAS_ALCOHOL 0
#define GAS_CH4     1    // Methane
#define GAS_CO      2    // Carbon Monoxide
#define GAS_H2      3    // Hydrogen
#define GAS_LPG     4    // Liquefied petroleum gas
#define GAS_PROPANE 5
#define GAS_SMOKE   6

class GasSensor {
  public:
    GasSensor(unsigned int);
    int getPPM(unsigned int);
    float calibrate();
  private:
    unsigned int _sensorPin;
    float _Ro = 10;    // Ro is initialized to 10 kilo ohms

    float _AlcoholCurve[2] = {3853.746539, -2.633253385};
    float _CH4Curve[2]     = {4524.382036, -2.590944711};
    float _COCurve[2]      = {37444.59219, -3.145699043};
    float _H2Curve[2]      = {1020.73003, -2.079398433};
    float _LPGCurve[2]     = {611.5979632, -2.099631015};
    float _PropaneCurve[2] = {670.5984364, -2.127489866};
    float _SmokeCurve[2]   = {4183.080446, -2.273031185};


    float calculateSensorResistance(unsigned int );
    float readSensor(unsigned int);
    int getPercentage(float, float[]);
};

#endif
