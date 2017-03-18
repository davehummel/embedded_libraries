#ifndef DH_CONTROLLEDMOTOR_H__
#define DH_CONTROLLEDMOTOR_H__
#include "dh_controller.h"
#include "dh_qik2s12v10.h"

class ControlledMotor: public Controller::Controlled{
public:

	ControlledMotor(bool flip,uint8_t _resetPin){
		flipMotors = flip;
		resetPin = _resetPin;
		pinMode(resetPin,OUTPUT);
		digitalWrite(resetPin,0);
		delay(100);
		digitalWrite(resetPin,1);
	}

	void begin(){
		if (!motor){
			Serial2.begin(115200);
			motor = new qik2s12v10(&Serial2);
		}
	}
	
	uint8_t readB(ADDR1 addr,uint8_t addr2){
		char command = addr.addr%26+'A';
		bool isLeft = ((addr.addr/26)%26+'A') == 'L';
		if (flipMotors)
			isLeft = !isLeft;
		switch(command){
				case 'B': 
					if (isLeft)
						return m0Break;
					else
						return m1Break;
				case 'E':
					uint32_t now = millis(); // Keep from repeatedly reading the error byte
					if (now!=readErrorTime){
						readErrorTime = now;
						error = motor->getErrorByte();
					}
					return error;
			
		}
			return 0;
	}
	
	int16_t readI(ADDR1 addr,uint8_t addr2){
		char command = addr.addr%26+'A';
		bool isLeft = ((addr.addr/26)%26+'A') == 'L';
		if (flipMotors)
			isLeft = !isLeft;
		switch(command){
				case 'T': 
					return throttle;
				case 'D':
					return differential;
				case 'S':
					uint32_t now = millis();
					if (now!=readSpeedTime){
						readSpeedTime = now;
						motor->getSpeed(m0Speed,m1Speed);
					}
					if (isLeft)
						return m0F?m0Speed:-m0Speed;
					else
						return m1F?m1Speed:-m1Speed;
		}
		return -1;
	}
	
	uint16_t readU(ADDR1 addr,uint8_t addr2){
		char command = addr.addr%26+'A';
		bool isLeft = ((addr.addr/26)%26+'A') == 'L';
		if (flipMotors)
			isLeft = !isLeft;
		switch(command){
				case 'C':
					uint32_t now = millis();
					if (now!=readErrorTime){
						readCurrentTime = now;
						motor->getCurrent_150ma(m0Current,m1Current);
					}

					if (isLeft)
						return m0Current*150;
					else
						return m1Current*150;
		}
			return -1;
	}

	
	void write(ADDR1 addr,uint8_t val){
		char command = addr.addr%26+'A';
		bool isLeft = ((addr.addr/26)%26+'A') == 'L';
		if (flipMotors)
			isLeft = !isLeft;
		switch(command){
			case 'B': 
				if(isLeft){
					motor->breaks0(m0Break = val);
					m0Speed = 0;
				}else{
					motor->breaks1(m1Break = val);
					m1Speed = 0;
				}
			break;
			default: break;
		}
	}
	
	void write(ADDR1 addr,int16_t val){
		char command = addr.addr%26+'A';
		switch(command){
			case 'T': 
				throttle = val;
				setMotor();
				break;
			case 'D': 
				differential = val;
				setMotor();
				break;
			break;
			default: break;
		}
	}
	
	

	void execute(uint32_t time,uint32_t id,char command[]){
		
	}
	

	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}

	void setMotor(){
		m0Break = 0;
		m1Break = 0;
		if (throttle > 255)
			throttle = 255;
		else if (throttle < -255)
			throttle = 255;

		if (differential>512)
			differential = 512;
		else if (differential < -512)
			differential = -512;


		int16_t a, b;
		if (differential == 1){
			a = throttle - 1;
			b = throttle;
		}
		else if (differential == -1){
			a = throttle;
			b = throttle - 1;
		}
		else {
			a = throttle - differential / 2;
			b = throttle + differential / 2;
		}

		if (a > 255){
			b = b + 255 - a;
			a = 255;
		}
		else if (b > 255) {
			a = a + 255 - b;
			b = 255;
		}
		else if (a < -255){
			b = b - a - 255;
			a = -255;
		}
		else if (b < -255){
			a = a - b - 255;
			b = -255;
		}

		if (a > 255)
			a = 255;
		else if (a < -255)
			a = -255;
		if (b > 255)
			b = 255;
		else if (b < -255)
			b = -255;

		if (a < 0){
			m0Speed = -a;
			m0F = false; 
		}
		else {
			m0Speed = a; 
			m0F = true;
		}

		if (b < 0){
			m1Speed = -b;
			m1F = false;
		}
		else {
			m1Speed = b;
			m1F = true;
		}
		
		if (flipMotors){
			uint8_t temp = m0Speed;
			m0Speed = m1Speed;
			m1Speed = temp;
			temp = m0F;
			m0F = m1F;
			m1F = temp;
		}
		updateMotor();
	}

	void updateMotor(){
		motor->move(m0F,m0Speed,m1F,m1Speed);
		#ifdef DEBUG
		Serial.print("Motor:");
		Serial.print(m0F);
		Serial.print(",");
		Serial.print(m0Speed);
		Serial.print(",");
		Serial.print(m1F);
		Serial.print(",");
		Serial.println(m1Speed);
		#endif
	}

	void stopMotor(){
		motor->breaks(m0Break=255,m1Break=255);
		m0Speed=m1Speed=0;
	}

	uint8_t m0Break=0,m1Break=0;
	uint8_t m0Current=0,m1Current=0;
	uint8_t m0Speed=0,m1Speed=0;
	bool m0F,m1F;
	
	int16_t throttle,differential;


	uint8_t error;

	uint32_t readSpeedTime = 0;
	uint32_t readCurrentTime = 0;
	uint32_t readErrorTime = 0;
	
private:
	bool flipMotors = false;
	uint8_t resetPin;
	qik2s12v10 *motor;

};



	

#endif