/*
 * TwoWire.h - TWI/I2C library for Linux Userspace
 * Copyright (c) 2013 Parav https://github.com/meanbot.
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Modified by Denny Yang on 25 Sep 2013:
 *              - added x86 support
 */

#define MAX_BUFFER 20
#define I2C_SLAVE_FORCE 0x0706
#define I2C_SMBUS   0x0720
#define ushort_t unsigned short
#define byte_t unsigned char
#define I2C_SMBUS_WRITE   0
#define I2C_SMBUS_BYTE   1
#define I2C_SMBUS_I2C_BLOCK_MAX   32
#define I2C_SMBUS_I2C_BLOCK_DATA   6

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/i2c-dev.h>

extern "C" {
#include <string.h>
}

#include "Wire.h"

#define MY_TRACE_PREFIX "Wire"

unsigned int i2c_buf_count = 0;

union i2c_smbus_data_t {
	byte_t 	byte;
	ushort_t word;
	byte_t block [I2C_SMBUS_I2C_BLOCK_MAX+2];
};

struct i2c_smbus_ioctl_data_t {
	byte_t 	read_write;
	byte_t 	command;
	int 	size;
	i2c_smbus_data_t* data;
};

struct i2c_msg {
  __u16 addr;
  __u16 flags;
#define I2C_M_TEN		0x0010
#define I2C_M_RD		0x0001
#define I2C_M_NOSTART		0x4000
#define I2C_M_REV_DIR_ADDR	0x2000
#define I2C_M_IGNORE_NAK	0x1000
#define I2C_M_NO_RD_ACK		0x0800
#define I2C_M_RECV_LEN		0x0400
  __u16 len;
  __u8 * buf;
};  

struct i2c_msg i2c_buf[2];

int i2c_openadapter(uint8_t i2c_adapter_nr)
{
	int i2c_fd;
	char buf[MAX_BUFFER];
	snprintf(buf, sizeof(buf), "/dev/i2c-%d", i2c_adapter_nr);
	if ((i2c_fd = open(buf, O_RDWR)) < 1) {
		perror("Failed to open adapter");
		return -1;
	}
	return i2c_fd;
}

int i2c_smbus_access( 	int  		fd,
			byte_t  	read_write,
			byte_t  	command,
			int  	size,
			i2c_smbus_data_t*  	data) 
{
   i2c_smbus_ioctl_data_t  args; 
   args.read_write = read_write;
   args.command    = command;
   args.size       = size;
   args.data       = data;
   return ioctl(fd, I2C_SMBUS, &args);
 }

int i2c_smbus_read_byte ( int fd )  	
{
   i2c_smbus_data_t data;
   int rc;
   rc = i2c_smbus_access(fd, 1, 0, 1, &data);
   return rc>=0? 0x0FF & data.byte: -1;
 }

