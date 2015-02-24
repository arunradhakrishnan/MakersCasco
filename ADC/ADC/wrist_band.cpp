//////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
  #include "Adc.cpp"
  #include "Adc.h"
  #include "Scheduler.h"
  using namespace std;

//////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////
Scheduler scheduler(1000);
int exitCode=0;
//////////////////////////////////////////////////////////////////
// Global Constants
//////////////////////////////////////////////////////////////////
//Scheduler time declaration for each task, note: REFRESH_ADC_MS, 
//REFRESH_GET_DATA_NN_MS and REFRESH_PROCESS_NN_MS are for adc task
//which is a serial proccess devided in three functions.
const int REFRESH_ADC_MS            = 20;
const int REFRESH_GET_DATA_NN_MS = 10;
const int REFRESH_PROCESS_NN_MS = 100;

const float MAGNITUDE_THRESHOLDUP   = 1500;
const float MAGNITUDE_THRESHOLDLOW  = 500;    // Original value used when trained: 6

const int NN_INPUTSIZE = 50;
float adcBuffer[NN_INPUTSIZE];
//////////////////////////////////////////////////////////////////
// Arduino Prototype header definition
//////////////////////////////////////////////////////////////////
void setup();
void loop();

//////////////////////////////////////////////////////////////////
// Periodic task definition
//////////////////////////////////////////////////////////////////
void refreshAdc();

//////////////////////////////////////////////////////////////////
// Function definition
//////////////////////////////////////////////////////////////////
void getDataNN();
void processNN();

//////////////////////////////////////////////////////////////////
// ADC configuration values
//////////////////////////////////////////////////////////////////
int ain1_operational_status =                         0b0;
int ain1_input_multiplexer_configuration =            0b101;
int ain1_programmable_gain_amplifier_configuration =  0b010;
int ain1_device_operating_mode =                      0b0;
int ain1_date_rate =                                  0b100;
int ain1_comparator_mode =                            0b0;
int ain1_compulator_polarity =                        0b0;
int ain1_latching_comparator  =                       0b0;
int ain1_comparator_queue_and_disable =               0b11;
Adc ain1;

//////////////////////////////////////////////////////////////////
// main function
//////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // run the setup program
    setup();  
    while(1)
    {
        // keep looping
        loop();
    }

    // return exit code
    exit(exitCode);
}

//////////////////////////////////////////////////////////////////
// Declaration of functions
//////////////////////////////////////////////////////////////////
void setup()
{
    // Initial Tasks
    ain1.set_config_command(
            ain1_operational_status, 
            ain1_input_multiplexer_configuration, 
            ain1_programmable_gain_amplifier_configuration, 
            ain1_device_operating_mode, 
            ain1_date_rate, 
            ain1_comparator_mode, 
            ain1_compulator_polarity, 
            ain1_latching_comparator,
            ain1_comparator_queue_and_disable
            );
    scheduler.schedule(&refreshAdc, REFRESH_ADC_MS);   //4 diferentes 
}

void loop()
{
    scheduler.update();  
}

void refreshAdc() 
{  
  float magnitude = ain1.adc_read();
  std::cout<<magnitude<<endl;

  if ( (magnitude < MAGNITUDE_THRESHOLDLOW)||(magnitude > MAGNITUDE_THRESHOLDUP))
  { 
      adcBuffer[0] = magnitude;
      scheduler.schedule(getDataNN, REFRESH_GET_DATA_NN_MS);   
  }
  scheduler.schedule(refreshAdc, REFRESH_ADC_MS);
}

void getDataNN() 
{
  int static sampleCount = 1;

  adcBuffer[sampleCount] = ain1.adc_read();
  std::cout<<adcBuffer[sampleCount]<<endl;
  sampleCount++;

  if (sampleCount == NN_INPUTSIZE) 
  { 
      sampleCount = 1;
      processNN();
  }
  else 
  {
      scheduler.schedule(getDataNN, REFRESH_GET_DATA_NN_MS);
  }
}

void processNN() 
{
  scheduler.schedule(refreshAdc, REFRESH_ADC_MS);
}