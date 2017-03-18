#ifndef DH_CONTROLLEDVMETER_H__
#define DH_CONTROLLEDVMETER_H__
#include "dh_controller.h"

class ControlledVMeter: public Controller::Controlled{

public:
	ControlledVMeter(uint8_t _vPin, uint8_t _cPin){
		vPin=_vPin;
		cPin=_cPin;
	}

	void begin(void){
		analogReadAveraging(32);
		analogReadResolution(16);
		pinMode(vPin, INPUT);
		pinMode(cPin, INPUT);

	}
	void execute(uint32_t time,uint32_t id,char command[]){

	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
	
	}


	uint16_t readU(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'C': return analogRead(cPin);
				case 'V': return analogRead(vPin);
				default: return -1;
			}
	}

	float readF(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'C': return ((float)readU(addr,addr2))*0.4121-4159.24;
				case 'V': return ((float)readU(addr,addr2))*0.000137 + 0.2056; 
				default: return -1;
			}
	}

	double readD(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'C': return ((double)readU(addr,addr2))*0.413l-4162l;
				case 'V': return ((double)readU(addr,addr2))*0.0001370023l + 0.205587l;
				default: return -1;
			}
	}

	
private:
	uint8_t vPin,cPin;

};

	

#endif