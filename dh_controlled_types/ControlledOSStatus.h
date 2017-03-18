#ifndef DH_CONTROLLEDOSSTATUS_H__
#define DH_CONTROLLEDOSSTATUS_H__
#include "dh_controller.h"

const uint16_t INP = ADDR1::solveADDR("INP");
const uint16_t ETX = ADDR1::solveADDR("ETX");
const uint16_t LTX = ADDR1::solveADDR("LTX");
const uint16_t LST = ADDR1::solveADDR("LST");
const uint16_t ETM = ADDR1::solveADDR("ETM");
const uint16_t TIM = ADDR1::solveADDR("TIM");
const uint16_t ECD = ADDR1::solveADDR("ECD");

class ControlledOSStatus: public Controller::Controlled{

public:
	ControlledOSStatus(){
	}

	void begin(void){


	}
	void execute(uint32_t time,uint32_t id,char command[]){
  	if (command[0] == 'C' && command[1] == 'E'){
				controller->getErrorLogger()->clearError();
				return;
		}
		if (command[0] == 'S' && command[1] == 'E'){
				Logger* logger = controller->getLogger();
				logger->startBatchSend('Z',id);

				logger->print(controller->getErrorLogger()->getErrorTime());
				logger->print(controller->getErrorLogger()->getErrorText(),0);

				logger->batchSend();
				return;
		}
	}
	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}


	uint8_t readB(ADDR1 addr,uint8_t addr2){
	//	if (addr.addr == ECD) return controller->getErrorCode();
		return 0 ;
	}
	//
	// float readF(ADDR1 addr,uint8_t addr2){
	//
	// }
	//
	uint32_t readT(ADDR1 addr,uint8_t addr2){
			if (addr.addr == ETM) return controller->getErrorLogger()->getErrorTime();
			if (addr.addr == TIM) return controller->lastProcessedMSTime;
			return 0;
	}

	char* readS(ADDR1 addr,uint8_t addr2){

			if (addr.addr == INP) return controller->getInputBuffer();

			if (addr.addr == ETX) return controller->getErrorLogger()->getErrorText();

		  if (addr.addr == LST) return controller->lastProcessedLine;

		return 0;
	}



private:


};



#endif
