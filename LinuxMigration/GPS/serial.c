#include "serial.h"


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
	fd=open("/dev/ttyMFD1", O_RDWR | O_NONBLOCK);
	if (fd)
	{
		printf("Could open device:0X%x\n",fd);
	}
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

