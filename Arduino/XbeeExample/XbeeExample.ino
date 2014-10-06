#include "ArdXbeeRobots.h"

XbeeLocal Xbee=XbeeLocal(Serial1,9600);

void setup(){
	Serial.begin(115200);
	Serial.println("--- Debug ---");
}

void loop(){
	checkForXbee();
	if(Serial.available()){
		String msg = readString();
		msg.toCharArray(Xbee.outBuffer, msg.length());
		if(Xbee.sendPack(msg.length())){
			Serial.println("Message Send!");
		}
		else{
			Serial.println("Fail!");
		}
	}
}

String readString(){
	String output="";
	while(Serial.available())
	{
	  	output.concat(char(Serial.read()));
	  	delay(10);
	}
	return output;
}


void checkForXbee(){
	int len = Xbee.recData();
	if(len){
		Serial.println("--- New Message! ---");
		Serial.print("Send from: ");
		for(int i = 0; i < 8 ; i++){
			Serial.print(uint8_t(Xbee.inputAdress[i]),HEX);
			Serial.print(" ");
		}
		Serial.println("");

		Serial.print("Lenght: ");
		Serial.println(len);

		Serial.print("Payload: ");	

		String output = "";
		for(int i = 0; i < len ; i++){
			output.concat(char(Xbee.inputBuffer[i]));
		}
		Serial.println(output);
	}
}