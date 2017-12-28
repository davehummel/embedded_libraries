#ifndef DH_RF24CLIENT_H__
#define DH_RF24CLIENT_H__
#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include "RF24.h"

#define DEFCLIENTNAME "DFLT"

class RF24Client
{
  public:
	RF24Client(uint8_t controlPin, uint8_t selectPin) : radio(controlPin, selectPin)
	{
	}

	void setClientPrefix(byte *clientPrefixIn)
	{
		memcpy(clientPrefix, clientPrefixIn, 4);
	}

	bool loadClientPrefixFromEEPROM(uint16_t addr = 0)
	{
		uint8_t readByte = 0;
		readByte = EEPROM.read(0);
		if (readByte != 0)
		{
			clientPrefix[0] = readByte;
			clientPrefix[1] = EEPROM.read(1);
			clientPrefix[2] = EEPROM.read(2);
			clientPrefix[3] = EEPROM.read(3);
			return true;
		}
		else
		{
			return false;
		}
	}

	void getClientPrefix(char *output)
	{
		memcpy(output, clientPrefix, 4);
	}

	void saveClientPrefixToEEPROM(byte *clientPrefixIn, uint16_t addr = 0)
	{
		EEPROM.write(addr, clientPrefixIn[0]);
		EEPROM.write(addr + 1, clientPrefixIn[1]);
		EEPROM.write(addr + 2, clientPrefixIn[2]);
		EEPROM.write(addr + 3, clientPrefixIn[3]);
	}

	void begin()
	{
		radio.begin();
		uint8_t pipeName[5];
		memcpy(pipeName, clientPrefix, 4);

		pipeName[4] = 'R';
		radio.openWritingPipe(pipeName);

		pipeName[4] = 'W';
		radio.openReadingPipe(1, pipeName);

		radio.startListening();
	}

	void sendDouble(uint8_t letter, double value)
	{
		radio.stopListening();
		uint8_t buffer[9];

		buffer[0] = letter;
		memcpy(&buffer[1], &value, sizeof(value));

		radio.write(buffer, 9);

		radio.startListening();
	}

	bool listen()
	{
		bool didSomething = false;
		while (radio.available())
		{
			Serial.println("Data in ...");
			didSomething = true;
			uint8_t data[9];
			radio.read(data, 9);
			uint8_t letter = data[0];
			double value;
			memcpy(&value, &data[1], sizeof(value));
			if (letter == 255)
			{ // PING
				Serial.println("Ping IN !!!!!!");
				timeSinceLastPing = 0;
				remoteTimeInMS = (long)value;
			}
			else if (letter < 26)
			{
				readCache[letter] = value;
				Serial.print("Wrote value ");
				Serial.print(value);
				Serial.print(" to cache at ");
				Serial.println(letter);

				if (func[letter] != 0)
				{
					Serial.println("And invoked a function!");
					func[letter](value);
				}
			}
		}
		return didSomething;
	}

	void setListener(uint8_t letter, void (*newFunc)(double))
	{
		if (letter >= 26)
			return;
		func[letter] = newFunc;
	}

	double getValue(uint8_t letter)
	{
		if (letter >= 26)
			return 0;
		return readCache[letter];
	}

	void setValue(uint8_t letter, double value)
	{
		if (letter >= 26)
			return;
		readCache[letter] = value;
	}

	long getTimeSinceLastPing()
	{
		return (long)timeSinceLastPing;
	}

	long getRemoteTime()
	{
		return remoteTimeInMS;
	}

  private:
	RF24 radio;
	elapsedMillis timeSinceLastPing = 0;
	long remoteTimeInMS = 0;
	double readCache[26] = {0};
	void (*func[26])(double) = {0};
	byte clientPrefix[4] = {0};
};

#endif
