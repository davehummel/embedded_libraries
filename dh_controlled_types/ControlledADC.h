#ifndef DH_CONTROLLEDADC_H__
#define DH_CONTROLLEDADC_H__
#include "dh_controller.h"

class ControlledADC: public Controller::Controlled{

public:
	ControlledADC(){

	}

	void begin(void){

	}

	void execute(uint32_t time,uint32_t id,char command[]){

		switch(command[0]){
      case 'P':{  // PIN
				uint8_t letter;
				uint8_t pID;
				if (command[4] >= 'a' && command[4] <= 'z'){
					letter = command[4] - 'a';
				}else if (command[4] >= 'A' && command[4] <= 'Z'){
					letter = command[4] - 'A';
				} else {
					controller->getErrorLogger()->println("Pin must be assigned to 'a' to 'z' var: PIN A 12");
					return;
				}
				uint16_t temp = 6;
				if (!Controller::parse_uint8(pID,temp,command)){
					controller->getErrorLogger()->println("Must be set to a pin: PIN A 12");
					return;
				}
					pinID[letter] = pID;

					if (pID!=0){
						pinMode(pID,INPUT);
					}
				break;}
			case 'R':{//RES
				  uint8_t  gainInput;
					uint16_t temp = 4;
					if (!Controller::parse_uint8(resolution,temp,command)){
						controller->getErrorLogger()->println("RES must be followed by a number.");
						return;
					}
          if (resolution<8 || resolution > 13){
            controller->getErrorLogger()->println("RES must be in the range of [8 to 13].");
            return;
          }
        analogReadResolution(resolution);
				break;
			}
		}
				controller->getErrorLogger()->print("Expected Res, Bad ADC command:");
				controller->getErrorLogger()->println(command);
		}

	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}

	uint16_t readU(ADDR1 addr,uint8_t addr2){
		uint8_t channel = addr.addr%26; // A = 0, B = 1, C = 2, D = 3
    if (pinID[channel]==0){
			controller->getErrorLogger()->print("Channel to read must be mapped to a pin number like PIN A 12");
			return 0;
		}

		return analogRead(pinID[channel]);
	}

  uint8_t readB(ADDR1 addr,uint8_t addr2){
		uint8_t channel = addr.addr%26; // A = 0, B = 1, C = 2, D = 3
    if (pinID[channel]==0){
			controller->getErrorLogger()->print("Channel to read must be mapped to a pin number like PIN A 12");
			return 0;
		}

		return analogRead(pinID[channel]);
	}

private:


	uint8_t pinID[26] = {0};
	uint8_t resolution;
};



#endif
