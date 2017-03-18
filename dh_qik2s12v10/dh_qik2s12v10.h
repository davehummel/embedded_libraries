#ifndef DH_QIK2S12V10_H__
#define DH_QIK2S12V10_H__
#include "Stream.h"


class qik2s12v10 {
public:


	qik2s12v10(Stream* source){
		stream = source;
		//setPWMParm(0);
		//setMotorAccel(0, 0);
		isHalfRange = true;
	}


	void breaks(uint8_t m0,uint8_t m1){
		if (m0>127)
			m0 = 127;
		if (m1>127)
			m1 = 127;

		stream->write(134);
		stream->write(m0);
		stream->write(135);
		stream->write(m1);
		stream->flush();
	}

	void breaks0(uint8_t m0){
		if (m0>127)
			m0 = 127;

		stream->write(134);
		stream->write(m0);
	}

	void breaks1(uint8_t m1){
		if (m1>127)
			m1 = 127;

		stream->write(135);
		stream->write(m1);
	}

	void move(bool m0Forward, uint8_t m0Speed, bool m1Forward, uint8_t m1Speed){
		if (isHalfRange){
			m0Speed /= 2;
			m1Speed /= 2;
		}

		uint8_t base;
		if (m0Forward)
			base = 136;
		else
			base = 138;

		if (m0Speed<=127){
			stream->write(base);
			stream->write(m0Speed);
		} else{
			stream->write(base+1);
			stream->write(m0Speed-128);
		}

		if (m1Forward)
			base = 140;
		else
			base = 142;

		if (m1Speed<=127){
			stream->write(base);
			stream->write(m1Speed);
		} else{
			stream->write(base+1);
			stream->write(m1Speed-128);
		}
	}

	void getCurrent_150ma(uint8_t& m0,uint8_t& m1){

			stream->write(144);
			stream->write(145);

			stream->flush();

			for (uint8_t i =0; i < 50 ; i++){
				delayMicroseconds(5);
				if (stream->available()){
					m0= stream->read();
					break;
				}
				if (i == 49){
					Serial.println("Failed to get M0 Current!");
				}
			}

			for (uint8_t i =0; i < 50 ; i++){

				if (stream->available()){
					m1= stream->read();
					break;
				}
				delayMicroseconds(5);
				if (i == 49){
					Serial.println("Failed to get M1 Current!");
				}
			}
	}

	void getSpeed(uint8_t& m0,uint8_t& m1){

			stream->write(146);
			stream->write(147);

			stream->flush();

			for (uint8_t i =0; i < 50 ; i++){
			delayMicroseconds(5);
				if (stream->available()){
					m0= stream->read();
					break;
				}
				if (i == 49){
					Serial.println("Failed to get M0 Speed!");
				}
			}

			for (uint8_t i =0; i < 50 ; i++){
				if (stream->available()){
					m1= stream->read();
					break;
				}
				if (i == 49){
					Serial.println("Failed to get M1 Speed!");
				}
				delayMicroseconds(5);
			}
	}


	void setPWMParm(uint8_t mode){
		writeConfig(1,mode);
	}

	void setShutDownErrorMode(bool onSerial,bool onOverCurrent, bool onMotorFault){
		writeConfig(2,onSerial+(onOverCurrent<<1)+(onMotorFault<<2));
	}

	void setTimeOut(uint8_t quarterSecs, uint8_t squaredMultiplier){
		if (quarterSecs>15)
			quarterSecs = 15;
		if (squaredMultiplier>7){
			squaredMultiplier=7;
		}

		writeConfig (3,quarterSecs+(squaredMultiplier<<4));
	}

	void setMotorAccel(uint8_t m0,uint8_t m1){
		if (m0>127)
			m0 = 127;
		if (m1>127)
			m1 = 127;

		writeConfig(4,m0);
		writeConfig(5,m1);
	}

	void setBreakDuration_10ms(uint8_t m0,uint8_t m1){
		if (m0>127)
			m0 = 127;
		if (m1>127)
			m1 = 127;

		writeConfig(6,m0);
		writeConfig(7,m1);
	}

	void setCurrentLimit_300ma(uint8_t m0,uint8_t m1){
		if (m0>127)
			m0 = 127;
		if (m1>127)
			m1 = 127;

		writeConfig(8,m0);
		writeConfig(9,m1);
	}

	void setCurrentLimitResponse(uint8_t m0,uint8_t m1){
		if (m0>127)
			m0 = 127;
		if (m1>127)
			m1 = 127;

		writeConfig(10,m0);
		writeConfig(11,m1);
	}

	uint8_t getErrorByte(){
		while (stream->available()){
			Serial1.print("Pre existing data");
			stream->read();
		}
		stream->write(130);
		stream->flush();
		for (uint8_t i =0; i < 100 ; i++){
			delayMicroseconds(10);
			if (stream->available()){
				return stream->read();
			}
		}

		Serial.println("Failed to get error response!");
		return 0;
	}

	uint8_t getConfigValue(uint8_t parm){
		if (parm>11)
			return 0;
		stream->write(131);
		stream->write(parm);
		stream->flush();
		for (uint8_t i =0; i < 100 ; i++){
			delayMicroseconds(10);
			if (stream->available()){
				return stream->read();
			}
		}
		Serial.println("Failed to get getConfig response!");
		return 0;
	}

	uint8_t writeConfig(uint8_t configID, uint8_t value){
		Serial.print("Config ");
		Serial.print(configID);
		Serial.print(' ');
		Serial.println(value);

		stream->write(132);
		stream->write(configID);
		stream->write(value);
		stream->write(85);
		stream->write(42);

		stream->flush();

		for (uint8_t i =0; i < 100 ; i++){
			delayMicroseconds(100);
			if (stream->available()){
				return stream->read();
			}
		}
		Serial.println("Failed to get setConfig response!");
		return 0;

	}



private:
	bool isHalfRange = false;


	Stream* stream;

};

#endif
