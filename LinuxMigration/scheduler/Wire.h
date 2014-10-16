#ifndef TwoWire_h
#define TwoWire_h

#define BUFFER_LENGTH 32
#define I2C2 0x4819c000
#define I2C1 0x00000000
#define WIRE_INTERFACES_COUNT 1

#include <stdlib.h>

class TwoWire {
public:
	TwoWire(void(*begin_cb)(void));
//	void begin();
//	void begin(unsigned char);
//	void begin(int);
	void beginTransmission(unsigned char);
	void beginTransmission(int);
	unsigned char endTransmission(void);
	unsigned char endTransmission(unsigned char);
	unsigned char requestFrom(unsigned char, unsigned char);
	unsigned char requestFrom(unsigned char, unsigned char, unsigned char);
	unsigned char requestFrom(int, int);
	unsigned char requestFrom(int, int, int);
	virtual size_t write(unsigned char);
	virtual size_t write(const unsigned char *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void(*)(int));
	void onRequest(void(*)(void));

    inline size_t write(unsigned long n) { return write((unsigned char)n); }
    inline size_t write(long n) { return write((unsigned char)n); }
    inline size_t write(unsigned int n) { return write((unsigned char)n); }
    inline size_t write(int n) { return write((unsigned char)n); }

	void onService(void);

private:
	// RX Buffer
	unsigned char rxBuffer[BUFFER_LENGTH];
	unsigned char rxBufferIndex;
	unsigned char rxBufferLength;

	// TX Buffer
	unsigned char txAddress;
	unsigned char txBuffer[BUFFER_LENGTH];
	unsigned char txBufferLength;

	// Service buffer
	unsigned char srvBuffer[BUFFER_LENGTH];
	unsigned char srvBufferIndex;
	unsigned char srvBufferLength;

	// Callback user functions
	void (*onRequestCallback)(void);
	void (*onReceiveCallback)(int);

	// Called before initialization
	void (*onBeginCallback)(void);

	// Timeouts (
	static const unsigned int RECV_TIMEOUT = 100000;
	static const unsigned int XMIT_TIMEOUT = 100000;

	unsigned char adapter_nr;
	int i2c_fd;
	int i2c_transfer;
};

#if WIRE_INTERFACES_COUNT > 0
extern TwoWire Wire;
#endif
#if WIRE_INTERFACES_COUNT > 1
extern TwoWire Wire1;
#endif

#endif
