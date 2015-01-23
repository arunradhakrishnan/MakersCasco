#include <stdlib.h>
#include <stdio.h>
#include "Scheduler.h"
#include "Adc.h"
#include "GPS.h"
#include "algoritmos.hpp"
#include <string.h>
#include <math.h>
//////////////////////////////////////////////////////////////////
// Network includes to get the MAC address
// These includes should be refactored into other *.cpp and *.h 
// files so that the main file does not have so much stuff
//////////////////////////////////////////////////////////////////
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>

using namespace std;
//////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////
Scheduler scheduler(1000);
int exitCode=0;
int wiFiTry = 0;
NeuralNetwork net;
//////////////////////////////////////////////////////////////////
// Global Constants
//////////////////////////////////////////////////////////////////
//Scheduler time declaration for each task, note: REFRESH_ADC_MS, 
//REFRESH_GET_DATA_NN_MS and REFRESH_PROCESS_NN_MS are for adc task
//which is a serial proccess devided in three functions.
const int REFRESH_ADC_MS = 20;
const int REFRESH_GET_DATA_NN_MS = 10;
const int REFRESH_PROCESS_NN_MS = 100;
const int CONVERSION_CONST = 1;

const int REFRESH_NET_MS = 1000;
const int RESCHEDULE_MS = 500; //delay to reachedule lower priority tasks

const int NN_INPUTSIZE = 50;
const float MAGNITUDE_THRESHOLDUP = 30.0f;
const float MAGNITUDE_THRESHOLDLOW = 6.0f;    // Original value used when trained: 6
float adcBuffer[NN_INPUTSIZE];

Adc adc = Adc();

//////////////////////////////////////////////////////////////////
// Arduino Prototype header definition
//////////////////////////////////////////////////////////////////
void setup();
void loop();

//////////////////////////////////////////////////////////////////
// Periodic task definition
//////////////////////////////////////////////////////////////////
void refreshNetData();
void refreshAdc();

//////////////////////////////////////////////////////////////////
// Function definition
//////////////////////////////////////////////////////////////////

float getMagnitude();
void getDataNN();
void processNN();
void sendEdisonPackage(char event_type);
//void postJson(string json);
int neuralNetwork(float  *buffer);

int getMACAddress(int iNetType, unsigned char chMAC[6]);


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
    adc.setup();
    net.initNN("algoritmos.net");
    scheduler.schedule(&refreshNetData, REFRESH_NET_MS);
    scheduler.schedule(&refreshAdc, REFRESH_ADC_MS);    
}

void loop()
{
    scheduler.update();  
}

float getMagnitude() {
    float data;
    float value;
    Adc adc = Adc();

    adc.getData();
    data = adc.x;
    value = data * CONVERSION_CONST;
    printf("value:%f, value:%f \n", data, value);

    return vector; 
}

void refreshNetData() {
    sendEdisonPackage(0);
    scheduler.schedule(refreshNetData, REFRESH_NET_MS);
}

void refreshAdc() {  
    float magnitude = getMagnitude();
    
    if ( (magnitude < MAGNITUDE_THRESHOLDLOW) || 
    (magnitude > MAGNITUDE_THRESHOLDUP)) { 
        adcBuffer[0] = magnitude;
        scheduler.schedule(getDataNN, REFRESH_GET_DATA_NN_MS);   
    }
    scheduler.schedule(refreshAdc, REFRESH_ADC_MS);
}

void getDataNN() {
    int static sampleCount = 1;

    adcBuffer[sampleCount] = getMagnitude();
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
  switch(neuralNetwork(adcBuffer))
  {
    case 0://OK
      printf("OK\n");
      break;
    case 1: //FALL
      printf("FALL\n");
      sendEdisonPackage(2);
      sleep(1);
      break;
    case 2://STRIKE
      printf("STRIKE\n");
      sendEdisonPackage(3);
      sleep(1);
      break;
  } 
  scheduler.schedule(refreshAdc, REFRESH_ADC_MS);
}

void sendEdisonPackage(char event_type)
{
    int status;
    unsigned char chMAC[6] = "";
    char system_python_call_str[600] = "/usr/bin/python /home/root/scheduler/push_event.py events ";
    char mac_address[36];
    char mac[6];
    float gps_lat = 0, gps_lon = 0, gps_alt = 0;
    char gps_status = 'A';
    char char_tmp[32];
    char out[200];

    gps_status = gps_get_data(&gps_lat, &gps_lon, &gps_alt);
    status = getMACAddress(1, chMAC); 
    // need to check if it getMACAdress was successfull
    sprintf(mac_address, " %02X:%02X:%02X:%02X:%02X:%02X ",
    	chMAC[0],chMAC[1],chMAC[2],chMAC[3],chMAC[4],chMAC[5]);
    strcat(system_python_call_str, " G \"\" 1 12.25 124.21 > /home/root/scheduler/python_kinesis.log");    
    strcat(system_python_call_str, mac_address);

    switch(event_type){
    case(0):
        strcpy (out, (const char*)"GPS ");
        break;
    case(1):
        strcpy (out, (const char*)"SMOKE ");
        break;
    case(2):
        strcpy (out, (const char*)"FALL ");
        break;
    case(3):
        strcpy (out, (const char*)"STRIKE ");
        break;
    default:
        strcpy (out, (const char*)"NA ");
        break;
    }
    
    strcat(system_python_call_str, (const char*)out);
    strcpy (out, (const char*)"NA ");
    strcat(system_python_call_str, (const char*)out);
    strcat(system_python_call_str, (const char*)&gps_status);
    sprintf(char_tmp, " %f ", gps_lat);
    strcat(system_python_call_str, (const char*)char_tmp);
    sprintf(char_tmp, "%f ", gps_lon);
    strcat(system_python_call_str, (const char*)char_tmp);
    sprintf(char_tmp, "%f ", gps_alt);
    strcat(system_python_call_str, (const char*)char_tmp);

    printf("Calling python program\n");
    printf("%s\n", system_python_call_str);
    status |= system(system_python_call_str);
    printf("Calling done and returned = %d\n", status); 
}

int neuralNetwork(float  *buffer)
{ 
  float fallTreshold, strikeTreshold, *calc_out;
  
  calc_out = net.run(buffer);
  fallTreshold   = calc_out[0];
  strikeTreshold = calc_out[1]; 
  if ((fallTreshold > strikeTreshold) && (fallTreshold > 0.8)) {
    return 1;
  }
  if((fallTreshold < strikeTreshold) && (strikeTreshold > 0.8)) {
    return 2;
  } 
  return 0;
}


/* Returns the MAC Address 
   Params: int iNetType - 0: ethernet, 1: Wifi 
        char chMAC[6] - MAC Address in binary format 
   Returns: 0: success 
           -1: Failure 
*/ 
 
int getMACAddress(int iNetType, unsigned char chMAC[6]) 
{ 
   struct ifreq ifr; 
   int sock; 
   char *ifname=NULL; 
   
   if (!iNetType) { 
           ifname="eth0"; /* Ethernet -- not really used in this code -- */ 
   } else { 
           ifname="wlan0"; /* Wifi */ 
   }   
   
   sock=socket(AF_INET,SOCK_DGRAM,0); 
   strcpy( ifr.ifr_name, ifname ); 
   ifr.ifr_addr.sa_family = AF_INET; 
   if (ioctl( sock, SIOCGIFHWADDR, &ifr ) < 0) { 
           return -1; 
   } 
   memcpy(chMAC, ifr.ifr_hwaddr.sa_data, 6); 
   
   close(sock); 
   return 0; 
}
