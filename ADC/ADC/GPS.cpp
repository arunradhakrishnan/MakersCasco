#include "GPS.h"
#define _GPRMC_TERM "GPRMC"

static TinyGPS GPSDevice;
static int itsAGPRMCMessage = 0;
static char dataStatus = 'V';  // A = valid data, V = invalid data

int test_function() {
    float lat, lon, alt;
    while(1) {
        gps_get_data(&lat, &lon, &alt);   
    }
    return 1;
}

char gps_get_data(float* lat, float* lon, float* alt) {
    unsigned long fixAge = 0;
    char uart_buffer[512] = {};
    int ctr = 0;

    uart_init(9600);// The default baud rate for this GPS device is 9600

    uart_read(uart_buffer, sizeof(uart_buffer));
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
    GPSDevice.f_get_position(lat, lon, &fixAge);
	*alt = GPSDevice.f_altitude();
    printf("lat: %.4f\t", *lat);
    printf("lon: %.4f\t", *lon);
    printf("alt: %.1f\t", *alt);
    printf("sta: %c\n", dataStatus);

    return dataStatus;
}

void uart_write(char* data, int size)
{
    int fd;	
	fd=open("/dev/ttyMFD1", O_RDWR | O_NONBLOCK);
    write(fd,data, size);
	close(fd);
}

void uart_read(char* data, int size)
{
    int fd;	
  fd = open("/dev/ttyMFD1", O_RDONLY);
	read(fd,data,size);
	close(fd);
}

void uart_init (int baud)
{
	struct termios tio;
	int fd;
	tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
 
    fd=open("/dev/ttyMFD1", O_RDWR | O_NONBLOCK);
    switch(baud)
	{
	   case (9600):
			cfsetospeed(&tio,B9600);
			cfsetispeed(&tio,B9600);
	   break;
	   case (115200):
			cfsetospeed(&tio,B115200);
			cfsetispeed(&tio,B115200);
	   break;
	   default:
			cfsetospeed(&tio,B9600);
			cfsetispeed(&tio,B9600);
	   break;	   
	}
    
    tcsetattr(fd,TCSANOW,&tio);
	close(fd);
}

