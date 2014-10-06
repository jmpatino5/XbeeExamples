
#include "ArdXbeeRobots.h"

XbeeLocal Xbee=XbeeLocal(Serial1,9600);

void setup()
{
  Serial.begin(115200);
  //Serial.println("Debug!");
  int adress[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  Xbee.setAdress(adress);
}

void loop() 
{
	Xbee.outBuffer[0] = 'H';
	Xbee.outBuffer[1] = 'E';
	Xbee.outBuffer[2] = 'L';
	Xbee.outBuffer[3] = 'L';
	Xbee.outBuffer[4] = 'O';
	Xbee.outBuffer[5] = ' ';
	Xbee.outBuffer[6] = 'W';
	Xbee.outBuffer[7] = 'O';
	Xbee.outBuffer[8] = 'R';
	Xbee.outBuffer[9] = 'L';
	Xbee.outBuffer[10] = 'D';
	Xbee.outBuffer[11] = '!';

	while(1)
	{
		Xbee.sendPack(12);
		delay(1000);
	}
}
