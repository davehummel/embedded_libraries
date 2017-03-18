#ifndef DH_CONTROLLEDPAN_H__
#define DH_CONTROLLEDPAN_H__


#include "dh_controller.h"
#include <dh_ax-12a.h>

#define TRI_STATE_PIN 6
#define ENABLE_PIN 26

class ControlledPanTilt: public Controller::Controlled{
public:
	const uint16_t INITDELAY = 50;

	void begin(void){
		 Serial3.begin(1000000); // change this to 1000000 if you havent changed default baud

		 pinMode(ENABLE_PIN, OUTPUT);
		 digitalWrite(ENABLE_PIN, enabled);

		 servo = new ax_12a(&Serial3,TRI_STATE_PIN,2);
	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
			if (!enabled)
				return 0;
			switch(addr.addr%26+'A'){
				case 'M': return servo->askMoving(addr2+1);
				case 'E': return enabled;
				default: return -1;
			}
	}

	uint16_t readU(ADDR1 addr,uint8_t addr2){
			if (!enabled)
				return 0;
			switch(addr.addr%26+'A'){
				case 'P':{
				uint16_t test =   servo->askPos(addr2+1); // Position
				return test;
			}
				case 'G': return servo->askGoal(addr2+1); // Goal
				case 'C':{ //CWL or CCL
					bool isCCW =  ((addr.addr/26)%26+'A') == 'C';
					return servo->askAngleLimit(isCCW,addr2+1);
				}
				case 'O': return enabled;
				default: return 0;
			}
	}

	float readF(ADDR1 addr,uint8_t addr2){
			if (!enabled)
				return 0;
			switch(addr.addr%26+'A'){
				case 'S': return servo->askSpeed(addr2+1); // Position
				case 'V': return servo->askVoltage(addr2+1); // Goal
				default: return -1;
			}
	}

	void write(ADDR1 addr,uint8_t val){
		switch(addr.addr%26+'A'){
			case 'E':
				enabled = val == 1;
				digitalWrite(ENABLE_PIN,enabled);
				break;
			default: break;
		}
	}

	void write(ADDR1 addr,uint16_t val){
		switch(addr.addr%26+'A'){
			case 'P':		;  servo->move(1,val);return;
			case 'T':      servo->move(2,val);return;
		}
	}

	void execute(uint32_t time,uint32_t id,char command[]){


	}

	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}



private:
	bool enabled = true;
	ax_12a *servo;
};



#endif
