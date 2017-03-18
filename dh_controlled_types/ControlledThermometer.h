#ifndef DH_CONTROLLEDTHERMO_H__
#define DH_CONTROLLEDTHERMO_H__
#include "dh_controller.h"
#include <OneWire.h>

#define SCAN_ID 70
#define SCAN_DELAY 1000

class ControlledThermometer: public Controller::Controlled{

public:
	ControlledThermometer(uint8_t oneWirePin,uint8_t* devAddr,uint8_t numDev){
		if (numDev>10){
			numDev = 10;
		}
		 ds = new OneWire(oneWirePin);
		 deviceCount = numDev;
		 values = new float[numDev];
		 for (uint8_t i = 0; i<numDev; i++){
			 values[i] = 0;
			 deviceAddr[i][1] = devAddr[i];
		 }
	}

	~ControlledThermometer(){
		 delete ds;
		 delete values;
	}

	void begin(void){
		  byte addr[8];
			while(ds->search(addr)){
				for (uint8_t i = 0; i < deviceCount ; i++){
					if (addr[1] == deviceAddr[i][1])
					  memcpy(deviceAddr[i],addr, sizeof(byte)*8);
				}
			}
			ds->reset_search();
	}

	void startScan(){
		if (deviceCount == 0)
			return;
		devScan = 0;
		if (scanning){
			stopScan();
		}
    startConversion(devScan);
		controller->schedule(SCAN_ID,0,SCAN_DELAY,false,0,Controller::newString("S"),id,false);
    scanning = true;
	}

	void stopScan(){
		if (scanning){
			controller->kill(SCAN_ID);
			scanning = false;
		}
	}



	void execute(uint32_t time,uint32_t id,char command[]){
		if (id == SCAN_ID && command[0] == 'S'){
			values[devScan]= readTemp(devScan);
			    //  Serial.print("Thermo:");
					//  Serial.print(deviceAddr[devScan][1],HEX);
					//  Serial.print("=");
					// 	Serial.println(	values[devScan]);
			devScan++;
			if (devScan>=deviceCount)
				devScan = 0;
				startConversion(devScan);
				return;
		}else{
			startScan();
		};

	}
	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}

	float readF(ADDR1 addr,uint8_t addr2){
			uint8_t num = addr.addr%26;
			if (num>=deviceCount){
				controller->getErrorLogger()->print("Thermo ");
				controller->getErrorLogger()->print((char)(num+'A'));
				controller->getErrorLogger()->print("is not supported. ");
				return 0 ;
			}

			return values[num];
	}


private:

	void startConversion(uint8_t dev){
		ds->reset();
		ds->select(deviceAddr[dev]);
		ds->write(0x44, 0);
	}

	float readTemp(uint8_t dev){
	  ds->reset();

			ds->select(deviceAddr[dev]);
			ds->write(0xBE);         // Read Scratchpad

	byte data[9];
	for (uint8_t  i = 0; i < 9; i++) {           // we need 9 bytes
		data[i] = ds->read();
	}

	// Convert the data to actual temperature
	// because the result is a 16 bit signed integer, it should
	// be stored to an "int16_t" type, which is always 16 bits
	// even when compiled on a 32 bit processor.
	int16_t raw = (data[1] << 8) | data[0];

	byte cfg = (data[4] & 0x60);
	// at lower res, the low bits are undefined, so let's zero them
			if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
			else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
			else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
			//// default is 12 bit resolution, 750 ms conversion time
		float temp = (float)raw / 16.0;
		temp = temp * 1.8 + 32.0;
		return temp;
	}

	OneWire* ds;
	bool scanning = false;
	byte deviceAddr[10][8];
	uint8_t deviceCount;
	uint8_t devScan;
  float* values;
};



#endif