int i2c_smbus_write_byte ( int fd, byte_t value) 	
{
	return i2c_smbus_access(fd, I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

int i2c_setslave(int i2c_fd, uint8_t addr)
{
	if (ioctl(i2c_fd, I2C_SLAVE_FORCE, addr) < 0) {
		fprintf(stderr, "Failed to set slave address %d:", addr);
		perror("");
		return -1;
	}
	return 0;
}

int i2c_readbyte(int i2c_fd)
{
	int byte;
	if ((byte = i2c_smbus_read_byte(i2c_fd)) < 0) {
	// perror("Failed to read byte from I2C slave");
	return -1;
	}
	return byte;
}

int i2c_readbyte2(int i2c_fd, uint8_t *byte)
{
	if (read(i2c_fd,byte,1) == 1) {
		return 0;
		}
		return -1;
}

int i2c_readbytes(int i2c_fd, uint8_t *buf, uint8_t length)
{
	if (read(i2c_fd, buf, length) == length)
		return length;
	else
		return -1;
}

int beginTransmission(uint8_t address)
{
	int i2c_fd;
	uint8_t adapter = 3;
	i2c_fd = i2c_openadapter(adapter);
	i2c_setslave(i2c_fd, address);
	return i2c_fd;
}

/*
//unused
uint8_t read(uint8_t i2c_fd, uint8_t *address, uint8_t quantity)
{
//	i2c_setslave(i2c_fd, address);
	if (i2c_readbytes(i2c_fd, address, quantity) < 0)
		return 0;
	return 1;
}
*/
void i2c_write(int i2c_fd, uint8_t *data)
{
	if(write(i2c_fd,data,1) !=1)
	{
		printf("Failed to write\n");
	}
}

int i2c_add_to_buf(uint8_t addr, uint8_t rw, uint8_t *value, int length)
{
	if(i2c_buf_count < 2) {
		i2c_buf[i2c_buf_count].addr = addr;
		i2c_buf[i2c_buf_count].flags = rw ? I2C_M_RD : 0;
		i2c_buf[i2c_buf_count].len = length;
		i2c_buf[i2c_buf_count].buf = (unsigned char *)value;
		return ++i2c_buf_count;
	} else
	return -1;
}

int i2c_readwrite(int i2c_fd)
{
	struct i2c_rdwr_ioctl_data packets;
	packets.msgs = i2c_buf;
	packets.nmsgs = i2c_buf_count;
	if (ioctl(i2c_fd, I2C_RDWR, &packets) < 0) {
		perror("Unable to send data");
		i2c_buf_count = 0;
		return -1;
	}
	i2c_buf_count = 0;
	return 0;
}

int i2c_writebyte(int i2c_fd, uint8_t byte)
{
	if (i2c_smbus_write_byte(i2c_fd, byte ) < 0) {
		perror("Failed to write byte to I2C slave");
		return -1;	
	}
	return 0;
}

int i2c_smbus_write_i2c_block_data(int fd, byte_t command, byte_t length, const byte_t *values)
{
	i2c_smbus_data_t data;
	int i;

	if( length > I2C_SMBUS_I2C_BLOCK_MAX )
	{ length = I2C_SMBUS_I2C_BLOCK_MAX; }

	for(i=1; i<=length; i++)
	{ data.block[i] = values[i-1]; }
	data.block[0] = length;

	return i2c_smbus_access(fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

int i2c_writebytes(int i2c_fd, uint8_t *bytes, uint8_t length)
{
	if(i2c_smbus_write_i2c_block_data(i2c_fd, bytes[0], length-1, bytes+1)< 0) {
		perror("Failed to write bytes to I2C slave");
		return -1;
	}
	return 0;
}









TwoWire::TwoWire(void(*_beginCb)(void)) : rxBufferIndex(0), rxBufferLength(0), 
					  txAddress(0), txBufferLength(0), 
					  srvBufferIndex(0), srvBufferLength(0), 
					  onBeginCallback(_beginCb), 
					  adapter_nr(-1), i2c_fd(-1),
					  i2c_transfer(0)
{
	// Empty
}

/*
 * Not used, I think
 *
void TwoWire::begin(void)
{
	if (onBeginCallback)
		onBeginCallback();
	if ((adapter_nr = i2c_getadapter(I2C2)) < 0) {
		return;	
	}
	if ((i2c_fd = i2c_openadapter(adapter_nr)) < 0) {
		return;	
	}

}

void TwoWire::begin(uint8_t address)
{
	if (onBeginCallback)
		onBeginCallback();
}

void TwoWire::begin(int address)
{
	begin((uint8_t) address);
}
*/

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
	if (quantity > BUFFER_LENGTH)
		quantity = BUFFER_LENGTH;

	/* Set slave address via ioctl  */ 
	i2c_setslave(i2c_fd, address);

	if(i2c_transfer) {
	/* Need to perform a combined read/write operation
	 */
		i2c_transfer = 0;
		if (sendStop == false)
			return 0;
		i2c_add_to_buf(address, 1, rxBuffer, quantity);
		if (!i2c_readwrite(i2c_fd)) {
			rxBufferIndex = 0;
			rxBufferLength = quantity;
			i2c_transfer = 0;
			return quantity;
		} else
			return 0;
	}
	if (i2c_readbytes(i2c_fd, rxBuffer, quantity) < 0)
		return 0;
	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = quantity;

	return quantity;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
	if (i2c_fd < 0 || adapter_nr < 0)
		return;
	/* set slave address via ioctl in case we need to perform terminating 
	 * write operation
	 */
	i2c_setslave(i2c_fd, address);
	// save address of target and empty buffer
	txAddress = address;
	txBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
	beginTransmission((uint8_t) address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
	int err;
	if (sendStop == true) {	

		// transmit buffer (blocking)
		if (txBufferLength > 1)
			err = i2c_writebytes(i2c_fd, txBuffer, txBufferLength);
		else if (txBufferLength == 1)
			err = i2c_writebyte(i2c_fd, *txBuffer);
		else
		/* FIXME: A zero byte transmit is typically used to check for an
		 * ACK from the slave device. I'm not sure if this is the
		 * correct way to do this.
		 */ 
			err = i2c_readbyte(i2c_fd);
		// empty buffer
		txBufferLength = 0;
		if (err < 0)
			return 2;
		return 0;
	} else {
	/* sendStop = false
	 * pretend we have held the bus while
	 * actually waiting for the next operation
	 */
		i2c_add_to_buf(txAddress, 0, txBuffer, txBufferLength);		
		i2c_transfer = 1;
		return 0;
	}
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t data)
{
	if (txBufferLength >= BUFFER_LENGTH)
		return 0;
	txBuffer[txBufferLength++] = data;
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	for (size_t i = 0; i < quantity; ++i) {
		if (txBufferLength >= BUFFER_LENGTH)
			return i;
		txBuffer[txBufferLength++] = data[i];
	}
	return quantity;
}

int TwoWire::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void)
{
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex++];
	return -1;
}

int TwoWire::peek(void)
{
	if (rxBufferIndex < rxBufferLength)
		return rxBuffer[rxBufferIndex];
	return -1;
}

void TwoWire::flush(void)
{
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

void TwoWire::onReceive(void(*function)(int))
{
	onReceiveCallback = function;
}

void TwoWire::onRequest(void(*function)(void)) {
	onRequestCallback = function;
}

void TwoWire::onService(void)
{
}

#if WIRE_INTERFACES_COUNT > 0
static void Wire_Init(void)
{
}

TwoWire Wire = TwoWire(Wire_Init);

void WIRE_ISR_HANDLER(void) {
	Wire.onService();
}
#endif

#if WIRE_INTERFACES_COUNT > 1
static void Wire1_Init(void)
{
	if ((adapter_nr = i2c_getadapter(I2C1)) < 0) {
		return;	
	}
	if ((i2c_fd = i2c_openadapter) < 0) {
		return;	
	}
}

TwoWire Wire1 = TwoWire(Wire1_Init);

void WIRE1_ISR_HANDLER(void) {
	Wire1.onService();
}
#endif





