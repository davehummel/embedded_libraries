#ifndef DH_CONTROLLEDI2CXL_H__
#define DH_CONTROLLEDI2CXL_H__
#include "dh_controller.h"
#include <i2c_t3.h>

//The Arduino Wire library uses the 7-bit version of the address, so the code example uses 0x70 instead of the 8‑bit 0xE0
#define SensorAddress1 byte(0x70)
#define SensorAddress2 byte(0x71)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)

#define SCAN_ID 30
#define SCAN_DELAY 50

class ControlledI2CXL: public Controller::Controlled{
public:

	void begin(void){
		scanning = false;
	}

	void setWire(i2c_t3* _wire){
		wire = _wire;
	}

	void write(ADDR1 addr,uint8_t val){
					if (scanning == (val>0))
						return;


					if ((val>0) == false){
						stopScan();
					} else {
						startScan();
					}

	

	}


	uint8_t readB(ADDR1 addr,uint8_t addr2){
		return scanning;
	}

	uint16_t readU(ADDR1 addr,uint8_t addr2){
		if (!scanning)
			return 0;

		switch(addr.addr%26+'A'){
				case 'A': return readingA;
				case 'B': return readingB;
				default: return -1;
		}
	}

	void startScan(){

		if (scanning){
			stopScan();
		}
		onA = true;
		wire->beginTransmission(SensorAddress1);             //Start addressing 
		wire->write(RangeCommand);                             //send range command 
		wire->endTransmission(I2C_STOP);  

		controller->schedule(SCAN_ID,SCAN_DELAY,SCAN_DELAY,false,0,Controller::newString("SA"),id,false);
		
		scanning = true;
		onA = true;
	}

	void stopScan(){
		if (scanning){
			controller->kill(SCAN_ID);
			scanning = false;
		}
	}

	void execute(uint32_t time,uint32_t id,char* command){
		if (!scanning)
			return;
		if(onA){
			  wire->requestFrom(SensorAddress1, byte(2));
			  if(wire->available() >= 2){                            //Sensor responded with the two bytes 
			      byte HighByte = wire->read();                        //Read the high byte back 
			      byte LowByte = wire->read();                        //Read the low byte back 
			      readingA  = word(HighByte, LowByte);         //Make a 16-bit word out of the two bytes for the range  
			   }else{
			      readingA = 0; 
			   }
				wire->beginTransmission(SensorAddress2);             //Start addressing 
				wire->write(RangeCommand);                             //send range command 
				wire->endTransmission(I2C_STOP,100); 
				onA = false;
		}else{
			wire->requestFrom(SensorAddress2, byte(2));
			if(wire->available() >= 2){                            //Sensor responded with the two bytes 
			    byte HighByte = wire->read();                        //Read the high byte back 
			    byte LowByte = wire->read();                        //Read the low byte back 
			    readingB  = word(HighByte, LowByte);         //Make a 16-bit word out of the two bytes for the range  
			}else{
			    readingB = 0; 
			}
			wire->beginTransmission(SensorAddress1);             //Start addressing 
			wire->write(RangeCommand);                             //send range command 
			wire->endTransmission(I2C_STOP,100); 
			onA = true;
		}
	}


	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
	
	}

	uint16_t readingA;
	uint16_t readingB;
	bool scanning = false;
	bool onA = false;


private:
	i2c_t3* wire;


};

	

#endif