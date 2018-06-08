#ifndef DH_ATLAS_I2C_H__
#define DH_ATLAS_I2C_H__

#include "Arduino.h"
#include <i2c_t3.h>

//#define DEBUG 1

#define SINGLE_READING  "R"

class AtlasSensorI2C
{
public:

  AtlasSensorI2C(uint8_t addr){
		address = addr;
	}

  uint8_t last_status; // status of last I2C transmission

  void setAddress(uint8_t new_addr){
		address = new_addr;
	}

  uint8_t getAddress(void) { return address; }

  bool init(){
		return true;
	}

  void setWire(i2c_t3 *_wire){
		wire = _wire;
	}

  void sendCommand(char* command, char* output){

		wire->beginTransmission(address);
		wire->write(command);
		wire->endTransmission();
		uint8_t data[20];
		while (true){
			readBytes(data,20);
			uint8_t input = data[0];
			if (input == 2){

				output[0] = 0;
				return;

			}
			if (input == 1){
				break;
			}
			if (input == 255){
				triggerMeasurement();
			}
			// Assumed input = 254 .. wait
			delay(10);
		}
		for (uint8_t index=0; index<20 ; index++)
			output[index] = data[index+1];

	}

	void triggerMeasurement(){
		wire->beginTransmission(address);
		wire->write(SINGLE_READING);
		wire->endTransmission();
	}

	double readMeasurement(){
		uint8_t data[10];
		while (true){
      //        Serial.print("reading:");
		if (!readBytes(data,10)){
			return -9999;
		}
			uint8_t input = data[0];
      // Serial.print(input);
      // Serial.print(":");
      //   Serial.println((char*)data);
			if (input == 2 || input == 0){
				return -9999;
			}
			if (input == 1){
				break;
			}
			if (input == 255){
				triggerMeasurement();
			}
			// Assumed input = 254 .. wait
			delay(100);
		}
		char rawData[20] ;

		for (uint8_t index =0; index<10 ; index++)
		rawData[index] = data[index+1];
		return parseASCII(rawData);
	}


private:

	double parseASCII(char* data){
		return atof(data);
	}

	bool readBytes(uint8_t *bytes,uint8_t length){
		if (!wire->requestFrom(address, length)) return 0;
		wire->read(bytes,length);
    return 1;
	}

  uint8_t address;

	uint32_t delayTime =0;

  i2c_t3 *wire = &Wire;

};

#endif
