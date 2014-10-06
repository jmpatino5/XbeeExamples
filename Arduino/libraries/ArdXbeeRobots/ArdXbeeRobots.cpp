#include <ArdXbeeRobots.h>
#include <Arduino.h>

XbeeLocal::XbeeLocal(HardwareSerial serial_port,uint32_t speed):uart(serial_port)
{
	uart = serial_port;
	uart.begin(speed);
	uart.setTimeout(10);
	attachedDevices=0;
	for(int i=0;i<8;i++)
	{
		OuputAdress[i]=0;
	}
}
void XbeeLocal::setAdress(int destAdress[8])
{
	for(int i=0;i<8;i++)
	{
		OuputAdress[i]=destAdress[i];
	}
}

bool XbeeLocal::sendPack(uint8_t len)
{
	while(uart.available()>0)
	{
		uart.read();
	}

	uint8_t i;
	uint8_t Length;
	uint8_t adressSum;
	uint8_t datoSum;
	uint8_t cheksum;
	char tempBuffer[10];
	char FT=0x8B; /// Constant value to string to compare in the recive confirmation must be a constant string

	Length=14+len;

	uart.write(0x7E);			//Inicio

	uart.write((byte)0x00);	//Length LSB
	uart.write(Length);		//Length MSB

	uart.write(0x10);			//ZigBee Transmit Request API frame type
	uart.write(0x01);			//Frame ID

	adressSum=0;

	for(i=0;i<8;i++)			//Direccion de destino
	{
		uart.write(OuputAdress[i]);
		adressSum=adressSum+int(OuputAdress[i]);
	}

	uart.write(0xFF);			//16-bit Destination Address
	uart.write(0xFE);			//16-bit Destination Address

	uart.write((byte)0x00);			//Broadcast radius
	uart.write((byte)0x00);			//Options

	datoSum=0;
	for(i=0;i<(Length-14);i++)	//Envio de datos
	{
		uart.write(outBuffer[i]);
		datoSum=datoSum+int(outBuffer[i]);
	}

	cheksum=datoSum+adressSum;
	cheksum=0xFF-char(cheksum+17+255+254);
	uart.write(cheksum);		//Checksum

	uint8_t timeout_counter=0;
	while(!uart.available())
	{
		if(timeout_counter==20)
		{
			
			return false;
		}
		timeout_counter++;
		delay(5);
	}

	if(uart.available()>0)
	{
		if(uart.find("~"))
		{
			uart.readBytes(tempBuffer,10);
			if(tempBuffer[2]==FT)
			{
				while(uart.available()>0)
				{
					uart.read();
				}
				
				return true;
			}
		}
	}
	else
	{
		while(uart.available()>0)
		{
			uart.read();
		}

	 	return false;
	}
}

int XbeeLocal::recData()
{
	char tempBuffer[128];
	char FT=0x90;

	if(uart.available()>0)
	{
		if(uart.find("~"))
		{
			uart.readBytes(tempBuffer,3);
			int len=(tempBuffer[1]-12);
			if(tempBuffer[2]==FT)
			{
				uart.readBytes(tempBuffer,len+12);
				for(int i=0;i<8;i++)
				{
					inputAdress[i]=tempBuffer[i];
				}
				for(int i=0;i<len;i++)
				{
					inputBuffer[i]=tempBuffer[i+11];
				}
				uart.flush();
				return len;
			}
			else
			{
				uart.flush();				
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
}

bool XbeeLocal::sendString(char data[])
{
	while(uart.available()>0)
	{
		uart.read();
	}

	int temSum=0;

	uart.write(0x7E);
	uart.write((byte)0x00);
	uart.write(strlen(data)+14);
	uart.write(0x10);
	uart.write(0x01);

	for(int i=0;i<8;i++)
	{
		temSum+=OuputAdress[i];
		uart.write(OuputAdress[i]);
	}

	uart.write(0xFF);
	uart.write(0xFE);
	uart.write((byte)0x00);
	uart.write((byte)0x00);

	for(int i=0;i<strlen(data);i++)
	{
		temSum+=data[i];
		uart.write(data[i]);

	}

	temSum+=17+255+254;
	int Chk=0xFF-char(temSum);
	uart.write(Chk);

	char tempBuffer[10];
	char FT=0x8B;

	uint8_t timeout_counter=0;
	while(!uart.available())
	{
		if(timeout_counter==20)
		{
			
			return false;
		}
		timeout_counter++;
		delay(5);
	}

	if(uart.available()>0)
	{
		if(uart.find("~"))
		{
			uart.readBytes(tempBuffer,10);
			if(tempBuffer[2]==FT && tempBuffer[7]==0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	else
	{
		while(uart.available()>0)
		{
			uart.read();
		}
		return false;
	}
}

int* XbeeLocal::nodeDiscover()
{
	char tempBuffer[35];
	int timeout=0;
	char FT=0x88;
	int devices=0;
	int* out=(int*)malloc(8+(devices*8));
	int Command[8]={0x7E,0x00,0x04,0x08,0x01,0x4E,0x44,0x64};
	for(int i=0;i<8;i++)
	{
		uart.write(Command[i]);
	}
	while(1)
	{
		if(uart.available()>0)
		{
			if(uart.find("~"))
			{
				uart.readBytes(tempBuffer,3);
				int len=tempBuffer[1];
				if(tempBuffer[2]==FT)
				{
					uart.readBytes(tempBuffer,len);
					for(int i=0;i<8;i++)
					{
						out[i+(devices*8)]=tempBuffer[i+6];
					}
					timeout=0;
					devices++;
				}
			}
		}
		else if(timeout==300)
		{
			if(devices>0)
			{
				attachedDevices=devices;
				
				return out;
			}
			else
			{
				
				return 0;
			}
		}
		else
		{
			timeout+=1;
			delay(10);
		}
	}
}

int XbeeLocal::getDevices()
{
	return attachedDevices;
}

bool XbeeLocal::setPan_ID(uint8_t pan_id)
{
	char FT=0x88;
	char tempBuffer[35];
	int Command[8]={0x7E,0x00,0x05,0x08,0x01,0x49,0x44,pan_id};
	int chksum = 0;
	for(int i=3;i<8;i++)
	{
		chksum+=Command[i];
	}
	chksum = 0xFF -char(chksum);

	for(int i=0;i<8;i++)
	{
		uart.write(Command[i]);
	}
	uart.write(chksum);

	uint8_t timeout_counter=0;
	
	while(!uart.available())
	{
		if(timeout_counter==20)
		{
			return false;
		}
		timeout_counter++;
		delay(5);
	}
	if(uart.available()>0)
	{
		if(uart.find("~"))
		{
			uart.readBytes(tempBuffer,10);
			if(tempBuffer[2]==FT)
			{
				while(uart.available()>0)
				{
					uart.read();
				}
				
				return true;
			}
		}
	}
	else
	{
		while(uart.available()>0)
		{
			uart.read();
		}
	 	return false;
	}
}