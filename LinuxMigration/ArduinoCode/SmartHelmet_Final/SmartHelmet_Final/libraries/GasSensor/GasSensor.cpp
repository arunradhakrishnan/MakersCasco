/*
  GasSensor.cpp - Library for getting parts per million (PPM) from a gas sensor
  Created by Roberto Burgos, August 15, 2014.
  Released into the public domain.
*/

#include <GasSensor.h>

GasSensor::GasSensor(unsigned int sensorPin) {
  _sensorPin = sensorPin;
}

int GasSensor::getPPM(unsigned int gasID) {
  switch (gasID) {
    case GAS_ALCOHOL:
      return getPercentage(readSensor(_sensorPin)/_Ro, _AlcoholCurve);
    case GAS_CH4:
      return getPercentage(readSensor(_sensorPin)/_Ro, _CH4Curve);
    case GAS_CO:
      return getPercentage(readSensor(_sensorPin)/_Ro, _COCurve);
    case GAS_H2:
      return getPercentage(readSensor(_sensorPin)/_Ro, _H2Curve);
    case GAS_LPG:
      return getPercentage(readSensor(_sensorPin)/_Ro, _LPGCurve);
    case GAS_PROPANE:
      return getPercentage(readSensor(_sensorPin)/_Ro, _PropaneCurve);
    case GAS_SMOKE:
      return getPercentage(readSensor(_sensorPin)/_Ro, _SmokeCurve);
  }
 
  return 0;
}

float GasSensor::calibrate() {  // Calibrating the sensor. Please make sure the sensor is
                                // in clean air when calibrating
  float val = 0;
 
  for (int i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {    // Take multiple samples
    val += calculateSensorResistance(analogRead(_sensorPin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;    // Calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;         // Dividing by RO_CLEAN_AIR_FACTOR yields the Ro 
                                         // according to the chart in the datasheet 
  _Ro = val;

  return _Ro; 
}

float GasSensor::calculateSensorResistance(unsigned int raw_adc) {
  return (((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}

float GasSensor::readSensor(unsigned int sensorPin) {
  float rs = 0;
 
  for (int i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += calculateSensorResistance(analogRead(sensorPin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
  
  return rs;  
}

int GasSensor::getPercentage(float rs_ro_ratio, float pcurve[]){
  int perc = (pcurve[0] * pow(rs_ro_ratio, pcurve[1]));
  if (perc < 0) perc = 1000000;
  return perc;
}


