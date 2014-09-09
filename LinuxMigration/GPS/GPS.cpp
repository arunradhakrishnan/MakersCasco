#include "GPS.h"
#include "serial.h"

static int uart_fd;

GPS::GPS(){
	itsAGPRMCMessage = 0;
	dataStatus = 'V';  // A = valid data, V = invalid data
	uart_fd = uart_init(9600);// The default baud rate for this GPS device is 9600
}

char GPS::getData(){
    float lat, lon = 0.0;
    unsigned long fixAge = 0;
	char* uart_buffer[256]={};
	int ctr = 0;

    for (unsigned long initialTime = millis(); millis() - initialTime < PARSE_TIME;)
    uart_read(uart_buffer,256);
    while (uart_buffer[ctr] != NULL){
		char charRead = uart_buffer[ctr];
        GPSDevice.encode(charRead);
	    if ((charRead == ',') && (GPSDevice._term_number == 1)) {
            itsAGPRMCMessage = !GPSDevice.gpsstrcmp(GPSDevice._term, _GPRMC_TERM);
        }

        if ((charRead == 'A' || charRead == 'V') && itsAGPRMCMessage) {
            dataStatus = charRead;
            itsAGPRMCMessage = 0;
        }
		ctr++;
    }
	GPSDevice.f_get_position(&lat, &lon, &fixAge);
	sprintf(latitude, "%.4f", lat);
	sprintf(longitude, "%.4f", lon);
	sprintf(longitude, "%.1f", GPSDevice.f_altitude());
	
  return dataStatus;
}

