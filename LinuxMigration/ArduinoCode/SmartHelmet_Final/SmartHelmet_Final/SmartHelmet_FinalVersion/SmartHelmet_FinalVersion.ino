/*
 *   Project: Edison Smart Helmet
 *   Date: October 2014
 *   Company: Intel Corporation
 */
#include <GPS.h>
#include <Scheduler.h>
#include <Accelerometer.h>
#include <Wire.h>
#include <WiFi.h>
#include <GasSensor.h>
#include <stdio.h>

const char* SERVER_IP = "192.168.43.240";
//const char* SERVER_IP = "192.168.1.113";
const int SERVER_PORT = 8084;

const int REFRESH_ACC_MS = 20;
const int REFRESH_GAS_MS = 5000;
const int REFRESH_GPS_MS = 10000;
const int DELAY_SETUP_WIFI = 20000;

const int REFRESH_GET_DATA_NN_MS = 10;
const int REFRESH_PROCESS_NN_MS = 100;

const int TIMES_SETUP_WIFI = 4;
float vector;

/*********************************
* TODO: Put correct GAS treshold *
*********************************/
const int GAS_TRESHOLD = 100;
unsigned int GAS_PIN = 0;

const int NN_INPUTSIZE = 50;
const float ACCEL_THRESHOLDUP = 30.0f;
const float ACCEL_THRESHOLDLOW = 6.0f;    // Original value used when trained: 6
float accelBuffer[NN_INPUTSIZE];
float dataX, dataY, dataZ;  //version final van en getAccelVector


GPS gps = GPS();
GasSensor gas = GasSensor(GAS_PIN);
Scheduler scheduler = Scheduler();
Accelerometer accelerometer = Accelerometer();

/* UTILS */
// function getAccelVector: returns the normalized acceleration from the sensor in m/s^2
float getAccelVector () {
  
    accelerometer.getData();
    dataX = accelerometer.x;
    dataY = accelerometer.y;
    dataZ = accelerometer.z;
    vector = sqrt(dataX*dataX + dataY*dataY + dataZ*dataZ);

  return vector; 

}

//function neuralNetwork: returns the helmet status, 0 ok, 1 fall, 2 hit.
int neuralNetwork(float  *buffer)
{
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
  }
  return 0;
}

// *** Edison JSON Packages *** //
void sendEdisonPackage(String event_type, String value)
{
  String json = "";

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
  Serial.println(json);
}

void postJson(String json) {
  WiFiClient client;
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
  }
}

// Init code
void setup() {  
  Serial.begin(9600);
  delay(2000);  
  Serial.println("Init");
  Wire.begin();
  accelerometer.setup();
  Serial1.begin(9600);  //The default baud rate for the GPS device is 9600
  Serial.println("Starting Edison Helmet..."); 


  //gas.calibrate();
   
  // Initial Tasks //
   scheduler.schedule(setupWifi, DELAY_SETUP_WIFI); 
   scheduler.schedule(refreshGPS, REFRESH_GPS_MS);
   scheduler.schedule(refreshAccelerometer, REFRESH_ACC_MS);
   scheduler.schedule(refreshGas, REFRESH_GAS_MS);
   
}

// *** Loop Tasks - DO NOT EDIT loop - Scheduler will handle all even*** //
void loop() {
  scheduler.update();  
}

int wiFiTry = 0;
void setupWifi() { 
  system("./setupwifi > /dev/ttyGS0"); 
  wiFiTry ++;
  if (wiFiTry < TIMES_SETUP_WIFI) {
     scheduler.schedule(setupWifi, DELAY_SETUP_WIFI); 
  }
} 
 
// *** Scheduler Tasks *** //
 
void refreshGPS () {
    sendEdisonPackage("GPS", "");
    scheduler.schedule(refreshGPS, REFRESH_GPS_MS);
}

void refreshGas() {
  int valueSmoke = gas.getPPM(GAS_SMOKE);
    if (valueSmoke > GAS_TRESHOLD)
    {
      sendEdisonPackage("SMOKE", String(valueSmoke));
      Serial.println("Sending smoke alert"); 
    }
    scheduler.schedule(refreshGas, REFRESH_GAS_MS);
}
   
void refreshAccelerometer() {  
  float acceleration = getAccelVector();
  
  if ( (acceleration < ACCEL_THRESHOLDLOW) || (acceleration > ACCEL_THRESHOLDUP)) {
    /* possible event, getting data to feed the Neural Network */
    //Serial.println(acceleration);
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
  switch(neuralNetwork(accelBuffer))
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
  }
  scheduler.schedule(refreshAccelerometer, REFRESH_ACC_MS);
}
