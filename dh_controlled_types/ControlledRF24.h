#ifndef DH_CONTROLLEDRF24_H__
#define DH_CONTROLLEDRF24_H__
#include "dh_controller.h"
#include <SPI.h>
#include "RF24.h"

//#define DEBUG 1

#define RF_SCAN_ID 692
#define RF_SCAN_DELAY 100

#define RF_PING_ID 693
#define RF_PING_DELAY 1000

class ControlledRF24 : public Controller::Controlled
{

  public:
	ControlledRF24(uint8_t controlPin, uint8_t selectPin) : radio(controlPin, selectPin)
	{
	}

	~ControlledRF24()
	{
	}

	void begin(void)
	{
		if (!radio.begin())
		{

			controller->getErrorLogger()->println("nRF24 radio failed to start.");
			controller->getErrorLogger()->finished(millis(), ErrorLogger::MOD_PARSER);

			return;
		}

		radio.startListening();
#ifdef DEBUG
		Serial1.println("Started listneing");
#endif
		started = true;
		controller->schedule(RF_SCAN_ID, 0, RF_SCAN_DELAY, false, 0, Controller::newString("S"), id, false);
		controller->schedule(RF_PING_ID, 0, RF_PING_DELAY, false, 0, Controller::newString("P"), id, false);
	}

	void execute(uint32_t time, uint32_t id, char command[])
	{
		if (id == RF_SCAN_ID && command[0] == 'S')
		{
			if (!started)
			{

				controller->getErrorLogger()->println("nRF24 NOT ENABLED.");
				controller->getErrorLogger()->finished(millis(), ErrorLogger::MOD_PARSER);
				return;
			}

			uint8_t pipeNum;
			while (radio.available(&pipeNum))
			{
				uint8_t buffer[9];

				radio.read(buffer, 9);

				uint8_t letter = buffer[0];

				if (letter < 26)
				{
#ifdef DEBUG
					Serial1.print("Got message on pipe");
					Serial1.print(pipeNum - 1);
					Serial1.print(" : ");
					Serial1.print(letter);
					Serial1.print(" [ ");
					for (uint8_t a = 0; a < 9; a++)
					{
						Serial1.print(buffer[a]);
						Serial1.print(" ");
					}
					Serial1.print("] = ");
#endif
					memcpy(&readCache[pipeNum - 1][letter], &buffer[1], 8);
#ifdef DEBUG
					Serial1.println(readCache[pipeNum - 1][letter]);
#endif
				}
				else
				{
					controller->getErrorLogger()->print("Master radio recieved a letter greater than Z");
					controller->getErrorLogger()->finished(controller->lastProcessedMSTime, ErrorLogger::MOD_PARSER);
				}
			}
		}
		else if (id == RF_PING_ID && command[0] == 'P')
		{
			if (!started)
			{

				controller->getErrorLogger()->println("nRF24 NOT ENABLED.");
				controller->getErrorLogger()->finished(millis(), ErrorLogger::MOD_PARSER);
				return;
			}
			//Ping command
			if (pipeOpen[pingPipeIterator])
			{

#ifdef DEBUG
				Serial1.print("Pinging ");
				Serial1.print(pingPipeIterator);
#endif
				bool success = rawWrite(pingPipeIterator, 255, (double)millis());
				if (success && pipePingFailed[pingPipeIterator] > 0)
				{
					pipePingFailed[pingPipeIterator]--;
				}
				else if (!success && pipePingFailed[pingPipeIterator] < 255)
				{
					pipePingFailed[pingPipeIterator]++;
				}

				if (!success)
				{
#ifdef DEBUG
					Serial1.print("!!!!!++");
#endif
					controller->getErrorLogger()->print("nRF24 ping failed on:");
					controller->getErrorLogger()->print(pingPipeIterator);
					controller->getErrorLogger()->print(" and count is ");
					controller->getErrorLogger()->print(pipePingFailed[pingPipeIterator]++);
					controller->getErrorLogger()->finished(controller->lastProcessedMSTime, ErrorLogger::MOD_PARSER);
				}
				else
				{
#ifdef DEBUG
					Serial1.print("     --");
					Serial1.println(pipePingFailed[pingPipeIterator]);
#endif
				}
			}
			pingPipeIterator++;
			if (pingPipeIterator > 4)
				pingPipeIterator = 0;
		}
		else if (command[0] == 'C')
		{ //CONNECT A DPT1
			if (!started)
			{

				controller->getErrorLogger()->println("nRF24 NOT ENABLED.");
				controller->getErrorLogger()->finished(millis(), ErrorLogger::MOD_PARSER);
				return;
			}
			uint8_t pipeNum = toupper(command[8]) - 'A';
			if (pipeNum > 4)
			{
				controller->getErrorLogger()->print("Master radio recieved connection request to a channel greater than E");
				controller->getErrorLogger()->finished(controller->lastProcessedMSTime, ErrorLogger::MOD_PARSER);
				return;
			}

			memcpy(&pipeReadName[pipeNum][0], &command[10], 4);
			memcpy(&pipeWriteName[pipeNum][0], &command[10], 4);

			pipeWriteName[pipeNum][4] = 'W';
			pipeWriteName[pipeNum][5] = '\0';

			pipeReadName[pipeNum][4] = 'R';
			pipeReadName[pipeNum][5] = '\0';

			if (pipeOpen[pipeNum])
			{
				radio.closeReadingPipe(pipeNum + 1);
			}

			radio.openReadingPipe(pipeNum + 1, pipeReadName[pipeNum]);
			pipeOpen[pipeNum] = true;
		}
	}

