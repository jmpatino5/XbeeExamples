
#include "ArdXbeeRobots.h"

XbeeLocal Xbee=XbeeLocal(Serial1,115200);

void setup() 
{
	Serial.begin(115200);
	Serial.println("----Start Debug----");	
}

int len;

void loop() 
{
	len = Xbee.recData();
	if(len>0)
	{
		for(int i=0;i<len;i++)
		{
			Serial.print(Xbee.inputBuffer[i]);
		}
		Serial.println(" ");
		Serial.println("------------");
	}
}

