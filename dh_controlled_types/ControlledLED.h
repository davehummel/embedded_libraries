#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include "dh_controller.h"

#define LED_ID 1920

class ControlledLED: public Controller::Controlled{

public:
	ControlledLED(){

	}

	void begin(void){

	}
	void execute(uint32_t time,uint32_t id,char command[]){
		bool isPWM = true;
		bool isBLK = false;
		bool isDSC = false;
		uint16_t temp;
		switch (command[0]){
			case 'D':
			isDSC = true;
			case 'B':
				isPWM = false;
				if (command[1] == 'L')
					isBLK = true;
			case 'P':{
				uint8_t letter;
				uint8_t pID;
				if (command[4] >= 'a' && command[4] <= 'z'){
					letter = command[4] - 'a';
				}else if (command[4] >= 'A' && command[4] <= 'Z'){
					letter = command[4] - 'A';
				} else {
					controller->getErrorLogger()->setParseError(command,4,"PWM/Bin must be assigned to 'a' to 'z' var.");
					controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
					return;
				}
				temp = 6;
				if (!Controller::parse_uint8(pID,temp,command)){
					controller->getErrorLogger()->setParseError(command,temp,"PWM/Bin must be set to a pin");
				 controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
					return;
				}

				if (isBLK){
					if ( (command[temp]=='\0') || !Controller::parse_uint16(blkDur[letter],++temp,command)){
						controller->getErrorLogger()->setParseError(command,temp,"if BLK mode, a second duration number must be provided (uint16)");
						controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
						return;
					}
				}else {
					blkDur[letter] = 0;
				}

					pinID[letter] = pID;
					pinVal[letter] = 0;
					pinPWM[letter] = isPWM;
					pinDSC[letter] = isDSC;


					if (pID!=0){

						if (isPWM){
							pinMode(pID,OUTPUT);
							analogWrite(pID,0);
						}else if (isDSC){
							pinMode(pID,INPUT);
					 }else{
						 		pinMode(pID,OUTPUT);
						  digitalWrite(pID,LOW);
						}
					}
				break;}
			case 'K':{ // this is self scheduled to powerr BLK functionality
				uint8_t letter = id - LED_ID;
				if (pinDSC[letter]){
					pinMode(pinID[letter],INPUT);
				}else {
					digitalWrite(pinID[letter],LOW);
				}
				return;
			}
			case 'F':{
				uint8_t pin;
			  uint32_t sampleRate;
				temp = 4;
				if (!Controller::parse_uint8(pin,temp,command)){
					controller->getErrorLogger()->println("FRQ must be set to a number.");
										controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
					return;
				}
				temp++;
				if (!Controller::parse_uint32(sampleRate,temp,command)){
					controller->getErrorLogger()->println("FRQ must be set to a number.");
										controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
					return;
				}
				setFreq(pin,sampleRate);
			break;}
			case 'R':{
				uint8_t resolution;
				temp = 4;
				if (!Controller::parse_uint8(resolution,temp,command)){
					controller->getErrorLogger()->println("RES must be set to a number.");
										controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
					return;
				}
				if (resolution<2)
					resolution = 2;
				if (resolution>16)
					resolution = 16;
				analogWriteResolution(resolution);
			break;}
			default:
				controller->getErrorLogger()->print("Bad ControlledLED command:");
				controller->getErrorLogger()->println(command);
									controller->getErrorLogger()->finished(time,ErrorLogger::MOD_PARSER);
		}
	}
	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}

	uint8_t readConB(ADDR1 addr,uint8_t addr2){
			return pinVal[addr.addr%26];
	}
	uint16_t readConU(ADDR1 addr,uint8_t addr2){
			return pinVal[addr.addr%26];
	}
	uint32_t readConT(ADDR1 addr,uint8_t addr2){
			return pinVal[addr.addr%26];
	}


	void writeCon(ADDR1 addr,uint8_t val){
		uint8_t letter = addr.addr%26;
		if (pinID[letter] == 0)
			return;
		if (pinPWM[letter]) {
			analogWrite(pinID[letter],pinVal[letter]=val);
		}else{
			if (pinDSC[letter]){
				if (val==0){
				  pinMode(pinID[letter],INPUT);
				}else{
					pinMode(pinID[letter],OUTPUT);
					digitalWrite(pinID[letter],HIGH);
				}
			}else{
				digitalWrite(pinID[letter],(pinVal[letter]=(val!=0)));
			}
			if (val > 0 && blkDur[letter]>0){
						controller->schedule(LED_ID+letter,blkDur[letter],0,false,1,Controller::newString("K"),id,false);
			}
		}
	}

	void writeCon(ADDR1 addr,uint16_t val){
		uint8_t letter = addr.addr%26;
		if (pinID[letter] == 0)
			return;
		if (pinPWM[letter]) {
			analogWrite(pinID[letter],pinVal[letter]=val);
		}else{
			writeCon(addr,(uint8_t) val);
		}
	}

private:
	void setFreq(uint8_t pin,uint32_t freq){
		if (pin == 0){
			analogWriteFrequency(5, freq);
			analogWriteFrequency(3, freq);
			analogWriteFrequency(25, freq);
		} else {
			analogWriteFrequency(pin, freq);
		}
	}

	uint8_t pinID[26] = {0};
	uint32_t pinVal[26] = {0};
	uint16_t blkDur[26] = {0};
	bool pinPWM[26] = {false};
	bool pinDSC[26] = {false};

};



#endif