	void startSchedule(char command[], uint32_t id)
	{
	}
	void endSchedule(char command[], uint32_t id)
	{
	}

	void writeCon(ADDR1 addr, double val)
	{

		if (!started)
		{
			controller->getErrorLogger()->println("nRF24 NOT ENABLED.");
			controller->getErrorLogger()->finished(millis(), ErrorLogger::MOD_PARSER);
			return;
		}

		uint8_t pipeNum = addr.addr % 26;

		if (pipeNum >= 5)
		{
			return;
		}

		if (pipeOpen[pipeNum] == false)
		{
			return;
		}

		uint8_t letter = (addr.addr / 26) % 26;
#ifdef DEBUG
		Serial1.print("Sending to pipe ");
		Serial1.print(pipeNum);
		Serial1.print(" to letter ");
		Serial1.print(letter);
		Serial1.print(" value ");
		Serial1.println(val);
#endif

		rawWrite(pipeNum, letter, val);
	}

	bool rawWrite(uint8_t pipeNum, uint8_t letter, double val)
	{
		radio.stopListening();

		radio.openWritingPipe(pipeWriteName[pipeNum]);

		uint8_t buffer[9];

		buffer[0] = letter;
		memcpy(&buffer[1], &val, sizeof(val));

		bool good = radio.write(buffer, 9);

		radio.startListening();

		return good;
	}

	double readConD(ADDR1 addr, uint8_t addr2)
	{
		uint8_t pipeNum = addr.addr % 26;

		if (pipeNum >= 5)
		{
			return 0;
		}

		if (pipeOpen[pipeNum] == false)
		{
			return 0;
		}

		uint8_t letter = (addr.addr / 26) % 26;

		return readCache[pipeNum][letter];
	}

	float readConF(ADDR1 addr, uint8_t addr2)
	{
		uint8_t pipeNum = addr.addr % 26;

		if (pipeNum >= 5)
		{
			return 0;
		}

		if (pipeOpen[pipeNum] == false)
		{
			return 0;
		}

		uint8_t letter = (addr.addr / 26) % 26;

		return (float)readCache[pipeNum][letter];
	}

	uint8_t readConB(ADDR1 addr, uint8_t addr2)
	{
		uint8_t pipeNum = addr.addr % 26;

		if (pipeNum >= 5)
		{
			return 0;
		}

		if (pipeOpen[pipeNum] == false)
		{
			return 0;
		}

		return pipePingFailed[pipeNum];
	}

  private:
	RF24 radio;

	bool started = false;

	uint8_t pipePingFailed[5] = {0};

	bool pipeOpen[5] = {0};

	double readCache[5][26] = {{0}};

	uint8_t pipeReadName[5][6] = {{0}};

	uint8_t pipeWriteName[5][6] = {{0}};

	uint8_t pingPipeIterator;
};

#endif
