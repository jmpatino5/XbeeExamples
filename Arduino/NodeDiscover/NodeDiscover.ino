#include "ArdXbeeRobots.h"

XbeeLocal Xbee=XbeeLocal(Serial1,9600);

uint8_t devices;
char* tempBuffer;
int modulos[3][8];

String msg = "BroadCast!";

void setup(){
	Serial.begin(115200);
	Serial.println("Loading...");
	delay(5000);
}

void loop(){
	int* dirs=Xbee.nodeDiscover();
	devices = Xbee.getDevices();
	for (int i=0; i<devices ; i++){
		for(int j=0;j<8;j++){
			modulos[i][j]=dirs[j+(8*i)];
		}
	}
	Serial.print("Se detectaron ");
	Serial.print(devices);
	Serial.println(" Modulos");
	for (int i=0;i<devices;i++)
	{
		Serial.print("El modulo ");
		Serial.print(i);
		Serial.print(" Termina en: ");
		Serial.println(modulos[i][7]);
	}

	if(devices){
		for(int i = 0; i < devices ; i++){
			Xbee.setAdress(modulos[i]);
			msg.toCharArray(Xbee.outBuffer, msg.length());
			Xbee.sendPack(msg.length());
			delay(5000); 
		}
	}

	while(1){
	    // statement
	}
}