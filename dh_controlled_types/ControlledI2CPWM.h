#ifndef DH_CONTROLLEDI2CPWM_H__
#define DH_CONTROLLEDI2CPWM_H__
#include "dh_controller.h"

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD


class ControlledI2CPWM: public Controller::Controlled{

public:
	ControlledI2CPWM(){

	}

	void setWire(i2c_t3* _wire){
		wire = _wire;
	}

	void setAddr(uint8_t _addr){
		_i2caddr = _addr;
	}

	void setPWMFreq(float freq) {
  //Serial.print("Attempting to set freq ");
  //Serial.println(freq);
  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

  uint8_t prescale = floor(prescaleval + 0.5);

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.

}

void setPWM(uint8_t num, uint16_t on, uint16_t off) {
  wire->beginTransmission(_i2caddr);
  wire->write(LED0_ON_L+4*num);
  wire->write(on);
  wire->write(on>>8);
  wire->write(off);
  wire->write(off>>8);
  wire->endTransmission();
}

// Sets pin without having to deal with on/off tick placement and properly handles
// a zero value as completely off.   Val should be a value from 0 to 4095 inclusive.
void setPin(uint8_t num, uint16_t val)
{
	if (num>25)
	   num = 25;
  // Clamp value between 0 and 4095 inclusive.
  val = min(val, 4095);
	vals[num] = val;
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
}


	void begin(void){
 		reset();
	}

void reset (){
	 write8(PCA9685_MODE1, 0x0);
}

	void execute(uint32_t time,uint32_t id,char command[]){

		switch(command[0]){
			case 'F':{
				  double sampleRate;
					uint16_t temp = 4;
					if (!Controller::parse_double(sampleRate,temp,command)){
						controller->getErrorLogger()->println("FRQ must be set to a number.");
						return;
					}
					setPWMFreq(sampleRate);
				break;
			}
		}
				controller->getErrorLogger()->print("Bad I2CPWM command:");
				controller->getErrorLogger()->println(command);
		}

	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
			return vals[addr.addr%26]/16;
	}

	uint16_t readU(ADDR1 addr,uint8_t addr2){
			return vals[addr.addr%26];
	}

	void write(ADDR1 addr,uint8_t val){
		uint8_t letter = addr.addr%26;
		setPin(letter,val*16);
	}

	void write(ADDR1 addr,uint16_t val){
		uint8_t letter = addr.addr%26;
		setPin(letter,val);
	}

private:

	uint8_t read8(uint8_t addr) {
	  wire->beginTransmission(_i2caddr);
	  wire->write(addr);
	  wire->endTransmission();

	  wire->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
	  return wire->read();
	}

	void write8(uint8_t addr, uint8_t d) {
	  wire->beginTransmission(_i2caddr);
	  wire->write(addr);
	  wire->write(d);
	  wire->endTransmission();
	}

	i2c_t3* wire;
  uint8_t _i2caddr = 0x40 ;

	uint16_t vals[26] = {0};
};



#endif
