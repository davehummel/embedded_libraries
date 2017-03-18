#ifndef DH_CONTROLLEDSPEK_H__
#define DH_CONTROLLEDSPEK_H__
#include "dh_controller.h"

#define MASK_2048_CHANID 0x7800
#define MASK_2048_SXPOS 0x07FF
#define MASK_2048_PHASE 0x8000

#define RADIO_SCHED_ID 290

class ControlledSpek: public Controller::Controlled{
public:

	ControlledSpek(Stream* _in,uint16_t millisPerRead,int _powerPin = -1){
		powerPin = _powerPin;
		in=_in;
		readDelay = millisPerRead;
		if (powerPin > -1){
				pinMode(powerPin,OUTPUT);
		}
	}

	void begin(){

				in->flush();
			 controller->schedule(RADIO_SCHED_ID,readDelay,readDelay,false,0,Controller::newString("R"),'R',false);
	}


	uint16_t readU(ADDR1 addr,uint8_t addr2){
		uint8_t command = addr.addr%26;

		if (command<8){
			return lastValue[command];
		}
			return -1;
	}

	void write(ADDR1 addr,uint16_t val){
		uint8_t command = addr.addr%26;

		if (command<8){
			 lastValue[command] = val;
		}
	}

	void write(ADDR1 addr,uint8_t val){
					digitalWrite(powerPin,val>0);
	}

	void execute(uint32_t time,uint32_t id,char command[]){
		if (command[0]=='R' ){
				if (in->available()>=16){
					processInput();
				}
		}
	}


	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}



private:

	union REMOTE_FRAME{
	  struct {
	  uint16_t fades;
	  uint16_t servo[7];
	  } data;
	  uint8_t raw[16];
	};

	boolean parseServo(uint16_t input, uint8_t &channel, uint16_t &value){
	  channel = (input&MASK_2048_CHANID)>>11;
	  value = (input&MASK_2048_SXPOS);
	  return (input&MASK_2048_PHASE)!=0;
	}

  void processInput(){
		REMOTE_FRAME frame;
		for (uint8_t i = 0 ;i <8; i ++ ){
			uint8_t temp[2];

			in->readBytes(temp ,2);
		 frame.raw[i*2+0] =temp[1];
		 frame.raw[i*2+1] = temp[0];

		}
    if (frame.raw[14]!=255 || frame.raw[15]!=255){
			Serial.println("Trashing radio data");
		     while (in->available())
				 			in->read();
          return;
			 }


		uint8_t channel=0;
		uint16_t value=0;
		for (uint8_t x = 0 ; x<7 ; x++){
						parseServo(frame.data.servo[x],channel,value);
						if (channel < 7){
							lastValue[channel] = value;
						}
		}
	}

	Stream *in;
	uint8_t powerPin;
	uint16_t lastValue[7];
	uint16_t readDelay;
};





#endif
