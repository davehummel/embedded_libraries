#ifndef DH_ATLAS_PH_I2C_H__
#define DH_ATLAS_PH_I2C_H__

#include "Arduino.h"
#include <i2c_t3.h>

#define DEF_ADDR 99               //default I2C ID number for EZO pH Circuit.
#define COMMAND_DELAY 300
//#define DEBUG 1

#define SINGLE_READING  "R"
#define CALIBRATE  "CAL"
#define TEMPERTATURE_COMP  "T"



class PHSensorI2C
{
public:

  PHSensorI2C(){
		address = DEF_ADDR;
	}

  uint8_t last_status; // status of last I2C transmission

  void setAddress(uint8_t new_addr){
		address = new_addr;
	}

  inline uint8_t getAddress(void) { return address; }

  bool init(){
		return true;
	}

  void setWire(i2c_t3 *_wire){
		wire = _wire;
	}

	void triggerMeasurement(){
		wire->beginTransmission(address);
		wire->write(SINGLE_READING);
		wire->endTransmission();
	}

	double readMeasurement(){
		uint8_t data[10];
		while (true){
			readBytes(data,10);
			uint8_t input = data[0];
			if (input == 2){

				return -1;
			}
			if (input == 1){
				break;
			}
			if (input == 255){
				triggerMeasurement();
			}
			// Assumed input = 254 .. wait
			delay(1);
		}
		char rawData[20] ;
		uint8_t index = 0;

		for (uint8_t index =0; index<10 ; index++)
		rawData[index] = data[index+1];
		return parseASCII(rawData);
	}


private:

	double parseASCII(char* data){
		return atof(data);
	}

	void readBytes(uint8_t *bytes,uint8_t length){
		wire->requestFrom(address, length);
		wire->read(bytes,length);
	}

  uint8_t address;

	uint32_t delayTime =0;

  i2c_t3 *wire = &Wire;

};

#endif
