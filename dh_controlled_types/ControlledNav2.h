
#ifndef DH_CONTROLLEDNAV_H__
#define DH_CONTROLLEDNAV_H__
#include "dh_controller.h"
 #include <ControlledMotor.h>
 #include <ControlledBN055.h>

class ControlledNav2: public Controller::Controlled{

public: 

	void begin(void){
		gyro = (ControlledBN055*) controller->getControlled('G');
	}
	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){

		int16_t velocity = getVelocity(time);
		
	}
	void serialize(Logger* logger, uint32_t id, char command[]) {
	
	}
	
	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}
	
private:
	ControlledBN055* gyro;


};

	


#endif