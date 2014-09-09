////////////////////////////////////////////////////////////////////////////////////////////////
// NAME:
//  casco_arduino.c
// DESCRIPTION:
//  This is a very simple program to verify Arduino scheduller emulation behavior in C
// AUTHOR:
//  Pedro Iniguez Huerta
// COPYRIGHTS:
//	Intel (r) copyrights
// DATE:
//   Sep/07/2014
// HISTORY CHANGES:
// flag Date       Name                 Description
// ---- ---------- -------------------- --------------------------------------------------------
// $A0  09/07/2014 Pedro Iniguez Huerta Created.
////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "Scheduler.h"

//////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////
Scheduler scheduler(1000);
int exitCode=0;

//////////////////////////////////////////////////////////////////
// Global Constants
//////////////////////////////////////////////////////////////////
//Scheduler time declaration for each task, note: REFRESH_ACC_MS, 
//REFRESH_GET_DATA_NN_MS and REFRESH_PROCESS_NN_MS are for accelerometer task
//which is a serial proccess devided in three functions.
const int REFRESH_ACC_MS = 1000; //10;
const int REFRESH_GET_DATA_NN_MS = 10;
const int REFRESH_PROCESS_NN_MS = 100;

const int REFRESH_GAS_MS = 2000;
const int REFRESH_GPS_MS = 3000; //300000;
const int RESCHEDULE_MS = 1000; //delay to reachedule lower priority tasks


//////////////////////////////////////////////////////////////////
// Arduino Prototype header definition
//////////////////////////////////////////////////////////////////
void setup();
void loop();

//////////////////////////////////////////////////////////////////
// Periodic task definition
//////////////////////////////////////////////////////////////////
void refreshGPS();
void refreshGas();
void refreshAccelerometer();

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
  scheduler.schedule(&refreshGPS, REFRESH_GPS_MS);
  scheduler.schedule(&refreshAccelerometer, REFRESH_ACC_MS);
  scheduler.schedule(&refreshGas, REFRESH_GAS_MS);
}

void loop()
{
  scheduler.update();  
}

void refreshGPS()
{
  printf("GPS Sensor monitoring\n");
  scheduler.schedule(&refreshGPS, REFRESH_GPS_MS);
}

void refreshGas()
{
  printf("GAS Sensor monitoring\n");
  scheduler.schedule(&refreshGas, REFRESH_GAS_MS);
}

void refreshAccelerometer()
{
  printf("ACCELEROMETER Sensor monitoring\n");
  scheduler.schedule(&refreshAccelerometer, REFRESH_ACC_MS);
}

