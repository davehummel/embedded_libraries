#ifndef DH_CONTROLLEDI2CADC_H__
#define DH_CONTROLLEDI2CADC_H__
#include "dh_controller.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define ADS1015_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)
/*=========================================================================*/

/*=========================================================================
    CONVERSION DELAY (in mS)
    -----------------------------------------------------------------------*/
    #define ADS1115_CONVERSIONDELAY         (8)
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_POINTER_MASK        (0x03)
    #define ADS1015_REG_POINTER_CONVERT     (0x00)
    #define ADS1015_REG_POINTER_CONFIG      (0x01)
    #define ADS1015_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1015_REG_POINTER_HITHRESH    (0x03)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1015_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1015_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1015_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1015_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1015_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1015_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1015_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1015_REG_CONFIG_DR_MASK      (0x00E0)
    #define ADS1015_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1015_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1015_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1015_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1015_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1015_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1015_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1015_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1015_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1015_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1015_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1015_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1015_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1015_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1015_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1015_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1015_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1015_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1015_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1015_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/

class ControlledI2CADC: public Controller::Controlled{

public:
	ControlledI2CADC(){

	}

	void setWire(i2c_t3* _wire){
		wire = _wire;
	}

	void setAddr(uint8_t _addr){
		_i2caddr = _addr;
	}

	void begin(void){

	}

  uint16_t  readADC_SingleEnded(uint8_t channel){

		if (channel > 3)
	  {
	    return 0;
	  }

	  // Start with default values
	  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
	                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
	                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
	                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
	                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

	  // Set PGA/voltage range
	  config |= gain;

	  // Set single-ended input channel
	  switch (channel)
	  {
	    case (0):
	      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
	      break;
	    case (1):
	      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
	      break;
	    case (2):
	      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
	      break;
	    case (3):
	      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
	      break;
	  }

	  // Set 'start single-conversion' bit
	  config |= ADS1015_REG_CONFIG_OS_SINGLE;

	  // Write config register to the ADC
	  writeRegister(_i2caddr, ADS1015_REG_POINTER_CONFIG, config);

	  // Wait for the conversion to complete
	  delay(ADS1115_CONVERSIONDELAY);

	  // Read the conversion results
	  uint16_t temp =  readRegister(_i2caddr, ADS1015_REG_POINTER_CONVERT);
   Serial.print(">");
      Serial.println(temp);
      return temp;
	}


	void execute(uint32_t time,uint32_t id,char command[]){

		switch(command[0]){
			case 'G':{//GAIN
				  uint8_t  gainInput;
					uint16_t temp = 5;
					if (!Controller::parse_uint8(gainInput,temp,command)){
						controller->getErrorLogger()->println("GAIN must be set to a number.");
						return;
					}
					switch (gainInput) {
						case 0: gain = ADS1015_REG_CONFIG_PGA_6_144V; break;
						case 1: gain = ADS1015_REG_CONFIG_PGA_4_096V; break;
						case 2: gain = ADS1015_REG_CONFIG_PGA_2_048V; break;
						case 4: gain = ADS1015_REG_CONFIG_PGA_1_024V; break;
						case 8: gain = ADS1015_REG_CONFIG_PGA_0_512V; break;
						case 16: gain = ADS1015_REG_CONFIG_PGA_0_256V; break;
						default : 		controller->getErrorLogger()->print("Bad GAIN level (0,1,2,4,8,16) required");
					}

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

	uint16_t readU(ADDR1 addr,uint8_t addr2){
		uint8_t channel = addr.addr%26; // A = 0, B = 1, C = 2, D = 3

		if (channel>3){
			controller->getErrorLogger()->print("Channel to read must be A,B,C,D");
			return 0;
		}

		return readADC_SingleEnded(channel);
	}

private:




/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
 void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) {
  wire->beginTransmission(i2cAddress);
  wire->write((uint8_t)reg);
  wire->write((uint8_t)(value>>8));
  wire->write((uint8_t)(value & 0xFF));
  wire->endTransmission();
}

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
*/
/**************************************************************************/
uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) {
  wire->beginTransmission(i2cAddress);
  wire->write(ADS1015_REG_POINTER_CONVERT);
  wire->endTransmission();
  wire->requestFrom(i2cAddress, (uint8_t)2);
  return ((wire->read() << 8) | wire->read());
}

	i2c_t3* wire;
  uint8_t _i2caddr = ADS1015_ADDRESS ;
	uint16_t gain;
};



#endif
