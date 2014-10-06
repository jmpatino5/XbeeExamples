char frame[64];
bool state = true;

void setup(){
	Serial.begin(115200);
	Serial1.begin(9600);
	delay(1000);
	Serial.println("--- Send Frame ---");
}

uint8_t adress[8] = {0x00,0x13,0xA2,0x00,0x40,0x61,0x4B,0x06};

// --- AT Remote Frame ---
/* Frame = 	[header]
			[MSB]
			[LSB] 
			[Frame Type] 
			[ID] 
			[64 Bit Adress] 
			[16 Bit Adress] 
			[Remote Command Option]
			[AT Command]
			[AT Parameter]
			[Check Sum]
*/
void loop(){
	if(Serial.available()){

		state = !state;

		frame[0] = 0x7E; 				// Header

		frame[1] = (byte)0x00;			// MSB
		frame[2] = 0x10;				// LSB

		frame[3] = 0x17; 				// Frame Type

		frame[4] = 0x01;				// ID

		for(int i=0;i < 8;i++){			// 64 Bit Adress
			frame[5+i] = adress[i];
		}

		// 16 bits Adress

		frame[13] = 0xFF;	
		frame[14] = 0xFE;

		frame[15] = 0x02; 				// Remote Command Option

		frame[16] = 'D';
		frame[17] = '2';

		if(state){						// AT Parameter
			frame[18] = 0x05;	
		}
		else{
			frame[18] = 0x04;
		}

		frame[19] = checkSum(18);		// CheckSum

		for(int i=0; i<20; i++){		// 
			Serial1.write(frame[i]);
		}

		delay(100);

		while(Serial1.available()){
			Serial.print(Serial1.read(),HEX);
			Serial.print('\t');
		}
		Serial.println("");
		Serial.println("Finish!");

		while(Serial.available()){
		    Serial.read();
		}
	}
}

uint8_t checkSum(uint8_t len){
	int sum = 0;
	for(int i = 3; i < len+1; i++){
		sum += uint8_t(frame[i]);
	}
	return (uint8_t)(0xFF - sum);
}