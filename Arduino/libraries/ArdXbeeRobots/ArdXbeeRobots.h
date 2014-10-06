#ifndef ArdXbeeRobots_h
#define ArdXbeeRobots_h

#include <Arduino.h>

class XbeeLocal
{
public:
	XbeeLocal(HardwareSerial,uint32_t);
	void setAdress(int[8]);
	bool sendPack(uint8_t);
	int recData();
	bool sendString(char[]);
	int* nodeDiscover();
	int getDevices();
	bool setPan_ID(uint8_t);
	char inputBuffer[128];
	int inputAdress[8];
	char outBuffer[128];
private:
	HardwareSerial uart;
	int OuputAdress[8];
	int attachedDevices;
};

#endif
//End of the headerfile
