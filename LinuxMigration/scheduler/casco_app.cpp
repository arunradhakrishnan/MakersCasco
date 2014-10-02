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
//#include <Accelerometer.h>
//#include <Wire.h>
#include <string>
#include <math.h>

using namespace std;
//////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////
Scheduler scheduler(1000);
int exitCode=0;
int wiFiTry = 0;
float vector;
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
const int DELAY_SETUP_WIFI = 20000; 
const int TIMES_SETUP_WIFI = 4;

const int GAS_TRESHOLD = 100;
unsigned int GAS_PIN = 0;

const int NN_INPUTSIZE = 50;
const float ACCEL_THRESHOLDUP = 30.0f;
const float ACCEL_THRESHOLDLOW = 6.0f;    // Original value used when trained: 6
float accelBuffer[NN_INPUTSIZE];
float dataX, dataY, dataZ;  //version final van en getAccelVector


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
void setupWifi(); 

//////////////////////////////////////////////////////////////////
// Function definition
//////////////////////////////////////////////////////////////////

float getAccelVector();
void getDataNN();
void processNN();
void sendEdisonPackage(string event_type, string value);
void postJson(string json);
int neuralNetwork(float  *buffer);

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
  scheduler.schedule(setupWifi, DELAY_SETUP_WIFI); 
}

void loop()
{
  scheduler.update();  
}

void setupWifi() { 
  //system("./setupwifi > /dev/ttyGS0"); 
  wiFiTry ++;
  if (wiFiTry < TIMES_SETUP_WIFI) {
     scheduler.schedule(setupWifi, DELAY_SETUP_WIFI); 
  }
} 

float getAccelVector() {
  
    //accelerometer.getData();
    //dataX = accelerometer.x;
    //dataY = accelerometer.y;
    //dataZ = accelerometer.z;
    vector = sqrt(dataX*dataX + dataY*dataY + dataZ*dataZ);

  return vector; 
}

void refreshGPS() {
    //sendEdisonPackage("GPS", "");
    scheduler.schedule(refreshGPS, REFRESH_GPS_MS);
}

void refreshGas() {
  //int valueSmoke = gas.getPPM(GAS_SMOKE);
  //if (valueSmoke > GAS_TRESHOLD)
    {
      //sendEdisonPackage("SMOKE", String(valueSmoke));
      printf("Sending smoke alert\n"); 
    }
    scheduler.schedule(refreshGas, REFRESH_GAS_MS);
}

void refreshAccelerometer() {  
  float acceleration = getAccelVector();
  
  if ( (acceleration < ACCEL_THRESHOLDLOW) || (acceleration > ACCEL_THRESHOLDUP)) {
 
    accelBuffer[0] = acceleration;
    scheduler.schedule(getDataNN, REFRESH_GET_DATA_NN_MS);
   
  }
  scheduler.schedule(refreshAccelerometer, REFRESH_ACC_MS);
}

void getDataNN() {
  int static sampleCount = 1;

  accelBuffer[sampleCount] = getAccelVector();
  sampleCount++;

  if (sampleCount == NN_INPUTSIZE) {
    sampleCount = 1;
    processNN();
  }
  else {
    scheduler.schedule(getDataNN, REFRESH_GET_DATA_NN_MS);
  }
}

void processNN() {
/*  switch(neuralNetwork(accelBuffer))
  {
    case 0://OK
      Serial.println("OK");
      break;
    case 1: //FALL
      Serial.println("FALL");
      sendEdisonPackage("FALL", "");
      delay(1000);
      break;
    case 2://STRIKE
      Serial.println("STRIKE");
      sendEdisonPackage("STRIKE", "");
      delay(1000);
      break;
  } */
  scheduler.schedule(refreshAccelerometer, REFRESH_ACC_MS);
}

void sendEdisonPackage(string event_type, string value)
{
/*  String json = "";

  char gps_status = gps.getData();
//  gps_status = 'A';
  
  char mac_address[36];
  byte mac[6];  
  WiFi.macAddress(mac);
  sprintf(mac_address, "%X:%X:%X:%X:%X:%X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  
  json += "{\"mac_address\":\""; json += String(mac_address) + "\",";
  json += "\"event_type\":\"";  json += event_type + "\",";
  json += "\"value\":\"";  json += value + "\",";
  json += "\"gps_status\":\"";  json += String(gps_status) + "\",";
  json += "\"gps_latitude\":\"";  json += String(gps.latitude) + "\",";
  json += "\"gps_longitude\":\"";  json += String(gps.longitude) + "\",";
  json += "\"gps_altitude\":\"";  json += String(gps.altitude) + "\"}";
 
  postJson(json); 
  Serial.print("   data:");
  Serial.println(json); */
}

void postJson(string json) {
/*  WiFiClient client;
  if (client.connect(SERVER_IP, SERVER_PORT)) {
    String httpRequest = "POST /HelmetWebApp/v1/api/new_event.jsp HTTP/1.1";
    Serial.print("connected...");
    Serial.println(httpRequest);
    client.println(httpRequest);
    client.println("Host:  artiswrong.com");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: "); client.println(json.length());
    client.println();
    client.println(json);
  } else {
    Serial.println("connection failed");
  } */
}

int neuralNetwork(float  *buffer)
{ /*
  float fallTreshold, strikeTreshold;
  FILE * dataFile = fopen("neuralInput.txt", "w");  
  for(int count = 0 ; count < NN_INPUTSIZE ; count++) {      
    if (dataFile) {
      fprintf(dataFile, "%f " , buffer[count]);
      //Serial.println(buffer[count]);
    }      
  }
  fclose(dataFile);
    
  system("./smartHelmetAlgNN");
  FILE * resultFile = fopen("neuralOutput.txt", "r");    
  fscanf(resultFile, "%f %f", &fallTreshold, &strikeTreshold);   
  //Serial.println("fallTreshold");
  //Serial.println(fallTreshold);
  //Serial.println("strikeTreshold"); 
  //Serial.println(strikeTreshold); 
  if ((fallTreshold > strikeTreshold) && (fallTreshold > 0.8)) {
    return 1;
  }
  if((fallTreshold < strikeTreshold) && (strikeTreshold > 0.8)) {
    return 2;
  } */
  return 0;
}
